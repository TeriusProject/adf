# Makefile
# ------------------------------------------------------------------------
# ADF - Agriculture Data Format
# Copyright (C) 2024 Matteo Nicoli
#
# This file is part of Terius
#
# ADF is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# ADF is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

INCLUDE = /usr/local/include
LIB = /usr/local/lib

all: adf test

adf:
	@echo "*****************************\n  Building libadf\n*****************************"
	@$(MAKE) -C src/

.PHONY : test
test:
	@echo "*****************************\n  TESTS\n*****************************"
	@$(MAKE) -C test/

.PHONY : clean
clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C test clean
	@$(MAKE) -C api/cc clean

.PHONY: init
init:
	@$(MAKE) -C test init

.PHONY : install
# currently just install on macOS and GNU/Linux
install:
	@$(MAKE) -C src install

.PHONY : uninstall
# currently just uninstall on macOS and GNU/Linux
uninstall: 
	@$(MAKE) -C src uninstall

.PHONY: asm
asm:
	@$(MAKE) -C src asm

.PHONY : cpp
cpp:
	@$(MAKE) -C api/cc

.PHONY: cpp_install
# currently just install on macOS and GNU/Linux
cpp_install:
	@$(MAKE) -C api/cc install
