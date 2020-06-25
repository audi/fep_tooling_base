/**

   @copyright
   @verbatim
   Copyright @ 2019 Audi AG. All rights reserved.
   
       This Source Code Form is subject to the terms of the Mozilla
       Public License, v. 2.0. If a copy of the MPL was not distributed
       with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
   
   If it is not possible or desirable to put the notice in a particular file, then
   You may include the notice in a location (such as a LICENSE file in a
   relevant directory) where a recipient would be likely to look for such a notice.
   
   You may add additional accurate notices of copyright ownership.
   @endverbatim
 */
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 6326 28182)
#endif

#include <gtest/gtest.h>
#include <a_util/strings.h>
#include "meta_model/meta_model_loader.h"
#include "meta_model/meta_model_writer.h"
#include "meta_model/meta_model_types.h"

TEST(tester_meta_model, Loader_Property_OKs)
{
    std::vector<std::string> msgs;
    auto properties = meta_model::loadProperties("test_files/ok1.fep_properties", msgs);
    ASSERT_TRUE(properties != nullptr);
    ASSERT_TRUE(msgs.empty());

    ASSERT_TRUE(properties->size() == 3);

    ASSERT_TRUE(properties->at(0).name == "name1");
    ASSERT_TRUE(properties->at(0).value == "value1");
    ASSERT_TRUE(properties->at(0).type == "type1");

    ASSERT_TRUE(properties->at(1).name == "name2");
    ASSERT_TRUE(properties->at(1).value == "value2");
    ASSERT_TRUE(properties->at(1).type == "type2");

    ASSERT_TRUE(properties->at(2).name == "name3");
    ASSERT_TRUE(properties->at(2).value == "value3");
    ASSERT_TRUE(properties->at(2).type == "type3");
}

TEST(tester_meta_model, Loader_Property_NOKs)
{
    for (int nok = 1; nok <= 7; ++nok)
    {
        std::vector<std::string> msgs;
        auto props = meta_model::loadProperties(a_util::strings::format("test_files/nok%d.fep_properties", nok), msgs);
        ASSERT_TRUE(props == nullptr);
        ASSERT_TRUE(!msgs.empty());
    }
}

TEST(tester_meta_model, Loader_Element_1_0_0_OKs)
{
    meta_model::MetaModelLoader loader;

    std::vector<std::string> msgs;
    auto element = loader.loadElementType("test_files/ok1_1_0_0.fep_element", msgs);
    ASSERT_TRUE(element != nullptr);
    ASSERT_TRUE(msgs.size() == 2); //warnings because of configuration

    ASSERT_TRUE(element->type_name == "type_name1");
    ASSERT_TRUE(element->description == "description1");
    ASSERT_TRUE(element->type_version == "version1");
    ASSERT_TRUE(element->contact == "contact1");

    ASSERT_TRUE(element->interfaces.size() == 2);
    ASSERT_TRUE(element->interfaces[0].name == "name1");
    ASSERT_TRUE(element->interfaces[0].version == "version1");
    ASSERT_TRUE(element->interfaces[0].type == "configuration");
    ASSERT_TRUE(element->interfaces[0].properties.size() == 2);
    ASSERT_TRUE(element->interfaces[0].properties[0].name == "name1");
    ASSERT_TRUE(element->interfaces[0].properties[0].value == "value1");
    ASSERT_TRUE(element->interfaces[0].properties[0].type == "type1");
    ASSERT_TRUE(element->interfaces[0].properties[1].name == "name2");
    ASSERT_TRUE(element->interfaces[0].properties[1].value == "value2");
    ASSERT_TRUE(element->interfaces[0].properties[1].type == "type2");
    ASSERT_TRUE(element->interfaces[1].name == "name2");
    ASSERT_TRUE(element->interfaces[1].version == "version2");
    ASSERT_TRUE(element->interfaces[1].type == "configuration");
    ASSERT_TRUE(element->interfaces[1].properties.size() == 2);
    ASSERT_TRUE(element->interfaces[1].properties[0].name == "name1");
    ASSERT_TRUE(element->interfaces[1].properties[0].value == "value1");
    ASSERT_TRUE(element->interfaces[1].properties[0].type == "type1");
    ASSERT_TRUE(element->interfaces[1].properties[1].name == "name2");
    ASSERT_TRUE(element->interfaces[1].properties[1].value == "value2");
    ASSERT_TRUE(element->interfaces[1].properties[1].type == "type2");

    ASSERT_TRUE(element->requirements.size() == 2);
    ASSERT_TRUE(element->requirements[0].name == "name1");
    ASSERT_TRUE(element->requirements[0].version == "version1");
    ASSERT_TRUE(element->requirements[1].name == "name2");
    ASSERT_TRUE(element->requirements[1].version == "version2");
}

