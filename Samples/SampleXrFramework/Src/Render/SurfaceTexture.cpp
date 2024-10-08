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

Filename    :   SurfaceTexture.cpp
Content     :   Interface to Android SurfaceTexture objects
Created     :   September 17, 2013
Authors     :   John Carmack

*************************************************************************************/

#include "SurfaceTexture.h"

#include <stdlib.h>

#include "Misc/Log.h"
#include "Egl.h"
#include "GlTexture.h"

namespace OVRFW {

SurfaceTexture::SurfaceTexture(JNIEnv* jni_)
    : textureId(0),
      javaObject(NULL),
      jni(NULL),
      nanoTimeStamp(0)
#if defined(OVR_OS_ANDROID)
      ,
      updateTexImageMethodId(NULL),
      getTimestampMethodId(NULL),
      setDefaultBufferSizeMethodId(NULL)
#endif // defined(OVR_OS_ANDROID)
{
    jni = jni_;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, GetTextureId());
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

#if defined(OVR_OS_ANDROID)
    static const char* className = "android/graphics/SurfaceTexture";
    JavaClass surfaceTextureClass(jni, jni->FindClass(className));
    if (0 == surfaceTextureClass.GetJClass()) {
        ALOGE_FAIL("FindClass( %s ) failed", className);
    }

    // find the constructor that takes an int
    const jmethodID constructor =
        jni->GetMethodID(surfaceTextureClass.GetJClass(), "<init>", "(I)V");
    if (constructor == 0) {
        ALOGE_FAIL("GetMethodID( <init> ) failed");
    }
    updateTexImageMethodId =
        jni->GetMethodID(surfaceTextureClass.GetJClass(), "updateTexImage", "()V");
    if (updateTexImageMethodId == 0) {
        ALOGE_FAIL("couldn't get updateTexImageMethodId");
    }
    getTimestampMethodId = jni->GetMethodID(surfaceTextureClass.GetJClass(), "getTimestamp", "()J");
    if (getTimestampMethodId == 0) {
        ALOGE_FAIL("couldn't get getTimestampMethodId");
    }
    setDefaultBufferSizeMethodId =
        jni->GetMethodID(surfaceTextureClass.GetJClass(), "setDefaultBufferSize", "(II)V");
    if (setDefaultBufferSizeMethodId == 0) {
        ALOGE_FAIL("couldn't get setDefaultBufferSize");
    }

    JavaObject obj(
        jni, jni->NewObject(surfaceTextureClass.GetJClass(), constructor, GetTextureId()));
    if (obj.GetJObject() == 0) {
        ALOGE_FAIL("NewObject() failed");
    }

    /// Keep globar ref around
    javaObject = jni->NewGlobalRef(obj.GetJObject());
    if (javaObject == 0) {
        ALOGE_FAIL("NewGlobalRef() failed");
    }
#endif // defined(OVR_OS_ANDROID)
}

SurfaceTexture::~SurfaceTexture() {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
#if defined(OVR_OS_ANDROID)
    if (javaObject) {
        jni->DeleteGlobalRef(javaObject);
        javaObject = 0;
    }
#endif // defined(OVR_OS_ANDROID)
}

void SurfaceTexture::SetDefaultBufferSize(const int width, const int height) {
#if defined(OVR_OS_ANDROID)
    jni->CallVoidMethod(javaObject, setDefaultBufferSizeMethodId, width, height);
#endif // defined(OVR_OS_ANDROID)
}

void SurfaceTexture::Update() {
#if defined(OVR_OS_ANDROID)
    // latch the latest movie frame to the texture
    if (!javaObject) {
        return;
    }

    jni->CallVoidMethod(javaObject, updateTexImageMethodId);
    nanoTimeStamp = jni->CallLongMethod(javaObject, getTimestampMethodId);
#endif // defined(OVR_OS_ANDROID)
}

unsigned int SurfaceTexture::GetTextureId() {
    return textureId;
}

jobject SurfaceTexture::GetJavaObject() {
    return javaObject;
}

long long SurfaceTexture::GetNanoTimeStamp() {
    return nanoTimeStamp;
}

} // namespace OVRFW
