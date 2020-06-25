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
#include "a_util/strings.h"
#include "meta_model/meta_model_configuration.h"
#include "meta_model/meta_model_checker.h"

using a_util::strings::format;

namespace meta_model
{
    static bool applyProperty(System::ElementInstance& element, const Property& prop,
        std::vector<std::string>& messages)
    {
        auto parts = a_util::strings::split(prop.name, "/", false);

        auto interface_it = element.interface_instances.find(parts[0]);
        if (parts.size() > 1 && interface_it != element.interface_instances.end())
        {
            auto copy = prop;
            parts.erase(parts.begin());
            copy.name = a_util::strings::join(parts, "/");

            interface_it->second.properties[copy.name] = copy;
        }
        else
        {
            messages.push_back(format("Property '%s' on element instance %s is not addressing an interface",
                prop.name.c_str(), element.name.c_str()));
            return false;
        }

        return true;
    }

    static bool applyProperty(System& system, const Property& prop,
        std::vector<std::string>& messages)
    {
        auto parts = a_util::strings::split(prop.name, "/", false);

        auto element_it = system.elements.find(parts[0]);
        if (parts.size() > 1 && element_it != system.elements.end())
        {
            auto copy = prop;
            parts.erase(parts.begin());
            copy.name = a_util::strings::join(parts, "/");

            return applyProperty(element_it->second, copy, messages);
        }
        else
        {
            system.properties[prop.name] = prop;
        }

        return true;
    }

    bool configureSystem(System& system, const std::vector<Property>& properties,
        std::vector<std::string>& messages)
    {
        for (const auto& prop : properties)
        {
            if (!applyProperty(system, prop, messages))
            {
                return false;
            }
        }

        return true;
    }

    void resolveReferencedProperties(System& system)
    {
        for (auto& instance : system.elements)
        {
            for (auto& intf_instance : instance.second.interface_instances)
            {
                for (auto& intf_prop : intf_instance.second.properties)
                {
                    std::string ref_prop;
                    if (isReferencingProperty(intf_prop.second.value, ref_prop))
                    {
                        intf_prop.second.value = system.properties[ref_prop].value;
                    }
                }
            }
        }
    }
}
