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
#include <cstdio>
#include <iostream>
#include <string>
#include "clara.hpp"
#include "a_util/xml.h"
#include "a_util/strings.h"
#include "a_util/filesystem.h"

#include "meta_model/meta_model_types.h"
#include "meta_model/meta_model_loader.h"
#include "meta_model/meta_model_checker.h"
#include "meta_model/meta_model_configuration.h"

#include "controller_base/controller.h"

using a_util::strings::format;

namespace controller_base
{
    int parseArguments(int argc, char* argv[], ControllerArguments& args, bool& should_exit)
    {
        bool help = false;
        auto cli =
            clara::Opt(args.system_name, "system name")["-n"]["--name"]("system name") |
            clara::Opt(args.system_description, "system description file")["-s"]["--system"]("system description file") |
            clara::Opt(args.element_descriptions, "element descriptions")["-e"]["--element"]("element description file") |
            clara::Opt(args.commands, "\"command [command args]\"")["-c"]["--command"]("commands") |
            clara::Opt(args.waiting_timeout, "timeout [ms]")["-t"]["--timeout"]("timeout base for participant responds") |
            clara::Opt(args.environment_variables, "environment variables")["--environment"]("variable=value") |
            clara::Opt(args.verbose)["--verbose"]("enable verbose debug output") |
            clara::Help(help);

        if (argc == 0) return 1;

        auto result = cli.parse(clara::Args(argc, argv));
        should_exit = help;

        if (!result)
        {
            printf("Error in command line: %s\n", result.errorMessage().c_str());
            return 1;
        }
        else if (help)
        {
            cli.writeToStream(std::cout);
        }

        return 0;
    }

    static int run_repl(ControllerRuntimeInterface& runtime)
    {
        std::cout << "Entering command prompt. Enter 'quit' to cancel\n";
        while (true)
        {
            std::string cmd;
            if (!std::getline(std::cin, cmd))
            {
                break;
            }
            std::cout << "... ";

            if (cmd == "quit")
            {
                std::cout << "Quitting...\n";
                break;
            }

            std::string msg;
            auto ret = runtime.execute(cmd, msg);
            std::cout << (ret ? "OK" : "FAILED");

            if (!msg.empty())
            {
                std::cout << ": " << msg;
            }
            std::cout << "\n";

            if (ret && cmd.size() >= 8 && cmd.substr(0, 8) == "shutdown")
            {
                std::cout << "Quitting...\n";
                break;
            }
        }

        return 0;
    }

    int controlSystem(int argc, char* argv[], ControllerRuntimeInterface& runtime)
    {
        ControllerArguments args;
        bool exit = false;

        int ret = parseArguments(argc, argv, args, exit);
        if (ret != 0 || exit)
        {
            return ret;
        }

        return controlSystem(args, runtime);
    }

    int controlSystem(const ControllerArguments& args, ControllerRuntimeInterface& runtime)
    {
        if (args.system_name.empty())
        {
            std::cerr << "System name not specified!\n";
            return 1;
        }

        std::vector<std::string> messages;

        meta_model::MetaModelLoader mm_loader;
        for (const auto& elem : args.element_descriptions)
        {
            messages.clear();
            if (!mm_loader.loadElementType(elem, messages))
            {
                std::cerr << "Could not load element description '" << elem << "':\n";
                for (const auto& msg : messages)
                {
                    std::cerr << "  -" << msg << "\n";
                }
                return 1;
            }
        }

        if (args.system_description.empty())
        {
            std::cerr << "System description properties not specified!\n";
            return 1;
        }

        messages.clear();
        auto system = mm_loader.loadSystem(args.system_description, messages);
        if (!system)
        {
            std::cerr << "Could not load system description '" << args.system_description << "':\n";
            for (const auto& msg : messages)
            {
                std::cerr << "  -" << msg << "\n";
            }
            return 1;
        }

        if (!meta_model::checkSystem(*system, args.system_name, messages))
        {
            std::cerr << "System description '" << args.system_description << "' contains errors:\n";
            for (const auto& msg : messages)
            {
                std::cerr << "  -" << msg << "\n";
            }
            return 1;
        }

        if (!runtime.connect(*system, args.system_name, args.verbose))
        {
            return 1;
        }

        if (args.commands.empty())
        {
            auto ret_val = run_repl(runtime);
            runtime.disconnect();
            return ret_val;
        }
        else
        {
            for (const auto& cmd : args.commands)
            {
                std::string msg;
                auto ok = runtime.execute(cmd, msg);
                std::cout << (ok ? "OK" : "FAILED");

                if (!msg.empty())
                {
                    std::cout << ": " << msg;
                }
                std::cout << "\n";

                if (!ok)
                {
                    runtime.disconnect();
                    return 1;
                }
            }
        }

        runtime.disconnect();
        return 0;
    }

}
