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

#include "launcher_base/launcher.h" // todo: refactor runtimeinterface out
#include "launcher_base/launch_configuration.h"
#include "meta_model/meta_model_types.h"

namespace launcher_base
{
    bool launchLocally(const launcher_base::LaunchConfig::Participant& config,
        const LauncherRuntimeInterface& runtime, bool accept_launched_participants);

    bool launchWithRemoteStarter(const launcher_base::LaunchConfig::Participant& config,
        const LauncherRuntimeInterface& runtime, bool accept_launched_participants,
        RemoteStartInterface* remote_access);
}
