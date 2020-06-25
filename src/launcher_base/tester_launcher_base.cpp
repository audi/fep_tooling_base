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
#include <a_util/process.h>
#include <a_util/filesystem.h>
#include "launcher_base/launcher.h"

class DummyLauncherRuntime : public launcher_base::LauncherRuntimeInterface
{
public:
    bool setContext(const meta_model::System&, const std::string&) override
    {
        return true;
    }

    ParticipantState getParticipantState(const std::string&) const override
    {
        return ParticipantState::Idle;
    }

    bool awaitParticipantsIdle(const std::vector<std::string>&,
        std::vector<std::string>&) const override
    {
        return true;
    }

    bool configureInterface(const meta_model::System::ElementInstance::InterfaceInstance&,
        const std::string&, const std::string&) override
    {
        return true;
    }

    bool applyRequirementResolution(const meta_model::System::ElementInstance&,
        const std::string&, const std::string&) override
    {
        return true;
    }

    bool cleanUp(const std::vector<std::string>&) override
    {
        return true;
    }
};

TEST(tester_launcher_base, CLI)
{
    DummyLauncherRuntime runtime;
    ASSERT_TRUE(launcher_base::launchSystem(0, nullptr, runtime) != 0);

    std::vector<const char*> args;
    args.push_back("");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--help");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);
    args.erase(args.begin() + 1);

    args.push_back("--accept_launched_participants");
    args.push_back("--verbose");

    args.push_back("--name");
    args.push_back("system1");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--system");
    args.push_back("test_files/system.fep_system");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--configuration");
    args.push_back("test_files/launch.fep2_launch_config");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--element");
    args.push_back("test_files/element.fep_element");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);

    args.push_back("--properties");
    args.push_back("test_files/property_file.fep_properties");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);

    args.push_back("--environment");
    args.push_back("env_var=env_value");
    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);
}

class TestLauncherRuntime : public launcher_base::LauncherRuntimeInterface
{
public:
    bool setContext(const meta_model::System&, const std::string& system_name) override
    {
        EXPECT_TRUE(system_name == "system1");
        return true;
    }

    ParticipantState getParticipantState(const std::string& participant) const override
    {
        EXPECT_TRUE(a_util::process::getEnvVar("env_var") == "env_value");
        EXPECT_TRUE(participant == "TestElementInstance");
        return ParticipantState::Idle;
    }

    bool awaitParticipantsIdle(const std::vector<std::string>& participants,
        std::vector<std::string>&) const override
    {
        EXPECT_TRUE(participants.size() == 1 && participants[0] == "TestElementInstance");
        return true;
    }

    bool configureInterface(const meta_model::System::ElementInstance::InterfaceInstance& intf,
        const std::string& participant, const std::string& system_desc_base_path) override
    {
        EXPECT_TRUE(participant == "TestElementInstance");
        EXPECT_TRUE(intf.name == "TestInterface");

        EXPECT_TRUE(intf.properties.size() == 2);
        EXPECT_TRUE(intf.properties.at("InterfaceProperty").value == "SystemPropertyValue");

        a_util::filesystem::Path p(system_desc_base_path);
        EXPECT_TRUE(a_util::filesystem::exists(p));
        EXPECT_TRUE(p.isAbsolute());

        return true;
    }

    bool applyRequirementResolution(const meta_model::System::ElementInstance& resolution_instance,
        const std::string& requirement, const std::string& participant) override
    {
        EXPECT_TRUE(participant == "TestElementInstance");
        EXPECT_TRUE(requirement == "TestInterface");
        EXPECT_TRUE(resolution_instance.name == "TestElementInstance");

        return true;
    }

    bool cleanUp(const std::vector<std::string>&) override
    {
        return true;
    }
};

