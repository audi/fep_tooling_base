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
#include <vector>
#include <map>
#include <memory>

namespace meta_model
{
    struct Property;
    struct ElementType;
    struct System;

    /// generate an element description from given input
    bool writeElementDescription(const ElementType& element, std::vector<std::string>& messages);

    /// generate a system description from given input
    bool writeSystemDescription(const System& system, std::vector<std::string>& messages);

    /// generate an system description from given input
    bool writePropertyDescription(const std::vector<Property>& properties, 
        const std::string& file_path, std::vector<std::string>& messages);
}
