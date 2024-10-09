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
/************************************************************************************

Filename    :   PointList.cpp
Content     :   Abstract base class for a list of points.
Created     :   6/16/2017
Authors     :   Jonathan E. Wright

************************************************************************************/

#include "PointList.h"
#include <assert.h>

using OVR::Vector3f;

namespace OVRFW {

//==============================================================================================
// ovrPointList_Vector

ovrPointList_Vector::ovrPointList_Vector(const int maxPoints) : MaxPoints(maxPoints) {}

int ovrPointList_Vector::GetNext(const int cur) const {
    int next = cur + 1;
    int arraySize = (int)Points.size();
    if (next >= arraySize) {
        return -1;
    }
    return next;
}

void ovrPointList_Vector::RemoveHead() {
    for (int i = 1; i < (int)Points.size(); ++i) {
        Points[i - 1] = Points[i];
    }
    Points.resize(Points.size() - 1);
}

//==============================================================================================
// ovrPointList_Circular

ovrPointList_Circular::ovrPointList_Circular(const int bufferSize)
    : BufferSize(bufferSize), CurPoints(0), HeadIndex(0), TailIndex(0) {
    Points.resize(BufferSize);
}

int ovrPointList_Circular::GetFirst() const {
    if (IsEmpty()) {
        assert(!IsEmpty());
        return -1;
    }
    return HeadIndex;
}

int ovrPointList_Circular::GetNext(const int cur) const {
    if (IsEmpty()) {
        assert(!IsEmpty());
        return -1;
    }
    int next = IncIndex(cur);
    if (next == TailIndex) {
        return -1;
    }
    return next;
}

int ovrPointList_Circular::GetLast() const {
    if (IsEmpty()) {
        return -1;
    }
    const int lastIndex = DecIndex(TailIndex);
    return lastIndex;
}

void ovrPointList_Circular::AddToTail(const Vector3f& p) {
    if (!IsFull()) {
        CurPoints++;
        Points[TailIndex] = p;
        TailIndex = IncIndex(TailIndex);
    }
}
void ovrPointList_Circular::RemoveHead() {
    if (HeadIndex == TailIndex) {
        return;
    }
    HeadIndex = IncIndex(HeadIndex);
    CurPoints--;
}

int ovrPointList_Circular::DecIndex(const int in) const {
    int d = in - 1;
    if (d < 0) {
        d += BufferSize;
    }
    return d;
}

} // namespace OVRFW
