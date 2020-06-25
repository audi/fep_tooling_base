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
 *
 * QNX support Copyright 2019 by dSPACE GmbH. All Rights Reserved.
 */
#include <sstream>
#include "a_util/regex.h"
#include "a_util/xml.h"
#include "a_util/strings.h"
#include "a_util/filesystem.h"
#include "meta_model/meta_model_loader.h"
#include "meta_model/meta_model_types.h"
#include "meta_model/meta_model_version.h"
#include "common/platform_fixes.h"          // -> #include <memory>

using a_util::strings::format;

namespace meta_model
{
    static bool isValidIdentifier(const std::string& name)
    {
        static a_util::regex::RegularExpression regex("^([a-zA-Z0-9_\\.\\-]+( [a-zA-Z0-9_\\.\\-]+)*)$");
        return regex.fullMatch(name);
    }

    std::string dumpSystem(const System& system)
    {
        std::ostringstream ss;
        auto indent = [](unsigned level) -> std::string {
            std::string s;
            for (unsigned i = 0; i < level; ++i)
            {
                s.append("  ");
            }
            return s;
        };

        ss << "System Description Dump ##########################\n";

        ss << "Description:\t" << system.description << "\n";
        ss << "Version:\t" << system.version << "\n";
        ss << "Contact:\t" << system.contact << "\n";

        if (system.global_requirement_resolutions.size() > 0)
        {
            ss << "Requirement Resolutions:\n";
            for (const auto& res : system.global_requirement_resolutions)
            {
                ss << indent(1) << res.first << ":" << res.second << "\n";
            }
        }

        ss << "Properties:\n";
        for (const auto& prop : system.properties)
        {
            ss << indent(1) << prop.second.name << ":" << prop.second.type << " = " << prop.second.value << "\n";
        }

        ss << "Elements:\n";
        for (const auto& elem : system.elements)
        {
            ss << indent(1) << elem.second.name << ":" << elem.second.type_name << "\n";

            ss << indent(2) << "Interfaces:\n";
            for (const auto& intf : elem.second.interface_instances)
            {
                ss << indent(3) << intf.second.name << "\n";
                for (const auto& prop : intf.second.properties)
                {
                    ss << indent(4) << prop.second.name << ":" << prop.second.type << " = " << prop.second.value << "\n";
                }
            }

            ss << indent(2) << "Requirement Resolutions:\n";
            for (const auto& res : elem.second.requirement_resolutions)
            {
                ss << indent(3) << res.first << ":" << res.second << "\n";
            }
        }

        return ss.str();
    }

    // used multiple times, therefore declare here
    static bool parseProperty(Property& prop, const a_util::xml::DOMElement& prop_node, std::vector<std::string>& messages);

    static bool parseElementTypeRequirementDefinition(ElementType::RequirementDefinition& requirement,
        const a_util::xml::DOMElement& requirement_node, std::vector<std::string>& messages)
    {
        auto name = requirement_node.getChild("name");
        if (name.isNull())
        {
            messages.push_back("Missing name node on requirement");
            return false;
        }
        requirement.name = name.getData();

        auto version = requirement_node.getChild("version");
        if (version.isNull())
        {
            messages.push_back(format("Missing version node on requirement %s", requirement.name.c_str()));
            return false;
        }
        requirement.version = version.getData();

        return true;
    }

