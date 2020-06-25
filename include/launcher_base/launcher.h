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

#include <cstdint>
#include <vector>
#include <string>
#include "meta_model/meta_model_types.h"

namespace launcher_base
{
    /**
    * This interface connects the abstract launch functionality (see launchSystem below) with a concrete
    * runtime implementation. Any middleware/system implementing this interface can be launched.
    */
    class LauncherRuntimeInterface
    {
    public:
        virtual ~LauncherRuntimeInterface() {}

        /// Sets the system context for all further calls to the interface - returns false if system unsupported
        virtual bool setContext(const meta_model::System& system, const std::string& system_name) = 0;

        /// The participant states relevant for the launcher
        enum class ParticipantState
        {
            NotStarted,
            Idle,
            Other
        };
        
        /// Returns the state of a participant
        virtual ParticipantState getParticipantState(const std::string& participant) const = 0;

        /// Awaits state Idle for all participants or until timeout is hit
        virtual bool awaitParticipantsIdle(const std::vector<std::string>& participants,
            std::vector<std::string>& failed_participants) const = 0;

        /// Applies properties of an element interface instance to the runtime
        virtual bool configureInterface(const meta_model::System::ElementInstance::InterfaceInstance& intf,
            const std::string& participant, const std::string& system_desc_base_path) = 0;

        /// Applies a requirement resolution to the runtime
        virtual bool applyRequirementResolution(const meta_model::System::ElementInstance& resolution_instance,
            const std::string& requirement, const std::string& participant) = 0;

        /// Shutting down participants if error occurs
        virtual bool cleanUp(const std::vector<std::string>& participants) = 0;
    };

    /**
    * This interface connects the abstract launch functionality (see launchSystem below) with a concrete
    * remote start implementation. Any middleware/system implementing this interface can launch by remote
    * on another host.
    */
    class RemoteStartInterface
    {
    public:
        virtual ~RemoteStartInterface() {};

        /// Sets all properties to start RemoteStarter on host and afterwards triggers RemoteStarter into state
        /// Running - returns false if starting failed
        virtual bool startRemote(const std::string& host, const std::string& executable,
            const std::string& executable_version = "", const std::string& working_dir = "", 
            const std::string& arguments = "") = 0;
    };    

    /// argument struct that holds all arguments that can be parsed by controller
    struct LaucherArguments
    {
        std::string system_name;
        std::string system_description;
        std::vector<std::string> element_descriptions;
        std::vector<std::string> property_descriptions;
        std::vector<std::string> environment_variables;
        std::string launch_configuration;
        uint64_t waiting_timeout = 0;
        bool accept_launched_participants = false;
        bool verbose = false;
    };
    /// Launches a system based on the meta model semantics (command line interface) - returns 0 on success
    int launchSystem(int argc, char* argv[], LauncherRuntimeInterface& runtime, 
        RemoteStartInterface* remote_access = nullptr);
    /// Launches a system based on the meta model semantics (command line interface) - returns 0 on success
    int launchSystem(const LaucherArguments& args, LauncherRuntimeInterface& runtime,
        RemoteStartInterface* remote_access = nullptr);
    /// Function that parses the arguments of launcher and stores them in args variable - returns 0 on success
    int parseArguments(int argc, char* argv[], LaucherArguments& args, bool& should_exit);
}
