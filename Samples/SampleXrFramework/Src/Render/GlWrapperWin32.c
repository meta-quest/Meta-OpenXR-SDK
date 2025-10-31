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
/*
Copyright (c) 2016 Oculus VR, LLC.
*/

// This file is a cut down version of gfxwrapper_opengl.c from the OpenXR SDK.
// It is originally based on the Oculus multiplatform GL wrapper but all of the
// code for platforms other than WIN32 has been removed.

#include "GlWrapperWin32.h"

#if defined(WIN32)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h> // for memset
#include <errno.h> // for EBUSY, ETIMEDOUT etc.
#include <ctype.h> // for isspace, isdigit

#define APPLICATION_NAME "OpenGL SI"
#define WINDOW_TITLE "OpenGL SI"

#include "GL/Glu.h"
#pragma comment(lib, "glu32.lib")

static void Error(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, 4096, _TRUNCATE, format, args);
    va_end(args);

    OutputDebugStringA(buffer);

    // MessageBoxA(NULL, buffer, "ERROR", MB_OK | MB_ICONINFORMATION);
    // Without exiting, the application will likely crash.
    if (format != NULL) {
        exit(1);
    }
}

/*
================================================================================================================================

OpenGL error checking.

================================================================================================================================
*/

#if !defined(NDEBUG)
#define GL(func) \
    func;        \
    GlCheckErrors(#func);
#else
#define GL(func) func;
#endif

#if !defined(NDEBUG)
#define EGL(func)                                                  \
    if (func == EGL_FALSE) {                                       \
        Error(#func " failed: %s", EglErrorString(eglGetError())); \
    }
#else
#define EGL(func)                                                  \
    if (func == EGL_FALSE) {                                       \
        Error(#func " failed: %s", EglErrorString(eglGetError())); \
    }
#endif

/*
================================
Get proc address / extensions
================================
*/

#define GetExtension(type, functionName) ((type)(void*)wglGetProcAddress(functionName))

PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLBUFFERSTORAGEPROC glBufferStorage;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
PFNGLTEXSTORAGE2DPROC glTexStorage2D;
PFNGLTEXSTORAGE3DPROC glTexStorage3D;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;

PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding;
PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
PFNGLMEMORYBARRIERPROC glMemoryBarrier;

PFNGLGENQUERIESPROC glGenQueries;
PFNGLDELETEQUERIESPROC glDeleteQueries;
PFNGLISQUERYPROC glIsQuery;
PFNGLBEGINQUERYPROC glBeginQuery;
PFNGLENDQUERYPROC glEndQuery;
PFNGLQUERYCOUNTERPROC glQueryCounter;
PFNGLGETQUERYIVPROC glGetQueryiv;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;

PFNGLFENCESYNCPROC glFenceSync;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
PFNGLDELETESYNCPROC glDeleteSync;
PFNGLISSYNCPROC glIsSync;

PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;

PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;

PFNGLBLENDCOLORPROC glBlendColor;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
PFNWGLDELAYBEFORESWAPNVPROC wglDelayBeforeSwapNV;

PFNGLDEPTHRANGEFPROC glDepthRangef;
PFNGLBLENDEQUATIONPROC glBlendEquation;
PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;

void GlBootstrapExtensions() {
    wglChoosePixelFormatARB =
        GetExtension(PFNWGLCHOOSEPIXELFORMATARBPROC, "wglChoosePixelFormatARB");
    wglCreateContextAttribsARB =
        GetExtension(PFNWGLCREATECONTEXTATTRIBSARBPROC, "wglCreateContextAttribsARB");
    wglSwapIntervalEXT = GetExtension(PFNWGLSWAPINTERVALEXTPROC, "wglSwapIntervalEXT");
    wglDelayBeforeSwapNV = GetExtension(PFNWGLDELAYBEFORESWAPNVPROC, "wglDelayBeforeSwapNV");
}

void GlInitExtensions() {
    glGenFramebuffers = GetExtension(PFNGLGENFRAMEBUFFERSPROC, "glGenFramebuffers");
    glDeleteFramebuffers = GetExtension(PFNGLDELETEFRAMEBUFFERSPROC, "glDeleteFramebuffers");
    glBindFramebuffer = GetExtension(PFNGLBINDFRAMEBUFFERPROC, "glBindFramebuffer");
    glBlitFramebuffer = GetExtension(PFNGLBLITFRAMEBUFFERPROC, "glBlitFramebuffer");
    glGenRenderbuffers = GetExtension(PFNGLGENRENDERBUFFERSPROC, "glGenRenderbuffers");
    glDeleteRenderbuffers = GetExtension(PFNGLDELETERENDERBUFFERSPROC, "glDeleteRenderbuffers");
    glBindRenderbuffer = GetExtension(PFNGLBINDRENDERBUFFERPROC, "glBindRenderbuffer");
    glIsRenderbuffer = GetExtension(PFNGLISRENDERBUFFERPROC, "glIsRenderbuffer");
    glRenderbufferStorage = GetExtension(PFNGLRENDERBUFFERSTORAGEPROC, "glRenderbufferStorage");
    glRenderbufferStorageMultisample =
        GetExtension(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC, "glRenderbufferStorageMultisample");
    glFramebufferRenderbuffer =
        GetExtension(PFNGLFRAMEBUFFERRENDERBUFFERPROC, "glFramebufferRenderbuffer");
    glFramebufferTexture2D = GetExtension(PFNGLFRAMEBUFFERTEXTURE2DPROC, "glFramebufferTexture2D");
    glFramebufferTextureLayer =
        GetExtension(PFNGLFRAMEBUFFERTEXTURELAYERPROC, "glFramebufferTextureLayer");
    glCheckFramebufferStatus =
        GetExtension(PFNGLCHECKFRAMEBUFFERSTATUSPROC, "glCheckFramebufferStatus");
    glCheckNamedFramebufferStatus =
        GetExtension(PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC, "glCheckNamedFramebufferStatus");

    glGenBuffers = GetExtension(PFNGLGENBUFFERSPROC, "glGenBuffers");
    glDeleteBuffers = GetExtension(PFNGLDELETEBUFFERSPROC, "glDeleteBuffers");
    glBindBuffer = GetExtension(PFNGLBINDBUFFERPROC, "glBindBuffer");
    glBindBufferBase = GetExtension(PFNGLBINDBUFFERBASEPROC, "glBindBufferBase");
    glBufferData = GetExtension(PFNGLBUFFERDATAPROC, "glBufferData");
    glBufferSubData = GetExtension(PFNGLBUFFERSUBDATAPROC, "glBufferSubData");
    glBufferStorage = GetExtension(PFNGLBUFFERSTORAGEPROC, "glBufferStorage");
    glMapBuffer = GetExtension(PFNGLMAPBUFFERPROC, "glMapBuffer");
    glMapBufferRange = GetExtension(PFNGLMAPBUFFERRANGEPROC, "glMapBufferRange");
    glUnmapBuffer = GetExtension(PFNGLUNMAPBUFFERPROC, "glUnmapBuffer");

    glGenVertexArrays = GetExtension(PFNGLGENVERTEXARRAYSPROC, "glGenVertexArrays");
    glDeleteVertexArrays = GetExtension(PFNGLDELETEVERTEXARRAYSPROC, "glDeleteVertexArrays");
    glBindVertexArray = GetExtension(PFNGLBINDVERTEXARRAYPROC, "glBindVertexArray");
    glVertexAttribPointer = GetExtension(PFNGLVERTEXATTRIBPOINTERPROC, "glVertexAttribPointer");
    glVertexAttribDivisor = GetExtension(PFNGLVERTEXATTRIBDIVISORPROC, "glVertexAttribDivisor");
    glDisableVertexAttribArray =
        GetExtension(PFNGLDISABLEVERTEXATTRIBARRAYPROC, "glDisableVertexAttribArray");
    glEnableVertexAttribArray =
        GetExtension(PFNGLENABLEVERTEXATTRIBARRAYPROC, "glEnableVertexAttribArray");

    glActiveTexture = GetExtension(PFNGLACTIVETEXTUREPROC, "glActiveTexture");
    glTexImage3D = GetExtension(PFNGLTEXIMAGE3DPROC, "glTexImage3D");
    glCompressedTexImage2D = GetExtension(PFNGLCOMPRESSEDTEXIMAGE2DPROC, "glCompressedTexImage2D");
    glCompressedTexImage3D = GetExtension(PFNGLCOMPRESSEDTEXIMAGE3DPROC, "glCompressedTexImage3D");
    glTexSubImage3D = GetExtension(PFNGLTEXSUBIMAGE3DPROC, "glTexSubImage3D");
    glCompressedTexSubImage2D =
        GetExtension(PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC, "glCompressedTexSubImage2D");
    glCompressedTexSubImage3D =
        GetExtension(PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC, "glCompressedTexSubImage3D");
    glTexStorage2D = GetExtension(PFNGLTEXSTORAGE2DPROC, "glTexStorage2D");
    glTexStorage3D = GetExtension(PFNGLTEXSTORAGE3DPROC, "glTexStorage3D");
    glTexImage2DMultisample =
        GetExtension(PFNGLTEXIMAGE2DMULTISAMPLEPROC, "glTexImage2DMultisample");
    glTexImage3DMultisample =
        GetExtension(PFNGLTEXIMAGE3DMULTISAMPLEPROC, "glTexImage3DMultisample");
    glTexStorage2DMultisample =
        GetExtension(PFNGLTEXSTORAGE2DMULTISAMPLEPROC, "glTexStorage2DMultisample");
    glTexStorage3DMultisample =
        GetExtension(PFNGLTEXSTORAGE3DMULTISAMPLEPROC, "glTexStorage3DMultisample");
    glGenerateMipmap = GetExtension(PFNGLGENERATEMIPMAPPROC, "glGenerateMipmap");
    glBindImageTexture = GetExtension(PFNGLBINDIMAGETEXTUREPROC, "glBindImageTexture");

    glCreateProgram = GetExtension(PFNGLCREATEPROGRAMPROC, "glCreateProgram");
    glDeleteProgram = GetExtension(PFNGLDELETEPROGRAMPROC, "glDeleteProgram");
    glCreateShader = GetExtension(PFNGLCREATESHADERPROC, "glCreateShader");
    glDeleteShader = GetExtension(PFNGLDELETESHADERPROC, "glDeleteShader");
    glShaderSource = GetExtension(PFNGLSHADERSOURCEPROC, "glShaderSource");
    glCompileShader = GetExtension(PFNGLCOMPILESHADERPROC, "glCompileShader");
    glGetShaderiv = GetExtension(PFNGLGETSHADERIVPROC, "glGetShaderiv");
    glGetShaderInfoLog = GetExtension(PFNGLGETSHADERINFOLOGPROC, "glGetShaderInfoLog");
    glUseProgram = GetExtension(PFNGLUSEPROGRAMPROC, "glUseProgram");
    glAttachShader = GetExtension(PFNGLATTACHSHADERPROC, "glAttachShader");
    glLinkProgram = GetExtension(PFNGLLINKPROGRAMPROC, "glLinkProgram");
    glGetProgramiv = GetExtension(PFNGLGETPROGRAMIVPROC, "glGetProgramiv");
    glGetProgramInfoLog = GetExtension(PFNGLGETPROGRAMINFOLOGPROC, "glGetProgramInfoLog");
    glGetAttribLocation = GetExtension(PFNGLGETATTRIBLOCATIONPROC, "glGetAttribLocation");
    glBindAttribLocation = GetExtension(PFNGLBINDATTRIBLOCATIONPROC, "glBindAttribLocation");
    glGetUniformLocation = GetExtension(PFNGLGETUNIFORMLOCATIONPROC, "glGetUniformLocation");
    glGetUniformBlockIndex = GetExtension(PFNGLGETUNIFORMBLOCKINDEXPROC, "glGetUniformBlockIndex");
    glProgramUniform1i = GetExtension(PFNGLPROGRAMUNIFORM1IPROC, "glProgramUniform1i");
    glUniform1i = GetExtension(PFNGLUNIFORM1IPROC, "glUniform1i");
    glUniform1iv = GetExtension(PFNGLUNIFORM1IVPROC, "glUniform1iv");
    glUniform2iv = GetExtension(PFNGLUNIFORM2IVPROC, "glUniform2iv");
    glUniform3iv = GetExtension(PFNGLUNIFORM3IVPROC, "glUniform3iv");
    glUniform4iv = GetExtension(PFNGLUNIFORM4IVPROC, "glUniform4iv");
    glUniform1f = GetExtension(PFNGLUNIFORM1FPROC, "glUniform1f");
    glUniform3f = GetExtension(PFNGLUNIFORM3FPROC, "glUniform3f");
    glUniform1fv = GetExtension(PFNGLUNIFORM1FVPROC, "glUniform1fv");
    glUniform2fv = GetExtension(PFNGLUNIFORM2FVPROC, "glUniform2fv");
    glUniform3fv = GetExtension(PFNGLUNIFORM3FVPROC, "glUniform3fv");
    glUniform4fv = GetExtension(PFNGLUNIFORM4FVPROC, "glUniform4fv");
    glUniformMatrix2fv = GetExtension(PFNGLUNIFORMMATRIX2FVPROC, "glUniformMatrix2fv");
    glUniformMatrix2x3fv = GetExtension(PFNGLUNIFORMMATRIX2X3FVPROC, "glUniformMatrix2x3fv");
    glUniformMatrix2x4fv = GetExtension(PFNGLUNIFORMMATRIX2X4FVPROC, "glUniformMatrix2x4fv");
    glUniformMatrix3x2fv = GetExtension(PFNGLUNIFORMMATRIX3X2FVPROC, "glUniformMatrix3x2fv");
    glUniformMatrix3fv = GetExtension(PFNGLUNIFORMMATRIX3FVPROC, "glUniformMatrix3fv");
    glUniformMatrix3x4fv = GetExtension(PFNGLUNIFORMMATRIX3X4FVPROC, "glUniformMatrix3x4fv");
    glUniformMatrix4x2fv = GetExtension(PFNGLUNIFORMMATRIX4X2FVPROC, "glUniformMatrix4x2fv");
    glUniformMatrix4x3fv = GetExtension(PFNGLUNIFORMMATRIX4X3FVPROC, "glUniformMatrix4x3fv");
    glUniformMatrix4fv = GetExtension(PFNGLUNIFORMMATRIX4FVPROC, "glUniformMatrix4fv");
    glGetProgramResourceIndex =
        GetExtension(PFNGLGETPROGRAMRESOURCEINDEXPROC, "glGetProgramResourceIndex");
    glUniformBlockBinding = GetExtension(PFNGLUNIFORMBLOCKBINDINGPROC, "glUniformBlockBinding");
    glShaderStorageBlockBinding =
        GetExtension(PFNGLSHADERSTORAGEBLOCKBINDINGPROC, "glShaderStorageBlockBinding");

    glDrawElementsInstanced =
        GetExtension(PFNGLDRAWELEMENTSINSTANCEDPROC, "glDrawElementsInstanced");
    glDispatchCompute = GetExtension(PFNGLDISPATCHCOMPUTEPROC, "glDispatchCompute");
    glMemoryBarrier = GetExtension(PFNGLMEMORYBARRIERPROC, "glMemoryBarrier");
    glGenQueries = GetExtension(PFNGLGENQUERIESPROC, "glGenQueries");
    glDeleteQueries = GetExtension(PFNGLDELETEQUERIESPROC, "glDeleteQueries");
    glIsQuery = GetExtension(PFNGLISQUERYPROC, "glIsQuery");
    glBeginQuery = GetExtension(PFNGLBEGINQUERYPROC, "glBeginQuery");
    glEndQuery = GetExtension(PFNGLENDQUERYPROC, "glEndQuery");
    glQueryCounter = GetExtension(PFNGLQUERYCOUNTERPROC, "glQueryCounter");
    glGetQueryiv = GetExtension(PFNGLGETQUERYIVPROC, "glGetQueryiv");
    glGetQueryObjectiv = GetExtension(PFNGLGETQUERYOBJECTIVPROC, "glGetQueryObjectiv");
    glGetQueryObjectuiv = GetExtension(PFNGLGETQUERYOBJECTUIVPROC, "glGetQueryObjectuiv");
    glGetQueryObjecti64v = GetExtension(PFNGLGETQUERYOBJECTI64VPROC, "glGetQueryObjecti64v");
    glGetQueryObjectui64v = GetExtension(PFNGLGETQUERYOBJECTUI64VPROC, "glGetQueryObjectui64v");

    glFenceSync = GetExtension(PFNGLFENCESYNCPROC, "glFenceSync");
    glClientWaitSync = GetExtension(PFNGLCLIENTWAITSYNCPROC, "glClientWaitSync");
    glDeleteSync = GetExtension(PFNGLDELETESYNCPROC, "glDeleteSync");
    glIsSync = GetExtension(PFNGLISSYNCPROC, "glIsSync");

    glBlendFuncSeparate = GetExtension(PFNGLBLENDFUNCSEPARATEPROC, "glBlendFuncSeparate");
    glBlendEquationSeparate =
        GetExtension(PFNGLBLENDEQUATIONSEPARATEPROC, "glBlendEquationSeparate");

    glBlendColor = GetExtension(PFNGLBLENDCOLORPROC, "glBlendColor");

    glDebugMessageControl = GetExtension(PFNGLDEBUGMESSAGECONTROLPROC, "glDebugMessageControl");
    glDebugMessageCallback = GetExtension(PFNGLDEBUGMESSAGECALLBACKPROC, "glDebugMessageCallback");

    glDepthRangef = GetExtension(PFNGLDEPTHRANGEFPROC, "glDepthRangef");
    glBlendEquation = GetExtension(PFNGLBLENDEQUATIONPROC, "glBlendEquation");
    glInvalidateFramebuffer =
        GetExtension(PFNGLINVALIDATEFRAMEBUFFERPROC, "glInvalidateFramebuffer");
}

typedef enum {
    KS_GPU_SURFACE_COLOR_FORMAT_R5G6B5,
    KS_GPU_SURFACE_COLOR_FORMAT_B5G6R5,
    KS_GPU_SURFACE_COLOR_FORMAT_R8G8B8A8,
    KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8,
    KS_GPU_SURFACE_COLOR_FORMAT_MAX
} ksGpuSurfaceColorFormat;

typedef enum {
    KS_GPU_SURFACE_DEPTH_FORMAT_NONE,
    KS_GPU_SURFACE_DEPTH_FORMAT_D16,
    KS_GPU_SURFACE_DEPTH_FORMAT_D24,
    KS_GPU_SURFACE_DEPTH_FORMAT_MAX
} ksGpuSurfaceDepthFormat;

typedef enum {
    KS_GPU_SAMPLE_COUNT_1 = 1,
    KS_GPU_SAMPLE_COUNT_2 = 2,
    KS_GPU_SAMPLE_COUNT_4 = 4,
    KS_GPU_SAMPLE_COUNT_8 = 8,
    KS_GPU_SAMPLE_COUNT_16 = 16,
    KS_GPU_SAMPLE_COUNT_32 = 32,
    KS_GPU_SAMPLE_COUNT_64 = 64,
} ksGpuSampleCount;

typedef struct {
    HDC hDC;
    HGLRC hGLRC;
} ksGpuContext;

typedef struct {
    unsigned char redBits;
    unsigned char greenBits;
    unsigned char blueBits;
    unsigned char alphaBits;
    unsigned char colorBits;
    unsigned char depthBits;
} ksGpuSurfaceBits;

typedef struct {
    ksGpuContext context;
    ksGpuSurfaceColorFormat colorFormat;
    ksGpuSurfaceDepthFormat depthFormat;
    ksGpuSampleCount sampleCount;
    int windowWidth;
    int windowHeight;

    HINSTANCE hInstance;
    HDC hDC;
    HWND hWnd;
} ksGpuWindow;

/*
================================================================================================================================

GPU Context.

================================================================================================================================
*/

ksGpuSurfaceBits ksGpuContext_BitsForSurfaceFormat(
    const ksGpuSurfaceColorFormat colorFormat,
    const ksGpuSurfaceDepthFormat depthFormat) {
    ksGpuSurfaceBits bits;
    bits.redBits =
        ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R8G8B8A8)
             ? 8
             : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8)
                    ? 8
                    : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R5G6B5)
                           ? 5
                           : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B5G6R5) ? 5 : 8))));
    bits.greenBits =
        ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R8G8B8A8)
             ? 8
             : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8)
                    ? 8
                    : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R5G6B5)
                           ? 6
                           : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B5G6R5) ? 6 : 8))));
    bits.blueBits =
        ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R8G8B8A8)
             ? 8
             : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8)
                    ? 8
                    : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R5G6B5)
                           ? 5
                           : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B5G6R5) ? 5 : 8))));
    bits.alphaBits =
        ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R8G8B8A8)
             ? 8
             : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8)
                    ? 8
                    : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_R5G6B5)
                           ? 0
                           : ((colorFormat == KS_GPU_SURFACE_COLOR_FORMAT_B5G6R5) ? 0 : 8))));
    bits.colorBits = bits.redBits + bits.greenBits + bits.blueBits + bits.alphaBits;
    bits.depthBits =
        ((depthFormat == KS_GPU_SURFACE_DEPTH_FORMAT_D16)
             ? 16
             : ((depthFormat == KS_GPU_SURFACE_DEPTH_FORMAT_D24) ? 24 : 0));
    return bits;
}