    static bool parseElementTypeInterfaceDefinition(ElementType::InterfaceDefinition& intf,
        const a_util::xml::DOMElement& interface_node,
        const std::string& schema_version,
        std::vector<std::string>& messages)
    {
        auto name = interface_node.getChild("name");
        if (name.isNull())
        {
            messages.push_back("Missing name node on interface");
            return false;
        }
        intf.name = name.getData();

        auto version = interface_node.getChild("version");
        if (version.isNull())
        {
            messages.push_back(format("Missing version node on interface %s", intf.name.c_str()));
            return false;
        }
        intf.version = version.getData();

        if (schema_version == std::string(version_1_0_0))
        {
            messages.push_back(format("Missing type node on interface %s, using default type 'configuration'", intf.name.c_str()));
            intf.type = "configuration";
        }
        else
        {
            auto type = interface_node.getChild("type");
            if (type.isNull())
            {
                messages.push_back(format("Missing type node on interface %s (type is mandetory in version %s)", intf.name.c_str(), schema_version.c_str()));
                return false;
            }
            intf.type = type.getData();
        }
        auto properties_node = interface_node.getChild("properties");
        if (properties_node.isNull())
        {
            messages.push_back(format("Missing properties node on interface %s", intf.name.c_str()));
            return false;
        }

        auto properties = properties_node.getChildren();
        for (const auto& property_node : properties)
        {
            if (property_node.getName() == "property")
            {
                Property prop;
                if (!parseProperty(prop, property_node, messages))
                {
                    return false;
                }

                intf.properties.push_back(std::move(prop));
            }
        }

        return true;
    }

    static bool parseElementType(ElementType& element_type, const a_util::xml::DOM& dom, std::vector<std::string>& messages)
    {
        auto root = dom.getRoot();
        if (root.getName() != "element")
        {
            messages.push_back("File is missing 'element' root node");
            return false;
        }

        auto schema_version = root.getChild("schema_version");
        if (schema_version.isNull() || !isSupportedVersion(schema_version.getData()))
        {
            messages.push_back("Missing schema_version node or invalid version (found \"" + schema_version.getData() + "\")");
            return false;
        }

        auto type_name = root.getChild("type_name");
        if (type_name.isNull())
        {
            messages.push_back("Missing type_name node");
            return false;
        }
        element_type.type_name = type_name.getData();

        auto description = root.getChild("description");
        if (description.isNull())
        {
            messages.push_back("Missing description node");
            return false;
        }
        element_type.description = description.getData();

        auto version = root.getChild("version");
        if (version.isNull())
        {
            messages.push_back("Missing version node");
            return false;
        }
        element_type.type_version = version.getData();

        auto contact = root.getChild("contact");
        if (contact.isNull())
        {
            messages.push_back("Missing contact node");
            return false;
        }
        element_type.contact = contact.getData();

        auto interfaces_node = root.getChild("interfaces");
        if (interfaces_node.isNull())
        {
            messages.push_back("Missing interfaces node");
            return false;
        }

        auto interfaces = interfaces_node.getChildren();
        for (const auto& interface_node : interfaces)
        {
            if (interface_node.getName() == "interface")
            {
                ElementType::InterfaceDefinition definition;
                if (!parseElementTypeInterfaceDefinition(definition, interface_node, schema_version.getData(), messages))
                {
                    return false;
                }

                element_type.interfaces.push_back(std::move(definition));
            }
        }

        auto requirements_node = root.getChild("requirements");
        if (requirements_node.isNull())
        {
            messages.push_back("Missing requirements node");
            return false;
        }

        auto requirements = requirements_node.getChildren();
        for (const auto& requirement_node : requirements)
        {
            if (requirement_node.getName() == "requirement")
            {
                ElementType::RequirementDefinition definition;
                if (!parseElementTypeRequirementDefinition(definition, requirement_node, messages))
                {
                    return false;
                }

                element_type.requirements.push_back(std::move(definition));
            }
        }

        return true;
    }

    std::shared_ptr<const ElementType>
        MetaModelLoader::loadElementType(const std::string& element_desc_file_path, std::vector<std::string>& messages)
    {
        messages.clear();

        a_util::xml::DOM dom;
        if (!dom.load(element_desc_file_path))
        {
            messages.push_back(dom.getLastError());
            return nullptr;
        }

        auto element = std::make_shared<ElementType>();
        if (!parseElementType(*element, dom, messages))
        {
            return nullptr;
        }

        a_util::filesystem::Path path(element_desc_file_path);
        if (path.isRelative())
        {
            path = a_util::filesystem::getWorkingDirectory();
            path.append(element_desc_file_path);
        }

        element->file_path = path.toString();

        _known_element_types.insert(std::make_pair(element->type_name, element));
        return element;
    }

