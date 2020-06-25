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
#include "launcher_base/launch_config_loader.h"
#include "launcher_base/launch_configuration.h"
#include "a_util/xml.h"
#include "a_util/strings.h"
#include "a_util/filesystem.h"
#include "common/platform_fixes.h"      // -> #include <memory>

using a_util::strings::format;

namespace launcher_base
{
    static const std::string parser_schema_version = "1.0.0";

    static bool parseLaunchConfigParticipant(LaunchConfig::Participant& participant,
        const a_util::xml::DOMElement& participant_node, std::vector<std::string>& messages)
    {
        auto name = participant_node.getChild("name");
        if (name.isNull())
        {
            messages.push_back("Missing name node in participant");
            return false;
        }
        participant.name = name.getData();

        auto host = participant_node.getChild("host");
        if (host.isNull())
        {
            messages.push_back(format("Missing host node in participant %s", participant.name.c_str()));
            return false;
        }
        participant.host = host.getData();

        auto working_dir = participant_node.getChild("working_dir");
        if (working_dir.isNull())
        {
            messages.push_back(format("Missing working_dir node in participant %s", participant.name.c_str()));
            return false;
        }
        participant.working_dir = working_dir.getData();

        auto arguments = participant_node.getChild("arguments");
        if (arguments.isNull())
        {
            messages.push_back(format("Missing arguments node in participant %s", participant.name.c_str()));
            return false;
        }
        participant.arguments = arguments.getData();

        auto executable = participant_node.getChild("executable");
        auto remote_starter_alias = participant_node.getChild("remote_starter_alias");
        if (executable.isNull() && remote_starter_alias.isNull())
        {
            messages.push_back(format("Either executable or remote_starter_alias node missing in participant %s",
                participant.name.c_str()));
            return false;
        }

        if (!executable.isNull() && !remote_starter_alias.isNull())
        {
            messages.push_back(format("Both executable and remote_starter_alias nodes present in participant %s - they are mutually exclusive",
                participant.name.c_str()));
            return false;
        }

        if (!executable.isNull())
        {
            participant.executable = executable.getData();
        }
        else
        {
            participant.remote_starter_alias = remote_starter_alias.getData();
        }

        return true;
    }

    static bool parseLaunchConfig(LaunchConfig& config, const a_util::xml::DOM& dom, std::vector<std::string>& messages)
    {
        auto root = dom.getRoot();
        if (root.getName() != "launch_configuration")
        {
            messages.push_back("File is missing 'launch_configuration' root node");
            return false;
        }

        auto schema_version = root.getChild("schema_version");
        if (schema_version.isNull() || schema_version.getData() != parser_schema_version)
        {
            messages.push_back("Missing schema_version node or invalid version");
            return false;
        }

        auto description = root.getChild("description");
        if (description.isNull())
        {
            messages.push_back("Missing description node");
            return false;
        }
        config.description = description.getData();

        auto contact = root.getChild("contact");
        if (contact.isNull())
        {
            messages.push_back("Missing contact node");
            return false;
        }
        config.contact = contact.getData();
        
        auto participants_node = root.getChild("participants");
        if (participants_node.isNull())
        {
            messages.push_back("Missing participants node");
            return false;
        }

        auto participants = participants_node.getChildren();
        for (const auto& participant_node : participants)
        {
            if (participant_node.getName() == "participant")
            {
                std::pair<std::string, LaunchConfig::Participant> pair;
                if (!parseLaunchConfigParticipant(pair.second, participant_node, messages))
                {
                    return false;
                }

                pair.first = pair.second.name;
                config.participants.insert(std::move(pair));
            }
        }

        return true;
    }

    std::unique_ptr<const LaunchConfig>
        loadLaunchConfiguration(const std::string& launch_config_file_path, std::vector<std::string>& messages)
    {
        messages.clear();

        a_util::xml::DOM dom;
        if (!dom.load(launch_config_file_path))
        {
            messages.push_back(dom.getLastError());
            return nullptr;
        }

#if __cplusplus >= 201402L
        auto config = std::make_unique<LaunchConfig>();
#else
        auto config = std::unique_ptr<LaunchConfig>(new LaunchConfig());
#endif
        if (!parseLaunchConfig(*config, dom, messages))
        {
            return nullptr;
        }

        a_util::filesystem::Path path(launch_config_file_path);
        if (path.isRelative())
        {
            path = a_util::filesystem::getWorkingDirectory();
            path.append(launch_config_file_path);
        }

        config->file_path = path.toString();

#if (__GNUC__ == 4 && __GNUC_MINOR__ < 9)
        return std::move(config); // GCC-4.8 sucks...
#else
        return config;
#endif
    }
}