TEST(tester_meta_model, Loader_Element_OKs)
{
    meta_model::MetaModelLoader loader;

    std::vector<std::string> msgs;
    auto element = loader.loadElementType("test_files/ok1.fep_element", msgs);
    ASSERT_TRUE(element != nullptr);
    ASSERT_TRUE(msgs.size() == 0); //no warnings no error

    ASSERT_TRUE(element->type_name == "type_name1");
    ASSERT_TRUE(element->description == "description1");
    ASSERT_TRUE(element->type_version == "version1");
    ASSERT_TRUE(element->contact == "contact1");

    ASSERT_TRUE(element->interfaces.size() == 2);
    ASSERT_TRUE(element->interfaces[0].name == "name1");
    ASSERT_TRUE(element->interfaces[0].version == "version1");
    ASSERT_TRUE(element->interfaces[0].type == "configuration");
    ASSERT_TRUE(element->interfaces[0].properties.size() == 2);
    ASSERT_TRUE(element->interfaces[0].properties[0].name == "name1");
    ASSERT_TRUE(element->interfaces[0].properties[0].value == "value1");
    ASSERT_TRUE(element->interfaces[0].properties[0].type == "type1");
    ASSERT_TRUE(element->interfaces[0].properties[1].name == "name2");
    ASSERT_TRUE(element->interfaces[0].properties[1].value == "value2");
    ASSERT_TRUE(element->interfaces[0].properties[1].type == "type2");
    ASSERT_TRUE(element->interfaces[1].name == "name2");
    ASSERT_TRUE(element->interfaces[1].version == "version2");
    ASSERT_TRUE(element->interfaces[1].type == "another");
    ASSERT_TRUE(element->interfaces[1].properties.size() == 2);
    ASSERT_TRUE(element->interfaces[1].properties[0].name == "name1");
    ASSERT_TRUE(element->interfaces[1].properties[0].value == "value1");
    ASSERT_TRUE(element->interfaces[1].properties[0].type == "type1");
    ASSERT_TRUE(element->interfaces[1].properties[1].name == "name2");
    ASSERT_TRUE(element->interfaces[1].properties[1].value == "value2");
    ASSERT_TRUE(element->interfaces[1].properties[1].type == "type2");

    ASSERT_TRUE(element->requirements.size() == 2);
    ASSERT_TRUE(element->requirements[0].name == "name1");
    ASSERT_TRUE(element->requirements[0].version == "version1");
    ASSERT_TRUE(element->requirements[1].name == "name2");
    ASSERT_TRUE(element->requirements[1].version == "version2");
}

TEST(tester_meta_model, Loader_Element_NOKs)
{
    meta_model::MetaModelLoader loader;

    for (int nok = 1; nok <= 15; ++nok)
    {
        std::vector<std::string> msgs;
        auto element = loader.loadElementType(a_util::strings::format("test_files/nok%d.fep_element", nok), msgs);
        ASSERT_TRUE(element == nullptr);
        ASSERT_TRUE(!msgs.empty());
    }
}