TEST(tester_launcher_base, RuntimeInterface)
{
    TestLauncherRuntime runtime;
    ASSERT_TRUE(launcher_base::launchSystem(0, nullptr, runtime) != 0);

    std::vector<const char*> args;
    args.push_back("");
    args.push_back("--accept_launched_participants");
    args.push_back("--verbose");

    args.push_back("--name");
    args.push_back("system1");

    args.push_back("--system");
    args.push_back("test_files/system.fep_system");

    args.push_back("--configuration");
    args.push_back("test_files/launch.fep2_launch_config");

    args.push_back("--element");
    args.push_back("test_files/element.fep_element");

    args.push_back("--properties");
    args.push_back("test_files/property_file.fep_properties");

    args.push_back("--environment");
    args.push_back("env_var=env_value");

    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);
}

// needed to simulate that participant is NOT started
class TestEmptyRuntime : public launcher_base::LauncherRuntimeInterface
{
public:
    bool setContext(const meta_model::System&, const std::string& system_name) override
    {
        EXPECT_TRUE(system_name == "system1");
        return true;
    }

    ParticipantState getParticipantState(const std::string& participant) const override
    {
        EXPECT_TRUE(participant == "TestElementInstance");
        return ParticipantState::NotStarted;
    }

    bool awaitParticipantsIdle(const std::vector<std::string>& participants,
        std::vector<std::string>&) const override
    {
        EXPECT_TRUE(participants.size() == 1 && participants[0] == "TestElementInstance");
        return true;
    }

    bool configureInterface(const meta_model::System::ElementInstance::InterfaceInstance& intf,
        const std::string& participant, const std::string& system_desc_base_path) override
    {
        EXPECT_TRUE(participant == "TestElementInstance");
        EXPECT_TRUE(intf.name == "TestInterface");

        EXPECT_TRUE(intf.properties.size() == 2);
        EXPECT_TRUE(intf.properties.at("InterfaceProperty").value == "SystemPropertyValue");

        a_util::filesystem::Path p(system_desc_base_path);
        EXPECT_TRUE(a_util::filesystem::exists(p));
        EXPECT_TRUE(p.isAbsolute());
        return true;
    }

    bool applyRequirementResolution(const meta_model::System::ElementInstance& resolution_instance,
        const std::string& requirement, const std::string& participant) override
    {
        EXPECT_TRUE(participant == "TestElementInstance");
        EXPECT_TRUE(requirement == "TestInterface");
        EXPECT_TRUE(resolution_instance.name == "TestElementInstance");
        return true;
    }

    bool cleanUp(const std::vector<std::string>&) override
    {
        return true;
    }
};

class TestRemoterStartAccess : public launcher_base::RemoteStartInterface
{
public:
    bool startRemote(const std::string& host, const std::string& executable,
        const std::string& executable_version = "", const std::string& working_dir = "",
        const std::string& arguments = "") override
    {
        EXPECT_TRUE(host == "SomeHost");
        EXPECT_TRUE(executable == "SomeExecutable");
        EXPECT_TRUE(executable_version == "");
        EXPECT_TRUE(working_dir == "SomeWorkingDir");
        EXPECT_TRUE(arguments == "SomeArguments");
        return true;
    }
};

TEST(tester_launcher_base, TestRemoteStartInterface)
{
    TestEmptyRuntime runtime;
    TestRemoterStartAccess remote_access;
    ASSERT_TRUE(launcher_base::launchSystem(0, nullptr, runtime, &remote_access) != 0);

    std::vector<const char*> args;
    args.push_back("");
    args.push_back("--accept_launched_participants");
    args.push_back("--verbose");

    args.push_back("--name");
    args.push_back("system1");

    args.push_back("--system");
    args.push_back("test_files/system.fep_system");

    args.push_back("--configuration");
    args.push_back("test_files/rs_launch.fep2_launch_config");

    args.push_back("--element");
    args.push_back("test_files/element.fep_element");

    args.push_back("--properties");
    args.push_back("test_files/property_file.fep_properties");

    args.push_back("--environment");
    args.push_back("env_var=env_value");

    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime,
        nullptr) != 0);

    ASSERT_TRUE(launcher_base::launchSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime,
        &remote_access) == 0);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