    static bool parseRequirementResolution(System::ElementInstance& instance,
        const a_util::xml::DOMElement& element_node, std::vector<std::string>& messages, bool is_element_rr, 
        std::map<std::string, std::string> system_requirement_resolutions)
    {
        if (element_node.isNull())
        {
            if (is_element_rr)
            {
                messages.push_back(format("Missing requirement_resolutions node on element instance %s", instance.name.c_str()));
            }
            // system rr argument can be null, no error thrown
            return false;
        }

        auto requirement_resolutions = element_node.getChildren();
        for (const auto& requirement_resolution_node : requirement_resolutions)
        {
            if (requirement_resolution_node.getName() == "requirement_resolution")
            {
                auto requirement_node = requirement_resolution_node.getChild("requirement");
                if (requirement_node.isNull())
                {
                    if (is_element_rr)
                    {
                        messages.push_back(format("Missing requirement node on requirement resolution on element instance %s",
                            instance.name.c_str()));
                    }
                    else
                    {
                        messages.push_back("Missing requirement node on requirement resolution of system level");
                    }
                    return false;
                }
                std::string requirement = requirement_node.getData();

                if (!isValidIdentifier(requirement))
                {
                    if (is_element_rr)
                    {
                        messages.push_back(format("Requirement '%s' on element instance %s is not a valid identifier",
                            requirement.c_str(), instance.name.c_str()));
                    }
                    else
                    {
                        messages.push_back(format("Requirement '%s' on system level is not a valid identifier",
                            requirement.c_str()));
                    }
                    return false;
                }

                auto resolution_node = requirement_resolution_node.getChild("resolution");
                if (resolution_node.isNull())
                {
                    if (is_element_rr)
                    {
                        messages.push_back(format("Missing resolution node on requirement resolution on element instance %s",
                            instance.name.c_str()));
                    }
                    else
                    {
                        messages.push_back("Missing resolution node on requirement resolution of system level");
                    }
                    return false;
                }
                std::string resolution = resolution_node.getData();

                if (!isValidIdentifier(resolution))
                {
                    if (is_element_rr)
                    {
                        messages.push_back(format("Requirement resolution '%s' on element instance %s is not a valid identifier",
                            requirement.c_str(), instance.name.c_str()));
                    }
                    else
                    {
                        messages.push_back(format("Requirement resolution '%s' on system level is not a valid identifier",
                            requirement.c_str()));
                    }
                    return false;
                }

                if (instance.requirement_resolutions.find(requirement) != instance.requirement_resolutions.end())
                {
                    if (is_element_rr)
                    {
                        messages.push_back(format("Requirement '%s' on element instance %s resolved more than once",
                            requirement.c_str(), instance.name.c_str()));
                    }
                    else
                    {
                        messages.push_back(format("Requirement '%s' on system level resolved more than once",
                            requirement.c_str()));
                    }
                    return false;
                }
                instance.requirement_resolutions.insert(std::make_pair(std::move(requirement), std::move(resolution)));
            }
        }

        if (is_element_rr)
        {
            // set system requirement resolutions if not already set
            for (const auto& system_resolution : system_requirement_resolutions)
            {
                if (instance.requirement_resolutions.find(system_resolution.first) ==
                    instance.requirement_resolutions.end())
                {
                    instance.requirement_resolutions.insert(std::move(system_resolution));
                }
            }
        }
        return true;
    }

