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
#include "clara.hpp"
#include "a_util/xml.h"
#include "a_util/strings.h"
#include "a_util/filesystem.h"
#include "a_util/process.h"

#include "meta_model/meta_model_types.h"
#include "meta_model/meta_model_loader.h"
#include "meta_model/meta_model_checker.h"
#include "meta_model/meta_model_configuration.h"

#include "launcher_base/launcher.h"                 // -> #include <cstdint>
#include "launcher_base/launch_config_loader.h"
#include "launcher_base/launch_configuration.h"
#include "launch_strategies.h"
#include "common/platform_fixes.h"                  // -> #include <memory>

using a_util::strings::format;

namespace launcher_base
{    
    int parseArguments(int argc, char* argv[], LaucherArguments& args, bool& should_exit)
    {
        bool help = false;
        auto cli =
            clara::Opt(args.system_name, "system name")["-n"]["--name"]("system name") |
            clara::Opt(args.system_description, "system description file")["-s"]["--system"]("system description file") |
            clara::Opt(args.launch_configuration, "launch configuration")["-c"]["--configuration"]("launch configuration file") |
            clara::Opt(args.element_descriptions, "element descriptions")["-e"]["--element"]("element description file") |
            clara::Opt(args.property_descriptions, "property descriptions")["-p"]["--properties"]("property description file") |
            clara::Opt(args.waiting_timeout, "timeout [ms]")["-t"]["--timeout"]("timeout base for participant responds") |
            clara::Opt(args.environment_variables, "environment variables")["--environment"]("variable=value") |
            clara::Opt(args.accept_launched_participants)["--accept_launched_participants"]("do not fail if participants already launched") |
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

    static void resolveEnvironmentVars(launcher_base::LaunchConfig::Participant& config,
        const std::map<std::string, std::string>& env_variables)
    {
        auto lookup = [&](const std::string& variable) -> std::string
        {
            auto env = env_variables.find(variable);
            if (env != env_variables.end())
            {
                return env->second;
            }
            if (variable == "name")
            {
                return config.name;
            }

            return std::string();
        };

        // I sure would love to use std::regex and iterate+replace but GCC 4.8 sucks and is still supported...
        auto resolve = [&](std::string str) -> std::string {
            std::string resolved;
            size_t prev_end = 0;
            while (true)
            {
                auto begin = str.find("$(", prev_end);
                if (begin == std::string::npos)
                {
                    std::copy(str.begin() + prev_end, str.end(), std::back_inserter(resolved));
                    break;
                }
                std::copy(str.begin() + prev_end, str.begin() + begin, std::back_inserter(resolved));

                auto end = str.find(')', begin);
                if (end == std::string::npos)
                {
                    break;
                }
                std::string variable;
                std::copy(str.begin() + begin + 2, str.begin() + end, std::back_inserter(variable));
                resolved.append(lookup(variable));

                prev_end = end + 1;
            }
            return resolved;
        };

        config.executable = resolve(config.executable);
        config.arguments = resolve(config.arguments);
    }

    static bool checkLaunchConfig(const launcher_base::LaunchConfig& config, const meta_model::System& system)
    {
        bool ok = true;

        // check if all elements are configured for launch
        for (const auto& instance : system.elements)
        {
            if (config.participants.find(instance.first) == config.participants.cend())
            {
                std::cerr << "No launch configuration found for element instance " << instance.first << "\n";
                ok = false;
            }
        }

        // TODO: Add further checks

        return ok;
    }

    static int performLaunch(LauncherRuntimeInterface& runtime, const launcher_base::LaunchConfig& launch_config,
        const std::string& system_name, bool accept_launched_participants,
        const meta_model::System& system, const std::map<std::string, std::string>& env_variables,
        RemoteStartInterface* remote_access)
    {
        if (!runtime.setContext(system, system_name))
        {
            return 1;
        }

        if (!checkLaunchConfig(launch_config, system))
        {
            return 1;
        }

        // Temporarily apply environment variables for child processes
        // TODO: Fix a_util::process to implement not just the minimum..
        for (const auto& env : env_variables)
        {
            a_util::process::setEnvVar(env.first, env.second);
        }

        std::vector<std::string> all_names;
        std::transform(system.elements.cbegin(), system.elements.cend(), std::back_inserter(all_names),
            [](const std::pair<std::string, meta_model::System::ElementInstance>& p) { return p.second.name; });

        // launch participants
        for (const auto& instance : system.elements)
        {
            auto config = launch_config.participants.find(instance.first)->second;
            resolveEnvironmentVars(config, env_variables);

            if (!config.remote_starter_alias.empty())
            {
                if (remote_access == nullptr)
                {
                    std::cerr << "Launcher runtime backend does not support remote starts!\n";
                    std::cerr << format("Failed to launch participant %s since launcher runtime backend does not support remote starts!\n",
                        config.name.c_str());
                    return 1;
                }

                if (!launchWithRemoteStarter(config, runtime, accept_launched_participants,
                    remote_access))
                {
                    std::cerr << format("Failed to launch participant %s!\n", config.name.c_str());
                    std::cerr << "Shutting system down!" << std::endl;
                    runtime.cleanUp(all_names);
                    return 1;
                }
            }
            else
            {
                // TODO: If SSH support is ever added, the host name has to be used to differentiate between local and remote launches
                if (!launchLocally(config, runtime, accept_launched_participants))
                {
                    std::cerr << format("Failed to launch participant %s!\n", config.name.c_str());
                    std::cerr << "Shutting system down!" << std::endl;
                    runtime.cleanUp(all_names);
                    return 1;
                }
            }
        }

        // Revert environment variables
        // TODO: Use a better API than a_util::process

        // wait for them to reach idle

        std::vector<std::string> failed;
        if (!runtime.awaitParticipantsIdle(all_names, failed))
        {
            std::cerr << format("The following participants failed to reach Idle: %s!\n",
                a_util::strings::join(failed, ", ").c_str());
            return 1;
        }

        a_util::filesystem::Path base_path(system.file_path);
        base_path.removeLastElement();

        // configure participants
        for (const auto& instance : system.elements)
        {
            std::cout << "Configuring " << instance.second.name << "... ";

            // apply intf properties
            for (const auto& intf_param : instance.second.interface_instances)
            {
                if (!runtime.configureInterface(intf_param.second, instance.first, base_path))
                {
                    std::cerr << format("Failed to configure interface %s of participant %s!\n",
                        intf_param.first.c_str(), instance.first.c_str());
                    return 1;
                }
            }

            // apply intf resolutions
            for (const auto& intf_res : instance.second.requirement_resolutions)
            {
                const auto& res_instance = system.elements.at(intf_res.second);
                if (!runtime.applyRequirementResolution(res_instance, intf_res.first, instance.first))
                {
                    std::cerr << format("Failed to configure requirement %s of participant %s!\n",
                        intf_res.first.c_str(), instance.first.c_str());
                    return 1;
                }
            }

            std::cout << "OK\n";
        }

        std::cout << "Successfully launched system '" << system_name << "'\n";
        return 0;
    }

    int launchSystem(int argc, char* argv[], LauncherRuntimeInterface& runtime,
        RemoteStartInterface* remote_access/*=nullptr*/)
    {
        LaucherArguments args;
        bool exit = false;

        int ret = parseArguments(argc, argv, args, exit);
        if (ret != 0 || exit)
        {
            return ret;
        }

        if (args.system_name.empty())
        {
            std::cerr << "System name not specified!\n";
            return 1;
        }
        return launchSystem(args, runtime, remote_access);
    }

    int launchSystem(const LaucherArguments& args, LauncherRuntimeInterface& runtime, RemoteStartInterface* remote_access /*= nullptr*/)
    {
        std::vector<std::string> messages;

        std::map<std::string, std::string> env_variables;
        for (const auto& str : args.environment_variables)
        {
            auto parts = a_util::strings::split(str, "=");
            if (parts.size() != 2)
            {
                std::cerr << "Could not parse environment variable '" << str << "':\n";
                return 1;
            }
            env_variables[parts[0]] = parts[1];
        }

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

        // load & apply properties to the system
        for (const auto& prop : args.property_descriptions)
        {
            auto properties = meta_model::loadProperties(prop, messages);
            if (!properties)
            {
                std::cerr << "Could not load property description '" << prop << "':\n";
                for (const auto& msg : messages)
                {
                    std::cerr << "  -" << msg << "\n";
                }
                return 1;
            }

            if (!meta_model::configureSystem(*system, *properties, messages))
            {
                std::cerr << "Could not apply property description '" << prop << "':\n";
                for (const auto& msg : messages)
                {
                    std::cerr << "  -" << msg << "\n";
                }
                return 1;
            }
        }

        // collapse referenced properties now
        meta_model::resolveReferencedProperties(*system);

        if (args.launch_configuration.empty())
        {
            std::cerr << "Launch configuration file not specified!\n";
            return 1;
        }

        messages.clear();
        auto launch_config = launcher_base::loadLaunchConfiguration(args.launch_configuration, messages);
        if (!launch_config)
        {
            std::cerr << "Could not load launch configuration '" << args.launch_configuration << "':\n";
            for (const auto& msg : messages)
            {
                std::cerr << "  -" << msg << "\n";
            }
            return 1;
        }

        if (args.verbose)
        {
            std::cout << "VERBOSE:\n" << meta_model::dumpSystem(*system) << "\n";
        }

        return performLaunch(runtime, *launch_config, args.system_name, args.accept_launched_participants, *system, env_variables, remote_access);
    }

}
