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
//  Color.cpp
//  ArenaForge
//
//  Created by king on 2025/9/17.
//

#include <arenaforge_core/Color.h>

#include <arenaforge_core/Log.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace arenaforge {
const Color &Color::Transparent() {
    static const Color color = {0.0f, 0.0f, 0.0f, 0.0f};
    return color;
}

const Color &Color::Black() {
    static const Color color = {0.0f, 0.0f, 0.0f, 1.0f};
    return color;
}

const Color &Color::White() {
    static const Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    return color;
}

const Color &Color::Red() {
    static const Color color = {1.0f, 0.0f, 0.0f, 1.0f};
    return color;
}

const Color &Color::Green() {
    static const Color color = {0.0f, 1.0f, 0.0f, 1.0f};
    return color;
}

const Color &Color::Blue() {
    static const Color color = {0.0f, 0.0f, 1.0f, 1.0f};
    return color;
}

Color Color::FromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    auto alpha = a == 255 ? 1.0f : static_cast<float>(a) / 255.0f;
    return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f, alpha};
}

Color Color::FromHex(const std::string &hex) {

    std::string h = hex;
    if (!h.empty() && h[0] == '#') {
        h = h.substr(1);
    }

    if (h.size() != 6 && h.size() != 8) {
        throw std::invalid_argument("Hex string must be 6 (RGB) or 8 (ARGB) characters long");
    }

    uint32_t value = 0;
    std::stringstream ss;
    ss << std::hex << h;
    ss >> value;

    if (h.size() == 6) {
        // RGB
        auto r = static_cast<uint8_t>((value >> 16) & 0xFF);
        auto g = static_cast<uint8_t>((value >> 8) & 0xFF);
        auto b = static_cast<uint8_t>(value & 0xFF);
        return FromRGBA(r, g, b);
    }

    // ARGB
    auto a = static_cast<uint8_t>((value >> 24) & 0xFF);
    auto r = static_cast<uint8_t>((value >> 16) & 0xFF);
    auto g = static_cast<uint8_t>((value >> 8) & 0xFF);
    auto b = static_cast<uint8_t>(value & 0xFF);
    return FromRGBA(r, g, b, a);
}

float Color::operator[](int index) const {
    DEBUG_ASSERT(index >= 0 && index < 4);
    return (&red)[index];
}

float &Color::operator[](int index) {
    DEBUG_ASSERT(index >= 0 && index < 4);
    return (&red)[index];
}

bool Color::isValid() const {
    return red >= 0.0f && red <= 1.0f && green >= 0.0f && green <= 1.0f && blue >= 0.0f &&
        blue <= 1.0f && alpha >= 0.0f && alpha <= 1.0f;
}

bool Color::isOpaque() const {
    DEBUG_ASSERT(alpha <= 1.0f && alpha >= 0.0f);
    return alpha == 1.0f;
}

Color Color::unpremultiply() const {
    if (alpha == 0.0f) {
        return {0, 0, 0, 0};
    } else {
        float invAlpha = 1 / alpha;
        return {red * invAlpha, green * invAlpha, blue * invAlpha, alpha};
    }
}

std::string Color::hex(bool includeAlpha) const {

    std::ostringstream oss;
    oss << '#';
    if (includeAlpha) {
        oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(alpha * 255.0f);
    }
    oss << std::uppercase << std::hex
        << std::setw(2) << std::setfill('0') << static_cast<int>(red * 255.0f)
        << std::setw(2) << std::setfill('0') << static_cast<int>(green * 255.0f)
        << std::setw(2) << std::setfill('0') << static_cast<int>(blue * 255.0f);
    return oss.str();
}
};  // namespace arenaforge
