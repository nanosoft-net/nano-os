####################################################################################################
# \file makefile.inc
# \brief  Makefile for the include files of demo_lpc43s37-m4 application
# \author C. Jimenez
# \copyright Copyright(c) 2017 Cedric Jimenez
#
# This file is part of Nano-OS.
#
# Nano-OS is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Nano-OS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Nano-OS.  If not, see <http://www.gnu.org/licenses/>.
####################################################################################################

# Application directory
APPLICATION_DIR := $(ROOT_DIR)/src/apps/demo_lpc43s37

# Source directories
SOURCE_DIRS := $(APPLICATION_DIR)/demo_lpc43s37-m4  \
               $(APPLICATION_DIR)/common
              
# Project specific include directories
PROJECT_INC_DIRS := $(PROJECT_INC_DIRS) \
                    $(SOURCE_DIRS)
