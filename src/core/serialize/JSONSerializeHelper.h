/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ArenaForge - An open source venue editing and rendering framework
//
//  Copyright (c) 2025 0x1306a94 (https://github.com/0x1306a94)
//  All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at:
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  Unless required by applicable law or agreed to in writing, software distributed under the
//  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
//  either express or implied. See the License for the specific language governing permissions
//  and limitations under the License.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
//  JSONSerializeHelper.h
//  arenaforge_core
//
//  Created by king on 2025/9/10.
//

#ifndef JSONSerializeHelper_h_ArenaForge
#define JSONSerializeHelper_h_ArenaForge

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace arenaforge::json {

template <typename T>
static bool read_field_internal(T &field, const nlohmann::json &json, const std::string &key, std::optional<T> defaultValue = std::nullopt) {
    if (!json.is_object()) {
        return false;
    }

    auto it = json.find(key);
    if (it == json.end()) {
        if (defaultValue.has_value()) {
            field = defaultValue.value();
            return true;
        } else {
            return false;  // Key not found and no default value provided
        }
    }

    field = it->get<T>();
    return true;
}

template <typename T>
static bool read_object_internal(T &object, const nlohmann::json &json, const std::string &key, std::optional<T> defaultValue = std::nullopt) {
    if (!json.is_object()) {
        return false;
    }

    auto it = json.find(key);
    if (it == json.end()) {
        if (defaultValue.has_value()) {
            object = defaultValue.value();
            return true;
        } else {
            return false;  // Key not found and no default value provided
        }
    }

    object = it->get<T>();
    return true;
}

template <typename T>
static T read_value_internal(const nlohmann::json &json, const std::string &key, std::optional<T> defaultValue = std::nullopt) {
    if (!json.is_object()) {
        if (defaultValue.has_value()) {
            return defaultValue.value();
        } else {
            return T{};
        }
    }

    auto it = json.find(key);
    if (it == json.end()) {
        if (defaultValue.has_value()) {
            return defaultValue.value();
        } else {
            return T{};
        }
    }

    return it->get<T>();
}

template <typename T>
static bool read_field(T &field, const nlohmann::json &json, const std::string &key) {
    return read_field_internal(field, json, key);
}

template <typename T>
static bool read_field_optional(T &field, const nlohmann::json &json, const std::string &key, T defaultValue) {
    return read_field_internal(field, json, key, defaultValue);
}

template <typename T>
static bool read_object(T &object, const nlohmann::json &json, const std::string &key) {
    return read_object_internal(object, json, key);
}

template <typename T>
static bool read_object(T &object, const nlohmann::json &json, const std::string &key, T defaultValue) {
    return read_object_internal(object, json, key, defaultValue);
}

template <typename T>
static T read_value(const nlohmann::json &json, const std::string &key) {
    return read_value_internal<T>(json, key, std::nullopt);
}

template <typename T>
static T read_value(const nlohmann::json &json, const std::string &key, T defaultValue) {
    return read_value_internal<T>(json, key, defaultValue);
}

[[maybe_unused]] static std::string read_string_value(const nlohmann::json &json, const std::string &key) {
    return read_value_internal<std::string>(json, key);
}

[[maybe_unused]] static std::string read_string_value(const nlohmann::json &json, const std::string &key, std::string defaultValue) {
    return read_value_internal<std::string>(json, key, std::move(defaultValue));
}
};  // namespace arenaforge::json

#endif /* JSONSerializeHelper_h_ArenaForge */
