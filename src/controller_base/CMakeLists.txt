##########################################################################################
#
# Copyright @ 2018 AUDI AG. All rights reserved.
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
##########################################################################################
find_package(a_util REQUIRED)
find_package(Clara REQUIRED)

add_library(fep_controller_base STATIC
    controller.cpp
    ${PROJECT_SOURCE_DIR}/include/controller_base/controller.h
)

target_include_directories(fep_controller_base PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_property(TARGET fep_controller_base PROPERTY FOLDER libraries)

target_link_libraries(fep_controller_base PRIVATE a_util Clara)
target_link_libraries(fep_controller_base PUBLIC fep_meta_model)

set_target_properties(fep_controller_base PROPERTIES
    OUTPUT_NAME fep_controller_base
    DEBUG_POSTFIX "d${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    RELEASE_POSTFIX "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    RELWITHDEBINFO_POSTFIX "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

if(WIN32)
    target_compile_options(
        fep_controller_base 
        PRIVATE 
             "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/Z7>"
    )
endif(WIN32)

install(TARGETS fep_controller_base
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION bin)

configure_file(fep_controller_base-config.cmake.in ${CMAKE_BINARY_DIR}/fep_controller_base-config.cmake @ONLY)
file(INSTALL ${CMAKE_BINARY_DIR}/fep_controller_base-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX})



###############################################################################
add_executable(tester_controller_base tester_controller_base.cpp)
add_test(NAME tester_controller_base
         COMMAND tester_controller_base
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tester_controller_base PRIVATE fep_controller_base a_util GTest::Main)
set_property(TARGET tester_controller_base PROPERTY FOLDER tests/private)
