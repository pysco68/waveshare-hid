/* This provides unification of code over GD32F subfamilies */

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

#include <libopencm3/cm3/common.h>
#include <libopencm3/gd32/memorymap.h>

#if defined(GD32F0)
#       include <libopencm3/gd32/f0/syscfg.h>
#elif defined(GD32F2)
#       include <libopencm3/gd32/f2/syscfg.h>
#elif defined(GD32F3)
#       include <libopencm3/gd32/f3/syscfg.h>
#elif defined(GD32F4)
#       include <libopencm3/gd32/f4/syscfg.h>
#elif defined(GD32L0)
#       include <libopencm3/gd32/l0/syscfg.h>
#elif defined(GD32L1)
#       include <libopencm3/gd32/l1/syscfg.h>
#else
#       error "gd32 family not defined."
#endif