TEST(tester_meta_model, Loader_System_OKs)
{
    meta_model::MetaModelLoader loader;
    std::vector<std::string> messages;

    auto system = loader.loadSystem("test_files/ok1.fep_system", messages);
    ASSERT_TRUE(system == nullptr);
    ASSERT_TRUE(!messages.empty());
    messages.clear();

    auto type1 = loader.loadElementType("test_files/system_type1.fep_element", messages);
    ASSERT_TRUE(type1 != nullptr);

    system = loader.loadSystem("test_files/ok1.fep_system", messages);
    ASSERT_TRUE(system == nullptr);
    ASSERT_TRUE(!messages.empty());
    messages.clear();

    auto type2 = loader.loadElementType("test_files/system_type2.fep_element", messages);
    ASSERT_TRUE(type2 != nullptr);

    system = loader.loadSystem("test_files/ok1.fep_system", messages);
    ASSERT_TRUE(system == nullptr);
    ASSERT_TRUE(!messages.empty());
    messages.clear();

    auto type3 = loader.loadElementType("test_files/system_type3.fep_element", messages);
    ASSERT_TRUE(type3 != nullptr);

    system = loader.loadSystem("test_files/ok1.fep_system", messages);
    ASSERT_TRUE(system != nullptr);
    ASSERT_TRUE(messages.empty());

    ASSERT_TRUE(system->description == "description1");
    ASSERT_TRUE(system->version == "version1");
    ASSERT_TRUE(system->contact == "contact1");
    ASSERT_TRUE(system->global_requirement_resolutions["systemrequirement"] == "systemresolution");

    ASSERT_TRUE(system->properties.size() == 2);
    ASSERT_TRUE(system->properties["name1"].name == "name1");
    ASSERT_TRUE(system->properties["name1"].type == "type1");
    ASSERT_TRUE(system->properties["name1"].value == "value1");
    ASSERT_TRUE(system->properties["name2"].name == "name2");
    ASSERT_TRUE(system->properties["name2"].type == "type2");
    ASSERT_TRUE(system->properties["name2"].value == "value2");

    ASSERT_TRUE(system->elements.size() == 3);

    ASSERT_TRUE(system->elements.at("name1").type_name == "type_name1");
    ASSERT_TRUE(system->elements.at("name1").type_version == "type_version1");
    ASSERT_TRUE(system->elements.at("name1").type == type1);
    const auto& element1_interfaces = system->elements.at("name1").interface_instances;
    ASSERT_TRUE(element1_interfaces.size() == 2);
    ASSERT_TRUE(element1_interfaces.at("name1").properties.size() == 1);
    ASSERT_TRUE(element1_interfaces.at("name1").properties.at("name1").name == "name1");
    ASSERT_TRUE(element1_interfaces.at("name1").properties.at("name1").type == "type1");
    ASSERT_TRUE(element1_interfaces.at("name1").properties.at("name1").value == "value1");
    ASSERT_TRUE(element1_interfaces.at("name2").properties.size() == 1);
    ASSERT_TRUE(element1_interfaces.at("name2").properties.at("name1").name == "name1");
    ASSERT_TRUE(element1_interfaces.at("name2").properties.at("name1").type == "type1");
    ASSERT_TRUE(element1_interfaces.at("name2").properties.at("name1").value == "value1");
    ASSERT_TRUE(system->elements.at("name1").requirement_resolutions.size() == 2);
    ASSERT_TRUE(system->elements.at("name1").requirement_resolutions.at("requirement1") == "resolution1");
    ASSERT_TRUE(system->elements.at("name1").requirement_resolutions.at("systemrequirement") == "systemresolution");

    ASSERT_TRUE(system->elements.at("name2").type_name == "type_name2");
    ASSERT_TRUE(system->elements.at("name2").type_version == "type_version2");
    ASSERT_TRUE(system->elements.at("name2").type == type2);
    const auto& element2_interfaces = system->elements.at("name2").interface_instances;
    ASSERT_TRUE(element2_interfaces.size() == 2);
    ASSERT_TRUE(element2_interfaces.at("name1").properties.size() == 1);
    ASSERT_TRUE(element2_interfaces.at("name1").properties.at("name1").name == "name1");
    ASSERT_TRUE(element2_interfaces.at("name1").properties.at("name1").type == "type1");
    ASSERT_TRUE(element2_interfaces.at("name1").properties.at("name1").value == "value1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.size() == 2);
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name1").name == "name1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name1").type == "type1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name1").value == "value1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name2").name == "name2");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name2").type == "type2");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name2").value == "value2");
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.size() == 3);
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.at("requirement1") == "resolution1");
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.at("requirement2") == "resolution2");
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.at("systemrequirement") == "own_private_resolution");

    ASSERT_TRUE(system->elements.at("name3").type_name == "type_name3");
    ASSERT_TRUE(system->elements.at("name3").type_version == "type_version3");
    ASSERT_TRUE(system->elements.at("name3").type == type3);
    const auto& element3_interfaces = system->elements.at("name3").interface_instances;
    ASSERT_TRUE(element3_interfaces.size() == 1);
    ASSERT_TRUE(element3_interfaces.at("name1").properties.size() == 1);
    ASSERT_TRUE(element3_interfaces.at("name1").properties.at("name1").name == "name1");
    ASSERT_TRUE(element3_interfaces.at("name1").properties.at("name1").type == "type1");
    ASSERT_TRUE(element3_interfaces.at("name1").properties.at("name1").value == "value1");
    ASSERT_TRUE(system->elements.at("name3").requirement_resolutions.size() == 2);
    ASSERT_TRUE(system->elements.at("name3").requirement_resolutions.at("requirement1") == "resolution1");
    ASSERT_TRUE(system->elements.at("name3").requirement_resolutions.at("systemrequirement") == "systemresolution");
}

