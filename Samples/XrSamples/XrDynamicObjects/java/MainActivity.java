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
// (c) Meta Platforms, Inc. and affiliates.
package com.oculus.sdk.xrdynamicobjects;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends android.app.NativeActivity {
  private static final String PERMISSION_USE_SCENE = "com.oculus.permission.USE_SCENE";
  private static final int REQUEST_CODE_PERMISSION_USE_SCENE = 1;
  private static final String TAG = "XrDynamicObjects";

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    requestScenePermissionIfNeeded();
  }

  private void requestScenePermissionIfNeeded() {
    Log.d(TAG, "requestScenePermissionIfNeeded");
    if (checkSelfPermission(PERMISSION_USE_SCENE) != PackageManager.PERMISSION_GRANTED) {
      Log.d(TAG, "Permission has not been granted, request " + PERMISSION_USE_SCENE);
      requestPermissions(
          new String[] {PERMISSION_USE_SCENE}, REQUEST_CODE_PERMISSION_USE_SCENE);
    }
  }
}
