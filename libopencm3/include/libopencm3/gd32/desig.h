/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
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

#ifndef LIBOPENCM3_DESIG_H
#define LIBOPENCM3_DESIG_H

#include <libopencm3/cm3/common.h>
#include <libopencm3/gd32/memorymap.h>

/* --- Device Electronic Signature -------------------------------- */

/* Flash size register */
#define DESIG_FLASH_SIZE		MMIO16(DESIG_FLASH_SIZE_BASE + 0x00)

BEGIN_DECLS

/**
 * Read the on board flash size
 * @return flash size in KB
 */
uint16_t desig_get_flash_size(void);

/**
 * Read the full 96 bit unique identifier
 * Note: ST specifies that bits 31..16 are _also_ reserved for future use
 * @param result pointer to at least 3xuint32_ts (96 bits)
 */
void desig_get_unique_id(uint32_t result[]);

/**
 * Read the full 96 bit unique identifier and return it as a
 * zero-terminated string
 * @param string memory region to write the result to
 8 @param string_len the size of string in bytes
 */
void desig_get_unique_id_as_string(char *string,
				   unsigned int string_len);

END_DECLS

#endif