TEST(tester_meta_model, Loader_System_NOKs)
{
    meta_model::MetaModelLoader loader;

    std::vector<std::string> messages;
    auto type1 = loader.loadElementType("test_files/system_type1.fep_element", messages);
    ASSERT_TRUE(type1 != nullptr);

    auto type2 = loader.loadElementType("test_files/system_type2.fep_element", messages);
    ASSERT_TRUE(type2 != nullptr);

    auto type3 = loader.loadElementType("test_files/system_type3.fep_element", messages);
    ASSERT_TRUE(type3 != nullptr);

    for (int nok = 1; nok <= 14; ++nok)
    {
        std::vector<std::string> msgs;
        auto system = loader.loadSystem(a_util::strings::format("test_files/nok%d.fep_system", nok), msgs);
        ASSERT_TRUE(system == nullptr);
        ASSERT_TRUE(!msgs.empty());
    }
}

TEST(tester_meta_model, Writer_Element_Description_OKs)
{
    std::vector<std::string> messages;
    meta_model::MetaModelLoader loader;
    auto element1 = loader.loadElementType("test_files/ok_generate_template.fep_element", messages);
    ASSERT_TRUE(element1 != NULL);
    meta_model::ElementType element_description(*element1);
    element_description.file_path = "test_files/ok_generated_description.fep_element";
    ASSERT_TRUE(meta_model::writeElementDescription(element_description, messages));
    ASSERT_TRUE(messages.empty());

    auto element2 = loader.loadElementType(element_description.file_path, messages);
    ASSERT_TRUE(element2 != nullptr);
    
    ASSERT_TRUE(element2->type_name == element1->type_name);
    ASSERT_TRUE(element2->description == element1->description);
    ASSERT_TRUE(element2->type_version == element1->type_version);
    ASSERT_TRUE(element2->contact == element1->contact);

    ASSERT_TRUE(element2->interfaces.size() == element1->interfaces.size());
    ASSERT_TRUE(element2->interfaces[0].name == element1->interfaces[0].name);
    ASSERT_TRUE(element2->interfaces[0].version == element1->interfaces[0].version);
    ASSERT_TRUE(element2->interfaces[0].type == element1->interfaces[0].type);
    ASSERT_TRUE(element2->interfaces[0].properties.size() == element1->interfaces[0].properties.size());
    ASSERT_TRUE(element2->interfaces[0].properties[0].name == element1->interfaces[0].properties[0].name);
    ASSERT_TRUE(element2->interfaces[0].properties[0].value == element1->interfaces[0].properties[0].value);
    ASSERT_TRUE(element2->interfaces[0].properties[0].type == element1->interfaces[0].properties[0].type);
    ASSERT_TRUE(element2->interfaces[0].properties[1].name == element1->interfaces[0].properties[1].name);
    ASSERT_TRUE(element2->interfaces[0].properties[1].value == element1->interfaces[0].properties[1].value);
    ASSERT_TRUE(element2->interfaces[0].properties[1].type == element1->interfaces[0].properties[1].type);
    ASSERT_TRUE(element2->interfaces[1].name == element1->interfaces[1].name);
    ASSERT_TRUE(element2->interfaces[1].version == element1->interfaces[1].version);
    ASSERT_TRUE(element2->interfaces[1].type == element1->interfaces[1].type);
    ASSERT_TRUE(element2->interfaces[1].properties.size() == element1->interfaces[1].properties.size());
    ASSERT_TRUE(element2->interfaces[1].properties[0].name == element1->interfaces[1].properties[0].name);
    ASSERT_TRUE(element2->interfaces[1].properties[0].value == element1->interfaces[1].properties[0].value);
    ASSERT_TRUE(element2->interfaces[1].properties[0].type == element1->interfaces[1].properties[0].type);
    ASSERT_TRUE(element2->interfaces[1].properties[1].name == element1->interfaces[1].properties[1].name);
    ASSERT_TRUE(element2->interfaces[1].properties[1].value == element1->interfaces[1].properties[1].value);
    ASSERT_TRUE(element2->interfaces[1].properties[1].type == element1->interfaces[1].properties[1].type);

    ASSERT_TRUE(element2->requirements.size() == element1->requirements.size());
    ASSERT_TRUE(element2->requirements[0].name == element1->requirements[0].name);
    ASSERT_TRUE(element2->requirements[0].version == element1->requirements[0].version);
    ASSERT_TRUE(element2->requirements[1].name == element1->requirements[1].name);
    ASSERT_TRUE(element2->requirements[1].version == element1->requirements[1].version);
}

