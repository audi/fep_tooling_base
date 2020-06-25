/**
* @file
*
* @copyright
* @verbatim
Copyright @ 2018 AUDI AG. All rights reserved.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

If it is not possible or desirable to put the notice in a particular file, then
You may include the notice in a location (such as a LICENSE file in a
relevant directory) where a recipient would be likely to look for such a notice.

You may add additional accurate notices of copyright ownership.
@endverbatim
*/
#pragma once

#include <string>
#include "meta_model/meta_model_types.h"

namespace controller_base
{
    /**
    * This interface connects the abstract controller functionality (see controlSystem below) with a concrete
    * runtime implementation. Any middleware/system implementing this interface can be controlled.
    */
    class ControllerRuntimeInterface
    {
    public:
        virtual ~ControllerRuntimeInterface() {}

        /// Initializes a connection to the system - returns true if successful
        virtual bool connect(const meta_model::System& system, const std::string& name, bool verbose) = 0;

        /// Executes a command - returns true if successful
        virtual bool execute(const std::string& command, std::string& message) = 0;

        /// Deinitializes the connection to the system previously connected to
        virtual void disconnect() = 0;
    };

    /// argument struct that holds all arguments that can be parsed by controller
    struct ControllerArguments
    {
        std::string system_name;
        std::string system_description;
        std::vector<std::string> element_descriptions;
        std::vector<std::string> environment_variables;
        std::vector<std::string> commands;
        uint64_t waiting_timeout = 0;
        bool verbose = false;
    };
    /// Function that parses the arguments of controller and stores them in args variable
    int parseArguments(int argc, char* argv[], ControllerArguments& args, bool& should_exit);
    /// Controls a system based on the meta model semantics (command line interface) - returns 0 on success
    int controlSystem(int argc, char* argv[], ControllerRuntimeInterface& runtime);
    /// Controls a system based on the meta model semantics (command line interface) - returns 0 on success
    int controlSystem(const ControllerArguments& args, ControllerRuntimeInterface& runtime);
}
