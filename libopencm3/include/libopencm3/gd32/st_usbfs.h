/* This provides unification of USB code for supported GD32F subfamilies */

/*
 * This file is part of the libopencm3 project.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBOPENCM3_ST_USBFS_H
#define LIBOPENCM3_ST_USBFS_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/gd32/memorymap.h>

#if defined(GD32F0)
#       include <libopencm3/gd32/f0/st_usbfs.h>
#elif defined(GD32F1)
#       include <libopencm3/gd32/f1/st_usbfs.h>
#elif defined(GD32F3)
#       include <libopencm3/gd32/f3/st_usbfs.h>
#elif defined(GD32L0)
#       include <libopencm3/gd32/l0/st_usbfs.h>
#elif defined(GD32L1)
#       include <libopencm3/gd32/l1/st_usbfs.h>
#else
#       error "GD32 family not defined or not supported."
#endif

#endif