    static bool parseSystemElementInstance(System::ElementInstance& instance,
        const a_util::xml::DOMElement& element_node, std::vector<std::string>& messages,
        std::map<std::string, std::string> system_requirement_resolutions)
    {
        auto name = element_node.getChild("name");
        if (name.isNull())
        {
            messages.push_back("Missing name node on element instance");
            return false;
        }
        instance.name = name.getData();

        if (!isValidIdentifier(instance.name))
        {
            messages.push_back(format("Element instance '%s' has invalid name", instance.name.c_str()));
            return false;
        }

        auto type = element_node.getChild("type");
        if (type.isNull())
        {
            messages.push_back(format("Missing type node on element instance %s", instance.name.c_str()));
            return false;
        }
        instance.type_name = type.getData();

        auto type_version = element_node.getChild("type_version");
        if (type_version.isNull())
        {
            messages.push_back(format("Missing type_version node on element instance %s", instance.name.c_str()));
            return false;
        }
        instance.type_version = type_version.getData();

        auto interface_properties_node = element_node.getChild("interface_properties");
        if (interface_properties_node.isNull())
        {
            messages.push_back(format("Missing interface_properties_node node on element instance %s",
                instance.name.c_str()));
            return false;
        }

        auto interface_properties = interface_properties_node.getChildren();
        for (const auto& interface_property_node : interface_properties)
        {
            if (interface_property_node.getName() == "interface")
            {
                System::ElementInstance::InterfaceInstance intf_instance;

                auto intf_name_node = interface_property_node.getChild("name");
                if (intf_name_node.isNull())
                {
                    messages.push_back(format("Missing name node on interface properties of element instance %s",
                        instance.name.c_str()));
                    return false;
                }
                intf_instance.name = intf_name_node.getData();

                if (!isValidIdentifier(intf_instance.name))
                {
                    messages.push_back(format("Interface '%s' in element instance %s has invalid name",
                        intf_instance.name.c_str(), instance.name.c_str()));
                    return false;
                }

                if (instance.interface_instances.find(intf_instance.name) != instance.interface_instances.end())
                {
                    messages.push_back(format("Interface '%s' in element instance %s is defined more than once",
                        intf_instance.name.c_str(), instance.name.c_str()));
                    return false;
                }

                auto actual_properties_node = interface_property_node.getChild("properties");
                if (actual_properties_node.isNull())
                {
                    messages.push_back(format("Missing properties node on interface properties of element instance %s",
                        instance.name.c_str()));
                    return false;
                }

                auto actual_properties = actual_properties_node.getChildren();
                for (const auto& property_node : actual_properties)
                {
                    if (property_node.getName() == "property")
                    {
                        Property prop;
                        if (!parseProperty(prop, property_node, messages))
                        {
                            return false;
                        }

                        auto prop_name = prop.name;
                        if (intf_instance.properties.find(prop_name) != intf_instance.properties.end())
                        {
                            messages.push_back(format("Property '%s' on interface %s of element instance %s defined more than once",
                                prop_name.c_str(), intf_instance.name.c_str(), instance.name.c_str()));
                            return false;
                        }

                        intf_instance.properties.insert(std::make_pair(std::move(prop_name), std::move(prop)));
                    }
                }

                auto pair_name = intf_instance.name;
                instance.interface_instances.insert(std::make_pair(std::move(pair_name), std::move(intf_instance)));
            }
        }

        auto requirement_resolutions_node = element_node.getChild("requirement_resolutions");
        return parseRequirementResolution(instance, requirement_resolutions_node, messages, true, system_requirement_resolutions);
    }

