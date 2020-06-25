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
#include <sstream>
#include "a_util/regex.h"
#include "a_util/xml.h"
#include "a_util/strings.h"
#include "a_util/filesystem.h"
#include "meta_model/meta_model_writer.h"
#include "meta_model/meta_model_types.h"
#include "meta_model/meta_model_version.h"
#include "common/platform_fixes.h"

using a_util::strings::format;
using a_util::xml::DOM;
using a_util::xml::DOMElement;

namespace meta_model
{
    static bool createNode(DOMElement parent, const std::string& value, const std::string& name,
        std::vector<std::string>& messages)
    {
        if (value.empty())
        {
            messages.push_back(format("%s can't be empty, please fill the description properly.\n",
                name.c_str()));
            return false;
        }
        auto node = parent.createChild(name);
        node.setData(value);
        return true;
    }

    static bool createProperty(DOMElement& properties_node, const Property& property,
        std::vector<std::string>& messages)
    {
        bool is_valid = true;
        auto prop_node = properties_node.createChild("property");
        is_valid &= createNode(prop_node, property.name, "name", messages);
        is_valid &= createNode(prop_node, property.type, "type", messages);
        is_valid &= createNode(prop_node, property.value, "value", messages);
        return is_valid;
    }

    static bool save_file(const DOM& dom, const std::string& file_path, std::vector<std::string>& messages)
    {
        bool save_result = dom.save(file_path);
        if (!save_result)
        {
            messages.push_back(dom.getLastError());
            messages.push_back("Make sure directory exists, before creating the file.");
            return false;
        }
        return true;
    }

    bool writeElementDescription(const ElementType& element, 
        std::vector<std::string>& messages)
    {
        DOM dom;
        DOMElement root = dom.getRoot();

        // fill the header of the element description
        root.setName("element");

        auto schema_version_node = root.createChild("schema_version");
        // right now, the last supported version will be written!
        schema_version_node.setData(version_current);

        bool is_valid = createNode(root, element.type_name, "type_name", messages);
        is_valid &= createNode(root, element.description, "description", messages);
        is_valid &= createNode(root, element.type_version, "version", messages);
        is_valid &= createNode(root, element.contact, "contact", messages);

        // fill and verify the interfaces
        auto interfaces_node = root.createChild("interfaces");
        for (const auto& interface : element.interfaces)
        {
            auto intf_node = interfaces_node.createChild("interface");
            is_valid &= createNode(intf_node, interface.name, "name", messages);
            is_valid &= createNode(intf_node, interface.version, "version", messages);
            is_valid &= createNode(intf_node, interface.type, "type", messages);
            auto properties_node = intf_node.createChild("properties");
            for (const auto& property : interface.properties)
            {
                is_valid &= createProperty(properties_node, property, messages);
            }
        }

        // fill and verify the requirements
        auto requirements = root.createChild("requirements");
        for (const auto& requirement : element.requirements)
        {
            auto req_node = requirements.createChild("requirement");
            is_valid &= createNode(req_node, requirement.name, "name", messages);
            is_valid &= createNode(req_node, requirement.version, "version", messages);
        }

        if (is_valid)
        {
            // Save file
            is_valid = save_file(dom, element.file_path, messages);
        }

        return is_valid;
    }

    bool writeSystemDescription(const System& system, std::vector<std::string>& messages)
    {
        DOM dom;
        DOMElement root = dom.getRoot();

        // fill the header of the element description
        root.setName("system");

        auto schema_version_node = root.createChild("schema_version");
        // right now, the only supported version
        schema_version_node.setData(version_current);

        bool is_valid = createNode(root, system.description, "description", messages);
        is_valid &= createNode(root, system.version, "version", messages);
        is_valid &= createNode(root, system.contact, "contact", messages);

        if (system.global_requirement_resolutions.size() > 0)
        {
            auto sys_rr = root.createChild("requirement_resolutions");
            for (const auto& map_req : system.global_requirement_resolutions)
            {
                auto req_node = sys_rr.createChild("requirement_resolution");
                is_valid &= createNode(req_node, map_req.first, "requirement", messages);
                is_valid &= createNode(req_node, map_req.second, "resolution", messages);
            }
        }

        // fill and verify the properties
        auto properties_node = root.createChild("properties");
        for (const auto& map_property : system.properties)
        {
            is_valid &= createProperty(properties_node, map_property.second, messages);
        }

        // fill and verify the elements
        auto elements_node = root.createChild("elements");
        for (const auto& map_element : system.elements)
        {
            auto element = map_element.second;
            auto element_node = elements_node.createChild("element");
            is_valid &= createNode(element_node, element.name, "name", messages);
            is_valid &= createNode(element_node, element.type_name, "type", messages);
            is_valid &= createNode(element_node, element.type_version, "type_version", messages);
            // Interfaces
            auto intf_properties_node = element_node.createChild("interface_properties");
            for (const auto& map_intf : element.interface_instances)
            {
                auto intf = map_intf.second;
                auto intf_node = intf_properties_node.createChild("interface");
                is_valid &= createNode(intf_node, intf.name, "name", messages);
                // properties
                auto properties_node_intf = intf_node.createChild("properties");
                for (const auto& map_prop : intf.properties)
                {
                    is_valid &= createProperty(properties_node_intf, map_prop.second, messages);
                }
            }
            // Requirement resolutions
            auto requirement_resolutions_node = element_node.createChild("requirement_resolutions");
            for (const auto& map_req : element.requirement_resolutions)
            {
                auto req_node = requirement_resolutions_node.createChild("requirement_resolution");
                is_valid &= createNode(req_node, map_req.first, "requirement", messages);
                is_valid &= createNode(req_node, map_req.second, "resolution", messages);
            }
        }
        
        if (is_valid)
        {
            // Save file
            is_valid = save_file(dom, system.file_path, messages);
        }

        return is_valid;
    }


    bool writePropertyDescription(const std::vector<Property>& properties,
        const std::string& file_path, std::vector<std::string>& messages)
    {
        DOM dom;
        DOMElement root = dom.getRoot();

        // fill the header of the element description
        root.setName("property_file");

        auto schema_version_node = root.createChild("schema_version");
        // right now, the only supported version
        schema_version_node.setData(version_current);

        // properties
        bool is_valid = true;
        auto properties_node = root.createChild("properties");
        for (const auto& property : properties)
        {
            is_valid &= createProperty(properties_node, property, messages);
        }

        if (is_valid)
        {
            // Save file
            is_valid = save_file(dom, file_path, messages);
        }

        return is_valid;
    }
}