static bool ksGpuContext_CreateForSurface(
    ksGpuContext* context,
    const ksGpuSurfaceColorFormat colorFormat,
    const ksGpuSurfaceDepthFormat depthFormat,
    const ksGpuSampleCount sampleCount,
    HINSTANCE hInstance,
    HDC hDC) {
    const ksGpuSurfaceBits bits = ksGpuContext_BitsForSurfaceFormat(colorFormat, depthFormat);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1, // version
        PFD_DRAW_TO_WINDOW | // must support windowed
            PFD_SUPPORT_OPENGL | // must support OpenGL
            PFD_DOUBLEBUFFER, // must support double buffering
        PFD_TYPE_RGBA, // iPixelType
        bits.colorBits, // cColorBits
        0,
        0, // cRedBits, cRedShift
        0,
        0, // cGreenBits, cGreenShift
        0,
        0, // cBlueBits, cBlueShift
        0,
        0, // cAlphaBits, cAlphaShift
        0, // cAccumBits
        0, // cAccumRedBits
        0, // cAccumGreenBits
        0, // cAccumBlueBits
        0, // cAccumAlphaBits
        bits.depthBits, // cDepthBits
        0, // cStencilBits
        0, // cAuxBuffers
        PFD_MAIN_PLANE, // iLayerType
        0, // bReserved
        0, // dwLayerMask
        0, // dwVisibleMask
        0 // dwDamageMask
    };

    HWND localWnd = NULL;
    HDC localDC = hDC;

    if (sampleCount > KS_GPU_SAMPLE_COUNT_1) {
        // A valid OpenGL context is needed to get OpenGL extensions including
        // wglChoosePixelFormatARB and wglCreateContextAttribsARB. A device context with a valid
        // pixel format is needed to create an OpenGL context. However, once a pixel format is set
        // on a device context it is final. Therefore a pixel format is set on the device context of
        // a temporary window to create a context to get the extensions for multi-sampling.
        localWnd =
            CreateWindowA(APPLICATION_NAME, "temp", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
        localDC = GetDC(localWnd);
    }

    int pixelFormat = ChoosePixelFormat(localDC, &pfd);
    if (pixelFormat == 0) {
        Error("Failed to find a suitable pixel format.");
        return false;
    }

    if (!SetPixelFormat(localDC, pixelFormat, &pfd)) {
        Error("Failed to set the pixel format.");
        return false;
    }

    // Now that the pixel format is set, create a temporary context to get the extensions.
    {
        HGLRC hGLRC = wglCreateContext(localDC);
        wglMakeCurrent(localDC, hGLRC);

        GlBootstrapExtensions();

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hGLRC);
    }

    if (sampleCount > KS_GPU_SAMPLE_COUNT_1) {
        // Release the device context and destroy the window that were created to get extensions.
        ReleaseDC(localWnd, localDC);
        DestroyWindow(localWnd);

        int pixelFormatAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB,
            GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB,
            GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,
            GL_TRUE,
            WGL_PIXEL_TYPE_ARB,
            WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,
            bits.colorBits,
            WGL_DEPTH_BITS_ARB,
            bits.depthBits,
            WGL_SAMPLE_BUFFERS_ARB,
            1,
            WGL_SAMPLES_ARB,
            sampleCount,
            0};

        unsigned int numPixelFormats = 0;

        if (!wglChoosePixelFormatARB(
                hDC, pixelFormatAttribs, NULL, 1, &pixelFormat, &numPixelFormats) ||
            numPixelFormats == 0) {
            Error("Failed to find MSAA pixel format.");
            return false;
        }

        memset(&pfd, 0, sizeof(pfd));

        if (!DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd)) {
            Error("Failed to describe the pixel format.");
            return false;
        }

        if (!SetPixelFormat(hDC, pixelFormat, &pfd)) {
            Error("Failed to set the pixel format.");
            return false;
        }
    }

    int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        OPENGL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        OPENGL_VERSION_MINOR,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
        0};

    context->hDC = hDC;
    context->hGLRC = wglCreateContextAttribsARB(hDC, NULL, contextAttribs);
    if (!context->hGLRC) {
        Error("Failed to create GL context.");
        return false;
    }

    wglMakeCurrent(hDC, context->hGLRC);

    GlInitExtensions();

    return true;
}