    static bool parseSystemFromDOM(System& system, const a_util::xml::DOM& dom, std::vector<std::string>& messages)
    {
        auto root = dom.getRoot();
        if (root.getName() != "system")
        {
            messages.push_back("File is missing 'system' root node");
            return false;
        }

        auto schema_version = root.getChild("schema_version");
        if (schema_version.isNull() || !isSupportedVersion(schema_version.getData()))
        {
            messages.push_back("Missing schema_version node or invalid version (found \"" + schema_version.getData() + "\")");
            return false;
        }

        auto description = root.getChild("description");
        if (description.isNull())
        {
            messages.push_back("Missing description node");
            return false;
        }
        system.description = description.getData();

        auto version = root.getChild("version");
        if (version.isNull())
        {
            messages.push_back("Missing version node");
            return false;
        }
        system.version = version.getData();

        auto contact = root.getChild("contact");
        if (contact.isNull())
        {
            messages.push_back("Missing contact node");
            return false;
        }
        system.contact = contact.getData();

        auto properties_node = root.getChild("properties");
        if (properties_node.isNull())
        {
            messages.push_back("Missing properties node");
            return false;
        }

        auto properties = properties_node.getChildren();
        for (const auto& property_node : properties)
        {
            if (property_node.getName() == "property")
            {
                Property prop;
                if (!parseProperty(prop, property_node, messages))
                {
                    return false;
                }

                std::string name = prop.name;
                system.properties.insert(std::make_pair(std::move(name), std::move(prop)));
            }
        }

        auto requirement_resolutions = root.getChild("requirement_resolutions");
        System::ElementInstance dummy_instance;
        if (parseRequirementResolution(dummy_instance, requirement_resolutions, messages, false, 
            system.global_requirement_resolutions))
        {
            // set system requirement resolutions 
            system.global_requirement_resolutions = dummy_instance.requirement_resolutions;
        }


        auto elements_node = root.getChild("elements");
        if (elements_node.isNull())
        {
            messages.push_back("Missing elements node");
            return false;
        }

        auto elements = elements_node.getChildren();
        for (const auto& element_node : elements)
        {
            if (element_node.getName() == "element")
            {
                std::pair<std::string, System::ElementInstance> pair;
                if (!parseSystemElementInstance(pair.second, element_node, messages,
                    system.global_requirement_resolutions))
                {
                    return false;
                }

                pair.first = pair.second.name;

                if (system.elements.find(pair.first) != system.elements.end())
                {
                    messages.push_back(format("Element instance '%s' defined more than once", pair.first.c_str()));
                    return false;
                }

                system.elements.insert(std::move(pair));
            }
        }

        return true;
    }

    std::unique_ptr<System> parseSystem(const std::string& system_desc_file_path, std::vector<std::string>& messages)
    {
        messages.clear();

        a_util::xml::DOM dom;
        if (!dom.load(system_desc_file_path))
        {
            messages.push_back(dom.getLastError());
            return nullptr;
        }

#if __cplusplus >= 201402L
        auto system = std::make_unique<System>();
#else
        auto system = std::unique_ptr<System>(new System());
#endif
        if (!parseSystemFromDOM(*system, dom, messages))
        {
            return nullptr;
        }
        return system;
    }

