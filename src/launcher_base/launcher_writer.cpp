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
#include "launcher_base/launch_configuration.h"
#include "common/platform_fixes.h"
#include "meta_model/meta_model_types.h"

using a_util::strings::format;
using a_util::xml::DOM;
using a_util::xml::DOMElement;

namespace launcher_base
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

    static bool save_file(DOM dom, const std::string& file_path, std::vector<std::string>& messages)
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

    bool writeLauncherConfiguration(const LaunchConfig& config, std::vector<std::string>& messages)
    {
        DOM dom;
        DOMElement root = dom.getRoot();
        
        // fill the header of the element description
        root.setName("launch_configuration");

        auto schema_version_node = root.createChild("schema_version");
        // right now, the only supported version
        schema_version_node.setData("1.0.0");

        bool is_valid = createNode(root, config.description, "description", messages);
        is_valid &= createNode(root, config.contact, "contact", messages);

        // fill and verify the participants
        auto participants_node = root.createChild("participants");
        for (const auto& participant_map : config.participants)
        {
            auto participant = participant_map.second;
            auto participant_node = participants_node.createChild("participant");
            is_valid &= createNode(participant_node, participant.name, "name", messages);
            is_valid &= createNode(participant_node, participant.host, "host", messages);
            auto working_dir_node = participant_node.createChild("working_dir");
            working_dir_node.setData(participant.working_dir);
            auto arguments_node = participant_node.createChild("arguments");
            arguments_node.setData(participant.arguments);

            if ((participant.executable.empty() && participant.remote_starter_alias.empty())
                || (!participant.executable.empty() && !participant.remote_starter_alias.empty()))
            {
                messages.push_back(format("Exactly one of %s and %s must be set, "
                    " please fill the description properly.\n",
                    participant.executable.c_str(), participant.remote_starter_alias.c_str()));
                is_valid = false;
            }
            else if(!participant.executable.empty())
            {
                is_valid &= createNode(participant_node, participant.executable, "executable", messages);
            }
            else
            {
                is_valid &= createNode(participant_node, participant.remote_starter_alias, 
                    "remote_starter_alias", messages);
            }
        }
        
        if (is_valid)
        {
            // Save file
            is_valid = save_file(dom, config.file_path, messages);
        }

        return is_valid;
    }
}
