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
#include "controller_base/controller.h"

class DummyControllerRuntime : public controller_base::ControllerRuntimeInterface
{
public:
    bool connect(const meta_model::System&, const std::string&, bool) override
    {
        return true;
    }

    bool execute(const std::string&, std::string&) override
    {
        return true;
    }

    void disconnect()
    {
    }
};

TEST(tester_controller_base, CLI)
{
    DummyControllerRuntime runtime;
    ASSERT_TRUE(controller_base::controlSystem(0, nullptr, runtime) != 0);

    std::vector<const char*> args;
    args.push_back("");
    ASSERT_TRUE(controller_base::controlSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--help");
    ASSERT_TRUE(controller_base::controlSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);
    args.erase(args.begin() + 1);

    args.push_back("--name");
    args.push_back("system1");
    ASSERT_TRUE(controller_base::controlSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--system");
    args.push_back("test_files/system.fep_system");
    ASSERT_TRUE(controller_base::controlSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) != 0);

    args.push_back("--element");
    args.push_back("test_files/element.fep_element");

    args.push_back("--command");
    args.push_back("cmd args");

    ASSERT_TRUE(controller_base::controlSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);
}

struct TestControllerRuntime : public controller_base::ControllerRuntimeInterface
{
    bool connect(const meta_model::System&, const std::string& name, bool) override
    {
        EXPECT_TRUE(name == "system1");
        return true;
    }

    bool execute(const std::string& command, std::string&) override
    {
        static int n_cmd = 0;
        n_cmd++;
        if (n_cmd == 1)
        {
            EXPECT_TRUE(command == "cmd1 args");
        }
        else if (n_cmd == 2)
        {
            EXPECT_TRUE(command == "cmd2");
        }
        return true;
    }

    bool disconnected = false;
    void disconnect()
    {
        disconnected = true;
    }
};

TEST(tester_controller_base, RuntimeInterface)
{
    TestControllerRuntime runtime;
    std::vector<const char*> args;
    args.push_back("");

    args.push_back("--name");
    args.push_back("system1");

    args.push_back("--system");
    args.push_back("test_files/system.fep_system");

    args.push_back("--element");
    args.push_back("test_files/element.fep_element");

    args.push_back("--command");
    args.push_back("cmd1 args");

    args.push_back("--command");
    args.push_back("cmd2");

    ASSERT_TRUE(controller_base::controlSystem(static_cast<int>(args.size()), const_cast<char**>(&args[0]), runtime) == 0);
    ASSERT_TRUE(runtime.disconnected);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
