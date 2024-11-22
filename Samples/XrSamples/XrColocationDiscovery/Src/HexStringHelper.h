/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <string>

#include <openxr/openxr.h>

class HexStringHelper {
   public:
    static bool HexStringToUuid(const std::string& hexStr, XrUuidEXT& uuid) {
        if (hexStr.size() != 36) {
            return false;
        }
        if (hexStr[8] != '-' || hexStr[13] != '-' || hexStr[18] != '-' || hexStr[23] != '-') {
            return false;
        }
        for (int i = 0; i < 4; i++) {
            if (!ParseByte(hexStr, (2 * i), uuid.data[i])) {
                return false;
            }
        }
        for (int i = 4; i < 6; i++) {
            if (!ParseByte(hexStr, (2 * i) + 1, uuid.data[i])) {
                return false;
            }
        }
        for (int i = 6; i < 8; i++) {
            if (!ParseByte(hexStr, (2 * i) + 2, uuid.data[i])) {
                return false;
            }
        }
        for (int i = 8; i < 10; ++i) {
            if (!ParseByte(hexStr, (2 * i) + 3, uuid.data[i])) {
                return false;
            }
        }
        for (int i = 10; i < 16; ++i) {
            if (!ParseByte(hexStr, (2 * i) + 4, uuid.data[i])) {
                return false;
            }
        }
        return true;
    }

    static std::string UuidToHexString(const XrUuidEXT& uuid) {
        std::string output;
        output.reserve(36);
        for (int i = 0; i < 16; i++) {
            AppendByte(uuid.data[i], output);
            if (i == 3 || i == 5 || i == 7 || i == 9) {
                output += '-';
            }
        }
        return output;
    }

   private:
    inline static bool ParseDigit(std::uint8_t digit, std::uint8_t& dst) {
        if (digit >= '0' && digit <= '9') {
            dst = digit - '0';
            return true;
        } else if (digit >= 'a' && digit <= 'f') {
            dst = digit - 'a' + 10;
            return true;
        } else if (digit >= 'A' && digit <= 'F') {
            dst = digit - 'A' + 10;
            return true;
        } else {
            return false;
        }
    }

    inline static bool ParseByte(const std::string& str, int pos, std::uint8_t& dst) {
        std::uint8_t hi;
        std::uint8_t lo;
        if (!ParseDigit(str[pos + 0], hi)) {
            return false;
        }
        if (!ParseDigit(str[pos + 1], lo)) {
            return false;
        }
        dst = (hi << 4) | lo;
        return true;
    }

    inline static void AppendDigit(std::uint8_t byte, std::string& str) {
        if (byte < 10) {
            str += static_cast<uint8_t>('0' + byte);
        } else {
            str += static_cast<uint8_t>('a' + (byte - 10));
        }
    }

    inline static void AppendByte(std::uint8_t byte, std::string& str) {
        AppendDigit((byte >> 4) & 0x0f, str);
        AppendDigit((byte >> 0) & 0x0f, str);
    }
};
