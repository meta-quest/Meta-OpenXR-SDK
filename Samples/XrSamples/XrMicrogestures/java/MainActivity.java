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
// Copyright (c) Facebook Technologies, LLC and its affiliates. All Rights reserved.
package com.oculus.sdk.xrmicrogestures;

///--BEGIN_SDK_REMOVE
import java.io.PrintWriter;
import java.io.FileDescriptor;
///--END_SDK_REMOVE

public class MainActivity extends android.app.NativeActivity {
///--BEGIN_SDK_REMOVE
  private static native String nativeGetDumpStr();

  static {
    System.loadLibrary("xrmicrogestures");
  }

  @Override
  public void dump(String prefix, FileDescriptor fd, PrintWriter writer, String[] args) {
    writer.println();
    writer.println("========== MICROGESTURES ==========");
    writer.println("Microgesture Events Begin");
    writer.println(nativeGetDumpStr());
    writer.println("Microgesture Events End");
  }
///--END_SDK_REMOVE
}
