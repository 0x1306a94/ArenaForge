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
//  UUID.cpp
//  arenaforge_core
//
//  Created by KK on 2025/9/6.
//

#include <arenaforge_core/uuid/UUID.h>

#if __APPLE__
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFUUID.h>
#endif

namespace arenaforge {

#if __APPLE__
class UUID::UUIDImpl {
  public:
    UUIDImpl() {
    }

    ~UUIDImpl() {
    }

    std::string generate() {
        auto uuidRef = CFUUIDCreate(kCFAllocatorDefault);
        if (uuidRef == nullptr) {
            return "";
        }

        auto stringRef = CFUUIDCreateString(kCFAllocatorDefault, uuidRef);
        if (stringRef == nullptr) {
            CFRelease(uuidRef);
            return "";
        }

        const char *cStr = CFStringGetCStringPtr(stringRef, kCFStringEncodingUTF8);
        if (cStr != nullptr) {
            auto copied = std::string(cStr, strlen(cStr));
            // fast path
            CFRelease(uuidRef);
            CFRelease(stringRef);
            return copied;
        }

        std::string str(static_cast<size_t>(CFStringGetLength(stringRef)) + 1, '\0');
        if (!CFStringGetCString(stringRef, &str.front(), static_cast<CFIndex>(str.size()), kCFStringEncodingUTF8)) {
            CFRelease(uuidRef);
            CFRelease(stringRef);
            return "";
        }

        str.pop_back();
        CFRelease(uuidRef);
        CFRelease(stringRef);
        return str;
    }
};

#endif

UUID::UUID()
    : _impl(std::make_shared<UUIDImpl>()) {
}

std::string UUID::generate() const {
    return _impl->generate();
}

std::string UUID::operator()() const {
    return generate();
}

};  // namespace arenaforge