TEST(tester_meta_model, Writer_Element_Description_NOKs)
{
    std::vector<std::string> messages;
    meta_model::ElementType element_description;
    ASSERT_FALSE(meta_model::writeElementDescription(element_description, messages));
    ASSERT_EQ(messages.size(), 4);
    for (auto msg : messages)
    {
        ASSERT_TRUE(msg.find("please fill the description properly.") != std::string::npos);
    }
    messages.clear();
    element_description.type_name = "albrecht";
    element_description.description = "some_good_description";
    element_description.type_version = "1";
    element_description.contact = "some_contact@audi.de";
    element_description.file_path = "test_files/created_description_nok.fep_element";

    ASSERT_TRUE(meta_model::writeElementDescription(element_description, messages));
}

TEST(tester_meta_model, Writer_Property_OKs)
{
    std::vector<std::string> messages;
    auto properties1 = meta_model::loadProperties("test_files/ok1.fep_properties", messages);
    ASSERT_TRUE(properties1 != nullptr);
    ASSERT_TRUE(messages.empty());
    ASSERT_TRUE(meta_model::writePropertyDescription(*properties1,
        "test_files/ok_generated_description.fep_properties", messages));

    auto properties2 = meta_model::loadProperties("test_files/ok_generated_description.fep_properties", messages);
    ASSERT_TRUE(properties2 != nullptr);
    ASSERT_TRUE(messages.empty());
    ASSERT_TRUE(properties2->size() == properties1->size());

    ASSERT_TRUE(properties2->at(0).name == properties1->at(0).name);
    ASSERT_TRUE(properties2->at(0).value == properties1->at(0).value);
    ASSERT_TRUE(properties2->at(0).type == properties1->at(0).type);

    ASSERT_TRUE(properties2->at(1).name == properties1->at(1).name);
    ASSERT_TRUE(properties2->at(1).value == properties1->at(1).value);
    ASSERT_TRUE(properties2->at(1).type == properties1->at(1).type);

    ASSERT_TRUE(properties2->at(2).name == properties1->at(2).name);
    ASSERT_TRUE(properties2->at(2).value == properties1->at(2).value);
    ASSERT_TRUE(properties2->at(2).type == properties1->at(2).type);
}

