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

namespace meta_model
{
    constexpr const char* version_1_0_0   = "1.0.0";
    constexpr const char* version_1_1_0   = "1.1.0";
    constexpr const char* version_current = version_1_1_0;

    inline bool isSupportedVersion(const std::string& version)
    {
        std::list<std::string> supported_versions = { version_1_0_0, version_1_1_0 };
        for (const auto& it_version : supported_versions)
        {
            if (it_version == version)
            {
                return true;
            }
        }
        return false;
    }
}
