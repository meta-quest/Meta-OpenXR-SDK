// Copyright (c) Facebook Technologies, LLC and its affiliates. All Rights reserved.
package com.oculus;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;

public class NativeActivity extends android.app.NativeActivity {
  private static final String PERMISSION_USE_SCENE = "com.oculus.permission.USE_SCENE";
  private static final int REQUEST_CODE_PERMISSION_USE_SCENE = 1;
  private static final String TAG = "XrSceneModel";

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
