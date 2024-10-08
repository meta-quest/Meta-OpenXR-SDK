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

Filename    :   PointList.h
Content     :   Abstract base class for a list of points.
Created     :   6/16/2017
Authors     :   Jonathan E. Wright

************************************************************************************/

#pragma once

#include <cstdint>
#include <vector>
#include "OVR_Math.h"

namespace OVRFW {

//==============================================================
// ovrPointList
class ovrPointList {
   public:
    virtual ~ovrPointList() {}

    virtual bool IsEmpty() const = 0;
    virtual bool IsFull() const = 0;
    virtual int GetFirst() const = 0;
    virtual int GetNext(const int cur) const = 0;
    virtual int GetLast() const = 0;
    virtual int GetCurPoints() const = 0;
    virtual int GetMaxPoints() const = 0;

    virtual const OVR::Vector3f& Get(const int index) const = 0;
    virtual OVR::Vector3f& Get(const int index) = 0;

    virtual void AddToTail(const OVR::Vector3f& p) = 0;
    virtual void RemoveHead() = 0;
};

//==============================================================
// ovrPointList_Vector
class ovrPointList_Vector : public ovrPointList {
   public:
    ovrPointList_Vector(const int maxPoints);

    virtual bool IsEmpty() const override {
        return Points.size() == 0;
    }
    virtual bool IsFull() const override {
        return false;
    }
    virtual int GetFirst() const override {
        return 0;
    }
    virtual int GetNext(const int cur) const override;
    virtual int GetLast() const override {
        return (int)Points.size() - 1;
    }
    virtual int GetCurPoints() const override {
        return Points.size();
    }
    virtual int GetMaxPoints() const override {
        return MaxPoints;
    }

    virtual const OVR::Vector3f& Get(const int index) const override {
        return Points[index];
    }
    virtual OVR::Vector3f& Get(const int index) override {
        return Points[index];
    }

    virtual void AddToTail(const OVR::Vector3f& p) override {
        Points.push_back(p);
    }
    virtual void RemoveHead() override;

   private:
    std::vector<OVR::Vector3f> Points;
    int MaxPoints;
};

//==============================================================
// ovrPointList_Circular
class ovrPointList_Circular : public ovrPointList {
   public:
    ovrPointList_Circular(const int bufferSize);

    virtual bool IsEmpty() const override {
        return HeadIndex == TailIndex;
    }
    virtual bool IsFull() const override {
        return IncIndex(TailIndex) == HeadIndex;
    }
    virtual int GetFirst() const override;
    virtual int GetNext(const int cur) const override;
    virtual int GetLast() const override;
    virtual int GetCurPoints() const override {
        return CurPoints;
    }
    virtual int GetMaxPoints() const override {
        return BufferSize - 1;
    }

    virtual const OVR::Vector3f& Get(const int index) const override {
        return Points[index];
    }
    virtual OVR::Vector3f& Get(const int index) override {
        return Points[index];
    }

    virtual void AddToTail(const OVR::Vector3f& p) override;
    virtual void RemoveHead() override;

   private:
    std::vector<OVR::Vector3f> Points;

    int BufferSize;
    int CurPoints;
    int HeadIndex;
    int TailIndex;

   private:
    int IncIndex(const int in) const {
        return (in + 1) % BufferSize;
    }
    int DecIndex(const int in) const;
};

} // namespace OVRFW
