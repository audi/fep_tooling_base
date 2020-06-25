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

#include "gtest/gtest.h"
#include "launcher_base/launch_config_loader.h"
#include "launcher_base/launcher_writer.h"
#include "a_util/strings.h"

TEST(tester_launch_config_parser, OKs)
{
    std::vector<std::string> msgs;
    auto conf = launcher_base::loadLaunchConfiguration("test_files/ok1.fep2_launch_config", msgs);
    ASSERT_TRUE(conf != nullptr);
    ASSERT_TRUE(msgs.empty());

    ASSERT_TRUE(conf->description == "Simple local deployment configuration");
    ASSERT_TRUE(conf->contact == "somebody@audi.de");

    ASSERT_TRUE(conf->participants.size() == 3);

    ASSERT_TRUE(conf->participants.at("name1").host == "host1");
    ASSERT_TRUE(conf->participants.at("name1").working_dir == "working_dir1");
    ASSERT_TRUE(conf->participants.at("name1").arguments == "arguments1");
    ASSERT_TRUE(conf->participants.at("name1").executable == "");
    ASSERT_TRUE(conf->participants.at("name1").remote_starter_alias == "remote_starter_alias1");

    ASSERT_TRUE(conf->participants.at("name2").host == "host2");
    ASSERT_TRUE(conf->participants.at("name2").working_dir == "working_dir2");
    ASSERT_TRUE(conf->participants.at("name2").arguments == "arguments2");
    ASSERT_TRUE(conf->participants.at("name2").executable == "executable2");
    ASSERT_TRUE(conf->participants.at("name2").remote_starter_alias == "");

    ASSERT_TRUE(conf->participants.at("name3").host == "host3");
    ASSERT_TRUE(conf->participants.at("name3").working_dir == "working_dir3");
    ASSERT_TRUE(conf->participants.at("name3").arguments == "arguments3");
    ASSERT_TRUE(conf->participants.at("name3").executable == "executable3");
    ASSERT_TRUE(conf->participants.at("name3").remote_starter_alias == "");
}

TEST(tester_launch_config_parser, NOKs)
{
    for (int nok = 1; nok <= 12; ++nok)
    {
        std::vector<std::string> msgs;
        auto conf = launcher_base::loadLaunchConfiguration(a_util::strings::format("test_files/nok%d.fep2_launch_config", nok), msgs);
        ASSERT_TRUE(conf == nullptr);
        ASSERT_TRUE(!msgs.empty());
    }
}


TEST(tester_write_config_parser, OKs)
{
    std::vector<std::string> msgs;
    auto conf = launcher_base::loadLaunchConfiguration("test_files/ok1.fep2_launch_config", msgs);
    ASSERT_TRUE(conf != nullptr);
    ASSERT_TRUE(msgs.empty());
    launcher_base::LaunchConfig conf1(*conf);
    conf1.file_path = "test_files/ok_generated_description.fep2_launch_config";
    ASSERT_TRUE(launcher_base::writeLauncherConfiguration(conf1, msgs));
    ASSERT_TRUE(msgs.empty());

    conf == nullptr;
    conf = launcher_base::loadLaunchConfiguration("test_files/ok_generated_description.fep2_launch_config", msgs);
    ASSERT_TRUE(conf != nullptr);
    ASSERT_TRUE(msgs.empty());

    ASSERT_TRUE(conf->description == "Simple local deployment configuration");
    ASSERT_TRUE(conf->contact == "somebody@audi.de");

    ASSERT_TRUE(conf->participants.size() == 3);

    ASSERT_TRUE(conf->participants.at("name1").host == "host1");
    ASSERT_TRUE(conf->participants.at("name1").working_dir == "working_dir1");
    ASSERT_TRUE(conf->participants.at("name1").arguments == "arguments1");
    ASSERT_TRUE(conf->participants.at("name1").executable == "");
    ASSERT_TRUE(conf->participants.at("name1").remote_starter_alias == "remote_starter_alias1");

    ASSERT_TRUE(conf->participants.at("name2").host == "host2");
    ASSERT_TRUE(conf->participants.at("name2").working_dir == "working_dir2");
    ASSERT_TRUE(conf->participants.at("name2").arguments == "arguments2");
    ASSERT_TRUE(conf->participants.at("name2").executable == "executable2");
    ASSERT_TRUE(conf->participants.at("name2").remote_starter_alias == "");

    ASSERT_TRUE(conf->participants.at("name3").host == "host3");
    ASSERT_TRUE(conf->participants.at("name3").working_dir == "working_dir3");
    ASSERT_TRUE(conf->participants.at("name3").arguments == "arguments3");
    ASSERT_TRUE(conf->participants.at("name3").executable == "executable3");
    ASSERT_TRUE(conf->participants.at("name3").remote_starter_alias == "");
}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