TEST(tester_meta_model, Writer_System_OK)
{
    meta_model::MetaModelLoader loader;
    std::vector<std::string> messages;

    auto type1 = loader.loadElementType("test_files/system_type1.fep_element", messages);
    ASSERT_TRUE(type1 != nullptr);
    
    auto type2 = loader.loadElementType("test_files/system_type2.fep_element", messages);
    ASSERT_TRUE(type2 != nullptr);

    auto type3 = loader.loadElementType("test_files/system_type3.fep_element", messages);
    ASSERT_TRUE(type3 != nullptr);

    auto system = loader.loadSystem("test_files/ok1.fep_system", messages);
    ASSERT_TRUE(system != nullptr);
    ASSERT_TRUE(messages.empty());

    system->file_path = "test_files/ok_generated_description.fep_system";

    messages.clear();
    ASSERT_TRUE(meta_model::writeSystemDescription(*system, messages));
    ASSERT_TRUE(messages.empty());

    system = nullptr;
    system = loader.loadSystem("test_files/ok_generated_description.fep_system", messages);
    ASSERT_TRUE(system != nullptr);
    ASSERT_TRUE(messages.empty());

    ASSERT_TRUE(system->description == "description1");
    ASSERT_TRUE(system->version == "version1");
    ASSERT_TRUE(system->contact == "contact1");
    ASSERT_TRUE(system->global_requirement_resolutions.at("systemrequirement") == "systemresolution");

    ASSERT_TRUE(system->properties.size() == 2);
    ASSERT_TRUE(system->properties["name1"].name == "name1");
    ASSERT_TRUE(system->properties["name1"].type == "type1");
    ASSERT_TRUE(system->properties["name1"].value == "value1");
    ASSERT_TRUE(system->properties["name2"].name == "name2");
    ASSERT_TRUE(system->properties["name2"].type == "type2");
    ASSERT_TRUE(system->properties["name2"].value == "value2");

    ASSERT_TRUE(system->elements.size() == 3);

    ASSERT_TRUE(system->elements.at("name1").type_name == "type_name1");
    ASSERT_TRUE(system->elements.at("name1").type_version == "type_version1");
    ASSERT_TRUE(system->elements.at("name1").type == type1);
    const auto& element1_interfaces = system->elements.at("name1").interface_instances;
    ASSERT_TRUE(element1_interfaces.size() == 2);
    ASSERT_TRUE(element1_interfaces.at("name1").properties.size() == 1);
    ASSERT_TRUE(element1_interfaces.at("name1").properties.at("name1").name == "name1");
    ASSERT_TRUE(element1_interfaces.at("name1").properties.at("name1").type == "type1");
    ASSERT_TRUE(element1_interfaces.at("name1").properties.at("name1").value == "value1");
    ASSERT_TRUE(element1_interfaces.at("name2").properties.size() == 1);
    ASSERT_TRUE(element1_interfaces.at("name2").properties.at("name1").name == "name1");
    ASSERT_TRUE(element1_interfaces.at("name2").properties.at("name1").type == "type1");
    ASSERT_TRUE(element1_interfaces.at("name2").properties.at("name1").value == "value1");
    ASSERT_TRUE(system->elements.at("name1").requirement_resolutions.size() == 2);
    ASSERT_TRUE(system->elements.at("name1").requirement_resolutions.at("requirement1") == "resolution1");
    ASSERT_TRUE(system->elements.at("name1").requirement_resolutions.at("systemrequirement") == "systemresolution");

    ASSERT_TRUE(system->elements.at("name2").type_name == "type_name2");
    ASSERT_TRUE(system->elements.at("name2").type_version == "type_version2");
    ASSERT_TRUE(system->elements.at("name2").type == type2);
    const auto& element2_interfaces = system->elements.at("name2").interface_instances;
    ASSERT_TRUE(element2_interfaces.size() == 2);
    ASSERT_TRUE(element2_interfaces.at("name1").properties.size() == 1);
    ASSERT_TRUE(element2_interfaces.at("name1").properties.at("name1").name == "name1");
    ASSERT_TRUE(element2_interfaces.at("name1").properties.at("name1").type == "type1");
    ASSERT_TRUE(element2_interfaces.at("name1").properties.at("name1").value == "value1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.size() == 2);
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name1").name == "name1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name1").type == "type1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name1").value == "value1");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name2").name == "name2");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name2").type == "type2");
    ASSERT_TRUE(element2_interfaces.at("name2").properties.at("name2").value == "value2");
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.size() == 3);
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.at("requirement1") == "resolution1");
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.at("requirement2") == "resolution2");
    ASSERT_TRUE(system->elements.at("name2").requirement_resolutions.at("systemrequirement") == "own_private_resolution");

    ASSERT_TRUE(system->elements.at("name3").type_name == "type_name3");
    ASSERT_TRUE(system->elements.at("name3").type_version == "type_version3");
    ASSERT_TRUE(system->elements.at("name3").type == type3);
    const auto& element3_interfaces = system->elements.at("name3").interface_instances;
    ASSERT_TRUE(element3_interfaces.size() == 1);
    ASSERT_TRUE(element3_interfaces.at("name1").properties.size() == 1);
    ASSERT_TRUE(element3_interfaces.at("name1").properties.at("name1").name == "name1");
    ASSERT_TRUE(element3_interfaces.at("name1").properties.at("name1").type == "type1");
    ASSERT_TRUE(element3_interfaces.at("name1").properties.at("name1").value == "value1");
    ASSERT_TRUE(system->elements.at("name3").requirement_resolutions.size() == 2);
    ASSERT_TRUE(system->elements.at("name3").requirement_resolutions.at("requirement1") == "resolution1");
    ASSERT_TRUE(system->elements.at("name3").requirement_resolutions.at("systemrequirement") == "systemresolution");

    auto system_dump = meta_model::dumpSystem(*system);
    ASSERT_TRUE(system_dump.size() > 0);
}

