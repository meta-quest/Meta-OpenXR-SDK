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

Filename    :   SurfaceTexture.h
Content     :   Interface to Android SurfaceTexture objects
Created     :   September 17, 2013
Authors		:	John Carmack

*************************************************************************************/

#pragma once

#include "JniUtils.h"

namespace OVRFW {

// SurfaceTextures are used to get movie frames, Camera
// previews, and Android canvas views.
//
// Note that these never have mipmaps, so you will often
// want to render them to another texture and generate mipmaps
// to avoid aliasing when drawing, unless you know it will
// always be magnified.
//
// Note that we do not get and use the TransformMatrix
// from java.  Presumably this was only necessary before
// non-power-of-two textures became ubiquitous.
class SurfaceTexture {
   public:
    SurfaceTexture(JNIEnv* jni_);
    ~SurfaceTexture();

    // For some java-side uses, you can set the size
    // of the buffer before it is used to control how
    // large it is.  Video decompression and camera preview
    // always override the size automatically.
    void SetDefaultBufferSize(const int width, const int height);

    // This can only be called with an active GL context.
    // As a side effect, the textureId will be bound to the
    // GL_TEXTURE_EXTERNAL_OES target of the currently active
    // texture unit.
    void Update();

    unsigned int GetTextureId();
    jobject GetJavaObject();
    long long GetNanoTimeStamp();

   private:
    unsigned int textureId;
    jobject javaObject;
    JNIEnv* jni;

    // Updated when Update() is called, can be used to
    // check if a new frame is available and ready
    // to be processed / mipmapped by other code.
    long long nanoTimeStamp;

#if defined(OVR_OS_ANDROID)
    jmethodID updateTexImageMethodId;
    jmethodID getTimestampMethodId;
    jmethodID setDefaultBufferSizeMethodId;
#endif // defined(OVR_OS_ANDROID)
};

} // namespace OVRFW