void ksGpuContext_Destroy(ksGpuContext* context) {
    if (context->hGLRC) {
        if (!wglMakeCurrent(NULL, NULL)) {
            DWORD error = GetLastError();
            Error("Failed to release context error code (%d).", error);
        }

        if (!wglDeleteContext(context->hGLRC)) {
            DWORD error = GetLastError();
            Error("Failed to delete context error code (%d).", error);
        }
        context->hGLRC = NULL;
    }
    context->hDC = NULL;
}

/*
================================================================================================================================

GPU Window.

================================================================================================================================
*/

void ksGpuWindow_Destroy(ksGpuWindow* window) {
    ksGpuContext_Destroy(&window->context);

    if (window->hDC) {
        if (!ReleaseDC(window->hWnd, window->hDC)) {
            Error("Failed to release device context.");
        }
        window->hDC = NULL;
    }

    if (window->hWnd) {
        if (!DestroyWindow(window->hWnd)) {
            Error("Failed to destroy the window.");
        }
        window->hWnd = NULL;
    }

    if (window->hInstance) {
        if (!UnregisterClassA(APPLICATION_NAME, window->hInstance)) {
            Error("Failed to unregister window class.");
        }
        window->hInstance = NULL;
    }
}

static LRESULT APIENTRY WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CLOSE: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcA(hWnd, message, wParam, lParam);
}