TEST(tester_meta_model, NameValidationOK)
{
    meta_model::MetaModelLoader loader;
    std::vector<std::string> messages;

    auto type1 = loader.loadElementType("test_files/system_type1.fep_element", messages);
    ASSERT_TRUE(type1 != nullptr);

    auto type2 = loader.loadElementType("test_files/system_type2.fep_element", messages);
    ASSERT_TRUE(type2 != nullptr);

    auto type3 = loader.loadElementType("test_files/system_type3.fep_element", messages);
    ASSERT_TRUE(type3 != nullptr);

    auto system = loader.loadSystem("test_files/nok20.fep_system", messages);
    ASSERT_TRUE(system == nullptr);
    ASSERT_TRUE(!messages.empty());

    system = loader.loadSystem("test_files/ok1.fep_system", messages);
    ASSERT_TRUE(messages.empty());

    system->file_path = "test_files/ok_generated_description.fep_system";
    system->elements.at("name1").name = "S0me-Nam3.With_A Space"; 
    system->elements.at("name1").requirement_resolutions.at("requirement1") = "S0me-Re5.With_A Space";

    messages.clear();
    ASSERT_TRUE(meta_model::writeSystemDescription(*system, messages));
    ASSERT_TRUE(messages.empty());

    system = nullptr;
    system = loader.loadSystem("test_files/ok_generated_description.fep_system", messages);
    ASSERT_TRUE(system != nullptr);
    ASSERT_TRUE(messages.empty());

    ASSERT_TRUE(system->description == "description1");
    ASSERT_TRUE(system->version == "version1");
    ASSERT_TRUE(system->contact == "contact1");


    ASSERT_TRUE(system->elements.size() == 3);

    ASSERT_TRUE(system->elements.at("S0me-Nam3.With_A Space").type_name == "type_name1");
    ASSERT_TRUE(system->elements.at("S0me-Nam3.With_A Space").type_version == "type_version1");
    ASSERT_TRUE(system->elements.at("S0me-Nam3.With_A Space").type == type1);
    ASSERT_TRUE(system->elements.at("S0me-Nam3.With_A Space").requirement_resolutions.at("requirement1") == 
        "S0me-Re5.With_A Space");
}

TEST(tester_meta_model, Checker)
{
    // TODO: Test meta model checker!
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
