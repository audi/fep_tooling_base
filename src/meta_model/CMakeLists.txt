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


add_library(fep_meta_model STATIC
    meta_model_loader.cpp
    ${PROJECT_SOURCE_DIR}/include/meta_model/meta_model_loader.h
    meta_model_writer.cpp
    ${PROJECT_SOURCE_DIR}/include/meta_model/meta_model_writer.h
    meta_model_checker.cpp
    ${PROJECT_SOURCE_DIR}/include/meta_model/meta_model_checker.h
    meta_model_configuration.cpp
    ${PROJECT_SOURCE_DIR}/include/meta_model/meta_model_configuration.h
    ${PROJECT_SOURCE_DIR}/include/meta_model/meta_model_types.h
	${PROJECT_SOURCE_DIR}/include/meta_model/meta_model_version.h
)

target_include_directories(fep_meta_model PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(fep_meta_model PUBLIC a_util)

set_property(TARGET fep_meta_model PROPERTY FOLDER libraries)

set_target_properties(fep_meta_model PROPERTIES
    OUTPUT_NAME fep_meta_model
    DEBUG_POSTFIX "d${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    RELEASE_POSTFIX "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    RELWITHDEBINFO_POSTFIX "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

if(WIN32)
    target_compile_options(
        fep_meta_model 
        PRIVATE 
             "$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/Z7>"
    )
endif(WIN32)

install(TARGETS fep_meta_model
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION bin)

configure_file(fep_meta_model-config.cmake.in ${CMAKE_BINARY_DIR}/fep_meta_model-config.cmake @ONLY)
file(INSTALL ${CMAKE_BINARY_DIR}/fep_meta_model-config.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/)

###################################################################################################
# private tests
add_executable(tester_meta_model tester_meta_model.cpp)
add_test(NAME tester_meta_model
         COMMAND tester_meta_model
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tester_meta_model PRIVATE fep_meta_model GTest::Main)
set_property(TARGET tester_meta_model PROPERTY FOLDER tests/private)
