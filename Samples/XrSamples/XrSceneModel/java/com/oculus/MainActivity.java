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
package com.oculus;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.content.Intent;

public class MainActivity extends android.app.Activity {
  private static final String PERMISSION_USE_SCENE = "com.oculus.permission.USE_SCENE";
  private static final int REQUEST_CODE_PERMISSION_USE_SCENE = 1;
  public static final String TAG = "XrSceneModel";

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
    } else {
      Log.d(TAG, "Permission has been granted " + PERMISSION_USE_SCENE);
      startNativeActivity();
    }
  }

  private void startNativeActivity() {
    Log.d(TAG, "Starting native activity");

    Intent intent = new Intent(getApplicationContext(), MainNativeActivity.class);
    startActivity(intent);
    finish();
  }

  @Override
  public void onRequestPermissionsResult(
      int requestCode, String permissions[], int[] grantResults) {
    Log.d(TAG, "onRequestPermissionsResult() called");
    if (requestCode == REQUEST_CODE_PERMISSION_USE_SCENE) {
      String permission = permissions[0];
      if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        Log.d(TAG, "Permission granted " + permission);
      } else {
        Log.d(TAG, "Permission denied " + permission);
      }
      startNativeActivity();
    }
  }
}
