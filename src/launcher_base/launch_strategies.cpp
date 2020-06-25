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
#include <iostream>
#include <clara.hpp>
#include "a_util/xml.h"
#include "a_util/strings.h"
#include "a_util/filesystem.h"
#include "a_util/process.h"
#include "a_util/system.h"

#include "launch_strategies.h"

using a_util::strings::format;

namespace launcher_base
{
    bool launchLocally(const launcher_base::LaunchConfig::Participant& config,
        const LauncherRuntimeInterface& runtime, bool accept_launched_participants)
    {
        std::cout << "Launching " << config.name << "... ";

        auto state = runtime.getParticipantState(config.name);
        if (state != LauncherRuntimeInterface::ParticipantState::NotStarted)
        {
            if (state == LauncherRuntimeInterface::ParticipantState::Idle && accept_launched_participants)
            {
                std::cout << "OK (Already started)\n";
                return true;
            }
            else
            {
                std::cout << "FAILED (already started or invalid state)\n";
                return false;
            }
        }
        else
        {
            a_util::filesystem::Path command = config.executable;

            auto code = a_util::process::execute(command, config.arguments, config.working_dir, false);
            if (code != 0)
            {
                std::cout << "FAILED with " << code << ": " << 
                    a_util::system::formatSystemError(a_util::system::getLastSystemError()) << "\n";
                return false;
            }

            std::cout << "OK\n";
            return true;
        }
    }

    bool launchWithRemoteStarter(const launcher_base::LaunchConfig::Participant& config,
        const LauncherRuntimeInterface& runtime, bool accept_launched_participants,
        RemoteStartInterface* remote_access)
    {
        std::cout << "Launching remote " << config.name << "... ";

        auto state = runtime.getParticipantState(config.name);
        if (state != LauncherRuntimeInterface::ParticipantState::NotStarted)
        {
            if (state == LauncherRuntimeInterface::ParticipantState::Idle && accept_launched_participants)
            {
                std::cout << "OK (Already started)\n";
                return true;
            }
            else
            {
                std::cout << "FAILED (already started or invalid state)\n";
                return false;
            }
        }
        else
        {
            std::string alias = config.remote_starter_alias;
            if (alias.empty() || !config.executable.empty())
            {
                std::cout << "FAILED (given config doesn't contain alias or a executable is set)\n";
                return false;
            }
            std::vector<std::string> splitting_helper = a_util::strings::split(alias, "@");
            std::string alias_version = "";
            if (splitting_helper.size() == 2)
            {
                alias = splitting_helper.front();
                alias_version = splitting_helper.back();
            }
            else if (splitting_helper.size() > 2)
            {
                std::cout << "FAILED (given alias doesn't support the correct syntax)\n";
                return false;
            }
            if (config.host.empty())
            {
                std::cout << "FAILED (empty host in config is not supported)\n";
                return false;
            }
            if (!remote_access->startRemote(config.host, alias, alias_version,
                config.working_dir, config.arguments))
            {
                std::cout << "FAILED (there was an error while starting the remote participant)\n";
                return false;
            }
            std::cout << "OK\n";
        }
        return true;
    }
}
