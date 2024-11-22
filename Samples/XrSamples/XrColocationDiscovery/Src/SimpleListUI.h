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

/*******************************************************************************

Filename    :   SimpleListUI.h
Content     :
Created     :
Authors     :
Language    :   C++
Copyright:  Copyright (c) Facebook Technologies, LLC and its affiliates. All rights reserved.

*******************************************************************************/

#include <cstdint>
#include <cstdio>
#include <ctime>

#include "XrApp.h"

#include "Input/SkeletonRenderer.h"
#include "Input/ControllerRenderer.h"
#include "Input/TinyUI.h"

class SimpleListUI {
   public:
    SimpleListUI(
        OVRFW::TinyUI& ui,
        const OVR::Vector3f& position,
        const OVR::Vector2f& size,
        const size_t numRows,
        const std::string& header) {
        const float rowHeight = size[1] / numRows;
        for (size_t i = 0; i < numRows; i++) {
            Rows.push_back(ui.AddLabel(
                "",
                OVR::Vector3f(
                    position[0],
                    position[1] + ((numRows - i - 1) * rowHeight / 1000.0),
                    position[2]),
                OVR::Vector2f(size[0], rowHeight / 2.0)));
        }
        Header = ui.AddLabel(
            header,
            OVR::Vector3f(position[0], position[1] + (numRows * rowHeight / 1000.0), position[2]),
            OVR::Vector2f(size[0], rowHeight / 2.0));
    }

    void SetHeader(const std::string& header) {
        Header->SetText(header.c_str());
    }

    void AppendRow(const std::string& text) {
        ALOGV("Appending row to list: %s", text.c_str());
        if (NumRowsFilled == Rows.size()) {
            for (size_t i = 0; i < Rows.size() - 1; i++) {
                Rows[i]->SetText(Rows[i + 1]->GetText().c_str());
            }
        } else {
            NumRowsFilled++;
        }
        Rows[NumRowsFilled - 1]->SetText(text.c_str());
    }

    void Clear() {
        for (auto& row : Rows) {
            row->SetText("");
        }
        NumRowsFilled = 0;
    }

    void SetVisible(bool visible = false) {
        Header->SetVisible(visible);
        for (auto& row : Rows) {
            row->SetVisible(visible);
        }
    }

   private:
    std::vector<OVRFW::VRMenuObject*> Rows;
    size_t NumRowsFilled = 0;
    OVRFW::VRMenuObject* Header;
};
