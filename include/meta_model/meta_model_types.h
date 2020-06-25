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
    /// Represents a single property
    struct Property
    {
        std::string name;
        std::string value;
        std::string type;
    };

    /// Represents the type of an element, described by a single element description file
    struct ElementType
    {
        struct InterfaceDefinition
        {
            std::string name;
            std::string version;
            std::string type;
            std::vector<Property> properties;
        };

        struct RequirementDefinition
        {
            std::string name;
            std::string version;
        };
        std::string file_path;
        std::string type_name;
        std::string description;
        std::string type_version;
        std::string contact;
        std::vector<InterfaceDefinition> interfaces;
        std::vector<RequirementDefinition> requirements;
    };

    /// Represents a concrete system, described by a single system description file
    struct System
    {
        struct ElementInstance
        {
            struct InterfaceInstance
            {
                std::string name;
                std::string type;
                std::map<std::string, Property> properties;
            };

            std::shared_ptr<const ElementType> type;
            std::string name;
            std::string type_name;
            std::string type_version;
            std::map<std::string, InterfaceInstance> interface_instances;
            std::map<std::string, std::string> requirement_resolutions;
        };

        std::string file_path;
        std::string description;
        std::string version;
        std::string contact;
        std::map<std::string, Property> properties;
        std::map<std::string, ElementInstance> elements;
        std::map<std::string, std::string> global_requirement_resolutions;
    };
}
