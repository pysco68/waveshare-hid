/**
    Open firmware for Waveshare 7" capacitive touchscreen 
    Copyright (C) 2016 Yannic Staudt / Staudt Engineering

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 */

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>
#include <stdlib.h>
#include "usb_device.h"

usbd_device *usbd_dev;
uint8_t usbd_control_buffer[128];   // Buffer to be used for control requests.

///
// This is the HID report descriptor for our touchscreen, operating in 
// the "single finger hybrid mode".
// This means we must send one HID report per recorded touch at a given
// time (on frame with 3 touches == 3 reports sent).
//
// A matching touch report must look like:
//
// [0]  Report ID (0x01, definded in the descriptor)
// [1]  State
// .......x: Tip switch
// xxxxxxx.: Ignored
// [2]  Contact identifier
// [3]  Pressure (0 - 255)
// [4]  X coordinate LSB (0 - 800)
// [5]  X coordinate MSB
// [6]  Y coordinate LSB (0 - 480)
// [7]  Y coordinate MSB
// [8]  Scan time LSB (0 - 65535)
// [9]  Scan time MSB
// [10] Contact count (for the serial hybrid / fill in 1st report per frame with the number of reports to come, then 0)
//
// Much information on the topic can be found at:
// https://msdn.microsoft.com/en-us/library/windows/hardware/dn672287(v=vs.85).aspx
//
static const uint8_t hid_report_descriptor[] = {
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)          
    0x09, 0x04,                         // USAGE (Touch Screen)             
    0xa1, 0x01,                         // COLLECTION (Application)         
    0x85, 0x01,                         //   REPORT_ID (Touch)              
    0x09, 0x22,                         //   USAGE (Finger)                 
    0xa1, 0x02,                         //     COLLECTION (Logical)  
    0x09, 0x42,                         //       USAGE (Tip Switch)           
    0x15, 0x00,                         //       LOGICAL_MINIMUM (0)          
    0x25, 0x01,                         //       LOGICAL_MAXIMUM (1)          
    0x75, 0x01,                         //       REPORT_SIZE (1)              
    0x95, 0x01,                         //       REPORT_COUNT (1)             
    0x81, 0x02,                         //       INPUT (Data,Var,Abs) 
    0x95, 0x07,                         //       REPORT_COUNT (7)  
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x26, 0xff, 0x00,                   //       LOGICAL_MAXIMUM (255)
    0x75, 0x08,                         //       REPORT_SIZE (8)
    0x95, 0x01,                         //       REPORT_COUNT (1)
    0x09, 0x51,                         //       USAGE (Contact Identifier)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x30,                         //       USAGE (Tip Pressure)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x75, 0x10,                         //       REPORT_SIZE (16)
    0x95, 0x01,                         //       REPORT_COUNT (1)
    0x05, 0x01,                         //       USAGE_PAGE (Generic Desktop)
    0x55, 0x0F,                         //       UNIT EXPONENT (-1)
    0x65, 0x11,                         //       UNIT VALUE (SI Linear:Distance)
    0x26, 0x20, 0x03,                   //       LOGICAL_MAXIMUM (800)
    0x35, 0x00,                         //       PHYSICAL_MINIMUM (0)
    0x46, 0xFF, 0xFF,                   //       PHYSICAL_MAXIMUM (x)
    0x09, 0x30,                         //       USAGE (X)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x46, 0xFF, 0xFF,                   //       PHYSICAL_MAXIMUM (y)
    0x26, 0xe0, 0x01,                   //       LOGICAL_MAXIMUM (480)
    0x09, 0x31,                         //       USAGE (Y)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)            
    0xc0,                               //    END_COLLECTION
    0x05, 0x0d,                         //    USAGE_PAGE (Digitizers)
    0x55, 0x0C,                         //    UNIT_EXPONENT (-4 == 100 microseconds)           
    0x66, 0x01, 0x10,                   //    UNIT (Seconds)        
    0x47, 0xff, 0xff, 0x00, 0x00,       //    PHYSICAL_MAXIMUM (65535)
    0x27, 0xff, 0xff, 0x00, 0x00,       //    LOGICAL_MAXIMUM (65535) 
    0x75, 0x10,                         //    REPORT_SIZE (16)             
    0x95, 0x01,                         //    REPORT_COUNT (1) 
    0x09, 0x56,                         //    USAGE (Scan Time)    
    0x81, 0x02,                         //    INPUT (Data,Var,Abs)     
    0x05, 0x0d,                         //    USAGE_PAGE (Digitizers)     
    0x09, 0x54,                         //    USAGE (Contact count)
    0x25, 0x7f,                         //    LOGICAL_MAXIMUM (127) 
    0x95, 0x01,                         //    REPORT_COUNT (1)
    0x75, 0x08,                         //    REPORT_SIZE (8)    
    0x81, 0x02,                         //    INPUT (Data,Var,Abs)
    0xc0                                // END_COLLECTION
};

