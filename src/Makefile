##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##
## Modified for UKHAS 2014 Badge by Jon Sowman <jon@jonsowman.com>
##

BINARY = main

SOURCES = $(filter-out $(BINARY).c, $(wildcard *.c))
OBJS = $(SOURCES:.c=.o)

# Set this to f0 or f4 depending on which STM32 you're using.
include ../common/Makefile.gd32f103.include