    std::unique_ptr<System>
        MetaModelLoader::loadSystem(const std::string& system_desc_file_path, std::vector<std::string>& messages) const
    {
        messages.clear();

        a_util::xml::DOM dom;
        if (!dom.load(system_desc_file_path))
        {
            messages.push_back(dom.getLastError());
            return nullptr;
        }

#if __cplusplus >= 201402L
        auto system = std::make_unique<System>();
#else
        auto system = std::unique_ptr<System>(new System());
#endif
        if (!parseSystemFromDOM(*system, dom, messages))
        {
            return nullptr;
        }

        a_util::filesystem::Path path(system_desc_file_path);
        if (path.isRelative())
        {
            path = a_util::filesystem::getWorkingDirectory();
            path.append(system_desc_file_path);
        }

        system->file_path = path.toString();

        // lookup/resolve element types from _known_element_types, fail on unknown type
        for (auto& instance : system->elements)
        {
            auto type = _known_element_types.find(instance.second.type_name);
            if (type == _known_element_types.end())
            {
                messages.push_back(format("Unknown element type %s on element instance %s",
                    instance.second.type_name.c_str(), instance.second.name.c_str()));
                return nullptr;
            }

            const auto& type_inst = type->second;
            if (type_inst->type_version != instance.second.type_version)
            {
                messages.push_back(format("Invalid type version for %s on element instance %s",
                    instance.second.type_name.c_str(), instance.second.name.c_str()));
                return nullptr;
            }

            instance.second.type = type_inst;
        }

        // now copy all interfaces/properties from the type to the instance
        // careful - we mustn't overwrite properties set by the system description!
        for (auto& instance : system->elements)
        {
            for (const auto& type_intf : instance.second.type->interfaces)
            {
                auto inst_intf = instance.second.interface_instances.find(type_intf.name);
                if (inst_intf == instance.second.interface_instances.end())
                {
                    // copy it entirely from the type
                    System::ElementInstance::InterfaceInstance inst;
                    inst.name = type_intf.name;
                    for (const auto& prop : type_intf.properties)
                    {
                        inst.properties.insert(std::make_pair(prop.name, prop));
                    }

                    inst_intf = instance.second.interface_instances.insert(std::make_pair(type_intf.name, std::move(inst))).first;
                }
                
                for (const auto& type_prop : type_intf.properties)
                {
                    // only copy if not already present from the system description
                    if (inst_intf->second.properties.find(type_prop.name) == inst_intf->second.properties.end())
                    {
                        inst_intf->second.properties.insert(std::make_pair(type_prop.name, type_prop));
                    }
                }
            }
        }
        return system;
    }

    static bool parseProperty(Property& prop, const a_util::xml::DOMElement& prop_node, std::vector<std::string>& messages)
    {
        auto name = prop_node.getChild("name");
        if (name.isNull())
        {
            messages.push_back("Missing name node in property");
            return false;
        }
        prop.name = name.getData();

        auto value = prop_node.getChild("value");
        if (value.isNull())
        {
            messages.push_back(format("Missing value node in property %s", prop.name.c_str()));
            return false;
        }
        prop.value = value.getData();

        auto type = prop_node.getChild("type");
        if (type.isNull())
        {
            messages.push_back(format("Missing type node in property %s", prop.name.c_str()));
            return false;
        }
        prop.type = type.getData();

        return true;
    }

    static bool parseProperties(std::vector<Property>& properties, const a_util::xml::DOM& dom, std::vector<std::string>& messages)
    {
        auto root = dom.getRoot();
        if (root.getName() != "property_file")
        {
            messages.push_back("File is missing 'property_file' root node");
            return false;
        }

        auto version = root.getChild("schema_version");
        if (version.isNull() || !isSupportedVersion(version.getData()))
        {
            messages.push_back("Missing schema_version node or invalid version (found \"" + version.getData() + "\")");
            return false;
        }

        auto props = root.getChild("properties");
        if (props.isNull())
        {
            messages.push_back("Missing properties node");
            return false;
        }

        auto children = props.getChildren();
        for (const auto& prop : children)
        {
            if (prop.getName() == "property")
            {
                meta_model::Property dest;
                if (!parseProperty(dest, prop, messages))
                {
                    return false;
                }

                properties.push_back(std::move(dest));
            }
        }

        return true;
    }

    std::unique_ptr<const std::vector<Property>>
        loadProperties(const std::string& property_desc_file_path, std::vector<std::string>& messages)
    {
        messages.clear();

        a_util::xml::DOM dom;
        if (!dom.load(property_desc_file_path))
        {
            messages.push_back(dom.getLastError());
            return nullptr;
        }

#if __cplusplus >= 201402L
        auto properties = std::make_unique<std::vector<Property>>();
#else
        auto properties = std::unique_ptr<std::vector<Property>>(new std::vector<Property>());
#endif
        if (!parseProperties(*properties, dom, messages))
        {
            return nullptr;
        }

#if (__GNUC__ == 4 && __GNUC_MINOR__ < 9)
        return std::move(properties); // GCC-4.8 sucks...
#else
        return properties;
#endif
    }
}