bool ksGpuWindow_Create(
    ksGpuWindow* window,
    ksGpuSurfaceColorFormat colorFormat,
    ksGpuSurfaceDepthFormat depthFormat,
    ksGpuSampleCount sampleCount,
    int width,
    int height) {
    memset(window, 0, sizeof(ksGpuWindow));

    window->colorFormat = colorFormat;
    window->depthFormat = depthFormat;
    window->sampleCount = sampleCount;
    window->windowWidth = width;
    window->windowHeight = height;

    DEVMODEA lpDevMode;
    memset(&lpDevMode, 0, sizeof(DEVMODEA));
    lpDevMode.dmSize = sizeof(DEVMODEA);
    lpDevMode.dmDriverExtra = 0;

    window->hInstance = GetModuleHandleA(NULL);

    WNDCLASSA wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = window->hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = APPLICATION_NAME;

    if (!RegisterClassA(&wc)) {
        Error("Failed to register window class.");
        return false;
    }

    // Fixed size window.
    DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    RECT windowRect;
    windowRect.left = (long)0;
    windowRect.right = (long)width;
    windowRect.top = (long)0;
    windowRect.bottom = (long)height;

    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);

    const int offsetX = (desktopRect.right - (windowRect.right - windowRect.left)) / 2;
    const int offsetY = (desktopRect.bottom - (windowRect.bottom - windowRect.top)) / 2;

    windowRect.left += offsetX;
    windowRect.right += offsetX;
    windowRect.top += offsetY;
    windowRect.bottom += offsetY;

    window->hWnd = CreateWindowExA(
        dwExStyle, // Extended style for the window
        APPLICATION_NAME, // Class name
        WINDOW_TITLE, // Window title
        dwStyle | // Defined window style
            WS_CLIPSIBLINGS | // Required window style
            WS_CLIPCHILDREN, // Required window style
        windowRect.left, // Window X position
        windowRect.top, // Window Y position
        windowRect.right - windowRect.left, // Window width
        windowRect.bottom - windowRect.top, // Window height
        NULL, // No parent window
        NULL, // No menu
        window->hInstance, // Instance
        NULL); // No WM_CREATE parameter
    if (!window->hWnd) {
        ksGpuWindow_Destroy(window);
        Error("Failed to create window.");
        return false;
    }

    SetWindowLongPtrA(window->hWnd, GWLP_USERDATA, (LONG_PTR)window);

    window->hDC = GetDC(window->hWnd);
    if (!window->hDC) {
        ksGpuWindow_Destroy(window);
        Error("Failed to acquire device context.");
        return false;
    }

    ksGpuContext_CreateForSurface(
        &window->context, colorFormat, depthFormat, sampleCount, window->hInstance, window->hDC);

    wglMakeCurrent(window->context.hDC, window->context.hGLRC);

    ShowWindow(window->hWnd, SW_SHOW);
    SetForegroundWindow(window->hWnd);
    SetFocus(window->hWnd);

    return true;
}

ksGpuWindow window;

void ovrGl_CreateContext_Windows(HDC* hDC, HGLRC* hGLRC) {
    ksGpuSurfaceColorFormat colorFormat = KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8;
    ksGpuSurfaceDepthFormat depthFormat = KS_GPU_SURFACE_DEPTH_FORMAT_D24;
    ksGpuSampleCount sampleCount = KS_GPU_SAMPLE_COUNT_1;
    if (!ksGpuWindow_Create(&window, colorFormat, depthFormat, sampleCount, 640, 480)) {
        Error("Unable to create GL context");
    }

    *hDC = window.context.hDC;
    *hGLRC = window.context.hGLRC;
}

void ovrGl_DestroyContext_Windows(HDC* hDC, HGLRC* hGLRC) {
    ksGpuWindow_Destroy(&window);
}

const char* ovrGl_ErrorString_Windows(const GLint err) {
    return (const char*)gluErrorString(err);
}

#endif // defined(OS_WINDOWS)
