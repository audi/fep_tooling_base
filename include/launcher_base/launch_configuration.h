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
#include <map>

namespace launcher_base
{
    /// Represents the launch configuration defined by the FEP2 meta model implementation
    struct LaunchConfig
    {
        struct Participant
        {
            std::string name;
            std::string host;
            std::string working_dir;
            std::string arguments;
            std::string executable;
            std::string remote_starter_alias;
        };

        std::string file_path;
        std::string description;
        std::string contact;
        std::map<std::string, Participant> participants;
    };
}
