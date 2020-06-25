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
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace meta_model
{
    struct Property;
    struct ElementType;
    struct System;

    std::string dumpSystem(const System& system);

    /// The MetaModelLoader holds a database of element types.
    /// Based on that it can load system descriptions with consistent references to its element types
    class MetaModelLoader
    {
        std::map<std::string, std::shared_ptr<ElementType>> _known_element_types;

    public:
        MetaModelLoader() = default;
        MetaModelLoader(const MetaModelLoader&) = delete;
        MetaModelLoader& operator=(const MetaModelLoader&) = delete;

        /// Loads an element type from a description file, adding it to the element type database
        std::shared_ptr<const ElementType>
            loadElementType(const std::string& element_desc_file_path, std::vector<std::string>& messages);

        /// Loads a system description from a description file, resolving all element types using its type database
        std::unique_ptr<System>
            loadSystem(const std::string& system_desc_file_path, std::vector<std::string>& messages) const;
    };

    /// Loads all properties from a description file
    std::unique_ptr<const std::vector<Property>>
        loadProperties(const std::string& property_desc_file_path, std::vector<std::string>& messages);

    // Parse system file without any verification of elements 
    std::unique_ptr<System> parseSystem(const std::string& system_desc_file_path, std::vector<std::string>& messages);
}
