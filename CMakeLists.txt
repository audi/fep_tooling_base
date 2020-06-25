##########################################################################################
#
# Copyright @ 2018 Audi AG. All rights reserved.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# If it is not possible or desirable to put the notice in a particular file, then
# You may include the notice in a location (such as a LICENSE file in a
# relevant directory) where a recipient would be likely to look for such a notice.
#
# You may add additional accurate notices of copyright ownership.
#
# QNX support Copyright 2019 by dSPACE GmbH. All Rights Reserved.
#
##########################################################################################

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0011 NEW)

# Disable extensions here and require the chosen CMAKE_CXX_STANDARD (coming from Conan)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

project(fep_tooling_base VERSION 1.3.0)

# Enable strict compiler warnings
if(MSVC)
    add_compile_options(/W4 /wd4100 /wd4251 /wd4505 /wd4290 /WX)
else()
    add_compile_options(-Wextra -pedantic -Wall -Wno-unknown-pragmas -Wno-reorder -Werror)
endif()

if(CONAN_COMPILER)
    if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
            include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    elseif (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
            include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
    elseif (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
            include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
    elseif (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
            include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
    else()
            message(FATAL_ERROR "Conan build info can't be found.")
    endif()

    if(CORTEX_WORKSPACE)
            conan_basic_setup(TARGETS)
    else()
            conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
    endif()
    find_package(GTest REQUIRED COMPONENTS GTest::Main PATHS ${CONAN_TESTING_ROOT})
else()
    find_package(GTest REQUIRED COMPONENTS GTest::Main)
endif()

if(NOT CMAKE_CROSSCOMPILING)
    enable_testing()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS True)

add_subdirectory(src)
add_subdirectory(doc)

# install content from include directory
file(INSTALL include/
     DESTINATION ${CMAKE_INSTALL_PREFIX}/include
     FILES_MATCHING PATTERN "*.h"
                    PATTERN ".svn" EXCLUDE)

###################################################################################################################
# INSTALL LICENSE INFORMATION !
###################################################################################################################
file(INSTALL doc/license DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
file(INSTALL doc/input/used_licenses.md doc/input/mpl.md
     DESTINATION ${CMAKE_INSTALL_PREFIX}/doc/license)
file(INSTALL README.md DESTINATION ${CMAKE_INSTALL_PREFIX})
