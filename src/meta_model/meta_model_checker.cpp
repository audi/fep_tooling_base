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
#include <set>
#include <algorithm>
#include "a_util/regex.h"
#include "a_util/strings.h"
#include "meta_model/meta_model_checker.h"
#include "meta_model/meta_model_types.h"

using a_util::strings::format;

namespace meta_model
{
    static bool isValidIdentifier(const std::string& name)
    {
        static a_util::regex::RegularExpression regex("[A-Za-z0-9_\\./]+");
        return regex.fullMatch(name);
    }

    static const ElementType::InterfaceDefinition*
        findInterfaceDefinition(const System& system, const std::string& element, const std::string& intf)
    {
        auto iter = system.elements.find(element);
        if (iter == system.elements.end())
        {
            return nullptr;
        }

        const auto& element_type = iter->second.type;
        for (const auto& intf_def : element_type->interfaces)
        {
            if (intf_def.name == intf)
            {
                return &intf_def;
            }
        }

        return nullptr;
    }

    bool isReferencingProperty(const std::string& value, std::string& referenced_property)
    {
        a_util::regex::RegularExpression regex("\\$\\((.*)\\)");
        return regex.fullMatch(value, referenced_property);
    }

    bool checkSystem(const System& system, const std::string& system_name, std::vector<std::string>& messages)
    {
        auto ok = true;

        // is the system name valid
        if (!isValidIdentifier(system_name))
        {
            messages.push_back(format("System name '%s' is invalid", system_name.c_str()));
            ok = false;
        }

        // does every configured element interface exist?
        for (const auto& instance : system.elements)
        {
            for (const auto& intf : instance.second.interface_instances)
            {
                const auto& name = intf.first;
                auto intf_definition = findInterfaceDefinition(system, instance.first, name);
                if (!intf_definition)
                {
                    messages.push_back(format("Interface '%s' in element %s is not defined by the type",
                        name.c_str(), instance.first.c_str()));
                    ok = false;
                }
            }
        }

        // is every resolution referring to a matching requirement
        std::map<std::string, std::set<std::string>> resolved_requirements; // reused below
        for (const auto& instance : system.elements)
        {
            auto& element_resolved_requirements = resolved_requirements[instance.first];

            for (const auto& rr : instance.second.requirement_resolutions)
            {
                const auto& req = rr.first;
                const auto& res = rr.second;

                if (element_resolved_requirements.find(req) != element_resolved_requirements.end())
                {
                    messages.push_back(format("Interface requirement '%s' in element %s is already resolved",
                        req.c_str(), instance.first.c_str()));
                    ok = false;
                    break;
                }

                auto req_definition = std::find_if(instance.second.type->requirements.begin(),
                    instance.second.type->requirements.end(), [&](const ElementType::RequirementDefinition& def)
                {
                    return def.name == req;
                });

                if (req_definition == instance.second.type->requirements.end())
                {
                    messages.push_back(format("Requirement resolution refers to non-existing requirement '%s' in element %s",
                        req.c_str(), instance.first.c_str()));
                    ok = false;
                    break;
                }

                auto intf_definition_at_resolution = findInterfaceDefinition(system, res, req);
                if (!intf_definition_at_resolution)
                {
                    messages.push_back(format("Requirement resolution refers to non-existing interface '%s' in element %s",
                        res.c_str(), instance.first.c_str()));
                    ok = false;
                    break;
                }

                if (req_definition->version != intf_definition_at_resolution->version)
                {
                    messages.push_back(format("Requirement resolution does not match required version '%s' in element %s",
                        res.c_str(), instance.first.c_str()));
                    ok = false;
                    break;
                }
                
                element_resolved_requirements.insert(req);
            }
        }

        // is every requirement resolved?
        for (const auto& instance : system.elements)
        {
            const auto& element_resolved_requirements = resolved_requirements[instance.first];

            for (const auto& req : instance.second.type->requirements)
            {
                if (element_resolved_requirements.find(req.name) == element_resolved_requirements.end())
                {
                    messages.push_back(format("Interface requirement '%s' in element %s is unresolved",
                        req.name.c_str(), instance.first.c_str()));
                    ok = false;
                }
            }
        }

        // are any system property references correct?
        for (const auto& instance : system.elements)
        {
            for (const auto& intf : instance.second.interface_instances)
            {
                const auto& props = intf.second.properties;

                for (const auto& prop_it : props)
                {
                    const auto& prop = prop_it.second;

                    std::string referenced_property;
                    if (isReferencingProperty(prop.value, referenced_property))
                    {
                        if (system.properties.find(referenced_property) == system.properties.end())
                        {
                            messages.push_back(
                                format("Interface property '%s' referencing invalid system property '%s' in interface %s of element %s",
                                    prop.name.c_str(), referenced_property.c_str(), intf.first.c_str(), instance.first.c_str()));
                            ok = false;
                        }
                    }
                }
            }
        }

        return ok;
    }
}