///
// the usb device descriptor
// see http://www.beyondlogic.org/usbnutshell/usb5.shtml
static struct usb_device_descriptor dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x6666,         // prototype VID
    .idProduct = 0xBEEF,        // dummy
    .bcdDevice = 0x0200,
    .iManufacturer = 1,         //
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

///
// these are some usb strings rerefences in the
// usb descriptor. They're not really displayed anywhere
// in Windows, but might appear in syslog on Linux
// so put something "meaningful" in here
static const char *usb_strings[] = {
    "Staudt Engineering",
    "HID Touchscreen",
    "123456789",
};

///
// this mess describes the HID report device descriptor
// which gets included in the usb interface description 
// below
static struct {
    struct usb_hid_descriptor hid_descriptor;
    struct {
        uint8_t bReportDescriptorType;
        uint16_t wDescriptorLength;
    } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
    .hid_descriptor = {
        .bLength = sizeof(hid_function),
        .bDescriptorType = USB_DT_HID,
        .bcdHID = 0x0111,   // spec version 1.11
        .bCountryCode = 0x09, // germany
        .bNumDescriptors = 1,
    },
    .hid_report = {
        .bReportDescriptorType = USB_DT_REPORT,
        .wDescriptorLength = sizeof(hid_report_descriptor),
    },
};

///
// the next two structs describe the HID endpoint & interface
struct usb_endpoint_descriptor hid_endpoint = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = ENDPOINT_ADDRESS,
    .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize = 64,
    .bInterval = 0x01,      // ask for frequent polling for best touchscreen reactiveness
};

struct usb_interface_descriptor hid_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 0, /* boot */
    .bInterfaceProtocol = 0, /* mouse */
    .iInterface = 0,
    .endpoint = &hid_endpoint,
    .extra = &hid_function,
    .extralen = sizeof(hid_function),
};

struct usb_interface ifaces[] = {{
    .num_altsetting = 1,
    .altsetting = &hid_iface,
}};

///
// put everything together for the USB stack
struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0xC0,
    .bMaxPower = 0xfa,     // 500mA / Maximum power consumption in "2 mA" unit 
    .interface = ifaces,
};

///
// setup the usb peripheral
void setup_usb(void)
{
    usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
    usbd_register_set_config_callback(usbd_dev, hid_set_config);
}

///
// has to be called frequently in the main loop in order to
// service the host usb interrupts as required
void poll_usb(void)
{
    usbd_poll(usbd_dev);
}

///
// (re)send a single report (until it actually got over the wire)
void send_hid_report(uint8_t *hid_report, uint8_t len)
{
    // the usb driver doesn't provide a working report "buffer" so we have to
    // retry sending the report until we get succeed (return value eq length parameter<)
    while(usbd_ep_write_packet(usbd_dev, ENDPOINT_ADDRESS, hid_report, len) != len);
}

///
// handle USB controll requests
int hid_control_request(usbd_device *cb_usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
{
    (void)complete;
    (void)cb_usbd_dev;

    // we were just asked to send the HID report descriptor.
    if (req->bmRequestType == 0x81 && req->bRequest == USB_REQ_GET_DESCRIPTOR && req->wValue == 0x2200)
    {
        *buf = (uint8_t *)hid_report_descriptor;
        *len = sizeof(hid_report_descriptor);
        return 1;
    }

    return 0;
}

///
// set the device configuration for HID endpoint
// and register controll callback to handle usb 
// controll requests
void hid_set_config(usbd_device *cb_usbd_dev, uint16_t wValue)
{
    (void)wValue;
    (void)cb_usbd_dev;

    usbd_ep_setup(cb_usbd_dev, ENDPOINT_ADDRESS, USB_ENDPOINT_ATTR_INTERRUPT, 4, NULL);

    usbd_register_control_callback(
                cb_usbd_dev,                                        // usb device
                USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,     // request type
                USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_RECIPIENT,       // request mask  
                hid_control_request);                               // callback
}