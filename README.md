# Meta OpenXR SDK / Samples

## Introduction
Welcome to the Meta OpenXR SDK project, a native OpenXR SDK and samples specifically designed for Meta Quest. This SDK is a comprehensive resource that includes header files and API definitions for both experimental and pre-release OpenXR APIs.

As these API definitions reach their final stages, they will be incorporated into the [OpenXR SDK](https://github.com/KhronosGroup/OpenXR-SDK). To help you understand and utilize these features, we've included a variety of samples. These samples serve as practical demonstrations, showing you how to integrate and use the APIs effectively.

## What are the samples?

|Sample Name |OpenXR features / extensions shown |Target devices| Extra notes
|--|--|--|--|
|[`XrBodyFaceEyeSocial`](Samples/XrSamples/XrBodyFaceEyeSocial/) |`XR_FB_body_tracking`, `XR_FB_eye_tracking_social`, `XR_FB_face_tracking` |Meta Quest Pro
|[`XrColocationDiscovery`](Samples/XrSamples/XrColocationDiscovery/) |`XR_META_colocation_discovery`, `XR_META_spatial_entity_group_sharing`, and `XR_META_spatial_entity_sharing`  |Meta Quest 2 and later devices
|[`XrColorSpaceFB`](Samples/XrSamples/XrColorSpaceFB/)               |`XR_FB_color_space` |All Meta Quest devices
|[`XrCompositor_NativeActivity`](Samples/XrSamples/XrCompositor_NativeActivity/)   |`XR_KHR_composition_layer_cube`, `XR_KHR_composition_layer_cylinder`, `XR_KHR_composition_layer_equirect2`, `XR_FB_foveation` |All Meta Quest devices |Single file `C` sample
|[`XrControllers`](Samples/XrSamples/XrControllers/)                 |`XR_FB_haptic_amplitude_envelope`, `XR_FB_haptic_pcm`|Meta Quest 2 and later devices
|[`XrDynamicObjects`](Samples/XrSamples/XrDynamicObjects/)                 |`XR_META_dynamic_object_tracker`|Meta Quest 3 and later devices
|[`XrHandDataSource`](Samples/XrSamples/XrHandDataSource/)              |`XR_EXT_hand_tracking_data_source`|Meta Quest 2 and later devices
|[`XrHandsAndControllers`](Samples/XrSamples/XrHandsAndControllers/)         |`XR_META_detached_controllers`,`XR_META_simultaneous_hands_and_controllers`|Meta Quest 3 and later|
|[`XrHandsFB`](Samples/XrSamples/XrHandsFB/)                     |`XR_FB_hand_tracking_mesh`, `XR_FB_hand_tracking_capsules`,`XR_FB_hand_tracking_aim`|All Meta Quest devices|
|[`XrHandTrackingWideMotionMode`](Samples/XrSamples/XrHandTrackingWideMotionMode/)      |`XR_META_hand_tracking_wide_motion_mode`|Meta Quest 3 and later|
|[`XrMicrogestures`](Samples/XrSamples/XrMicrogestures/)      |`XR_META_hand_tracking_microgestures`|Meta Quest 2 and later devices|
|[`XrInput`](Samples/XrSamples/XrInput/)              |OpenXR Action System|All Meta Quest devices|
|[`XrPassthrough`](Samples/XrSamples/XrPassthrough/)                |`XR_FB_passthrough`|All Meta Quest devices|Demonstrates the use of still and animated styles, selective and projected passthrough.
|[`XrPassthroughOcclusion`](Samples/XrSamples/XrPassthroughOcclusion/)        |`XR_META_envionment_depth`|Meta Quest 3 and later|
|[`XrSceneModel`](Samples/XrSamples/XrSceneModel/)                 |`XR_FB_scene_capture`, `XR_FB_scene`, `XR_FB_spatial_entity`, `XR_FB_spatial_entity_query`, `XR_FB_spatial_entity_container`, `XR_META_spatial_entity_mesh`, `XR_META_boundary_visibility`|Meta Quest 2 and later|Demonstrates a scene-aware experience including floor, walls, and furniture.
|[`XrSceneSharing`](Samples/XrSamples/XrSceneSharing/)                 |`XR_META_spatial_entity_group_sharing`, `XR_META_spatial_entity_sharing`, `XR_FB_spatial_entity`, `XR_FB_scene`|Meta Quest 2 and later|Demonstrates a scene-sharing experience including sharing captured room, floor, walls, and furniture.
|[`XrSpaceWarp`](Samples/XrSamples/XrSpaceWarp/)                   |`XR_FB_space_warp`|Meta Quest 2 and later|
|[`XrSpatialAnchor`](Samples/XrSamples/XrSpatialAnchor/)              |`XR_FB_spatial_entity`, `XR_FB_spatial_entity_query`, `XR_FB_spatial_entity_storage`, `XR_FB_spatial_entity_storage_batch`, `XR_FB_spatial_entity_sharing`, `XR_FB_spatial_entity_user`|Meta Quest 2 and later|
|[`XrVirtualKeyboard`](Samples/XrSamples/XrVirtualKeyboard/) |`XR_META_virtual_keyboard`,`XR_FB_render_model`|Meta Quest 2 and later|

## Build
> Enable Quest system property to use experimental features, you will need the command: `adb shell setprop debug.oculus.experimentalEnabled 1`.

> **Note**: This value resets with each headset reboot.
### Android
#### Dependencies
* Android Studio
  * The lastest Android Studio release is recommended.
  * If you have agreed with the licenses previously on your development system, Android Studio will automatically install, at the start up time. Otherwise (or if the installation failed), you need to install the required CMake and NDK manually, refer to the [official instructions](https://developer.android.com/studio/projects/install-ndk) for the detailed steps. The default installed locations are:
    * `$SDK-ROOT-DIR/ndk/$ndkVersion` for NDK.
    * `$SDK-ROOT-DIR/cmake/$cmake-version` for CMake.
#### Build with CMake on Android Studio
1. If this is the first time you are launching Android Studio, select Open an existing Android Studio project. If you have launched Android Studio before, click File > Open instead.
2. Open build.gradle under Samples folder
   * You could also open individual sample app from the Samples/XrSamples folders. For example, Samples/XrSamples/XrHandsFB/Projects/Android/build.gradle
3. After the build has finished, directly run a sample with the device connected. Click Run in the toolbar.

### Meta Quest Link
While the samples provided in this package are designed as Quest Native / Android samples, some of them can be built and run on Windows for Meta Quest Link. For building with Meta Quest Link, you'll need:
* [cmake](https://cmake.org/download/)
* A C++ development environment compatible with cmake (such as Microsoft Visual Studio 2022)
* [Meta Quest Link application](https://www.meta.com/quest/setup/)

#### Notes for running samples with Meta Quest Link
* Ensure that Developer Runtime Features is enabled in the Meta Quest Link application.
* Make sure the headset is on, the Meta Quest Link application is running and Meta Quest Link is started; before double-click and launch the sample.

## More details

- [https://developer.oculus.com/downloads/package/oculus-openxr-mobile-sdk/](https://developer.oculus.com/downloads/package/oculus-openxr-mobile-sdk/)
- [https://developer.oculus.com/documentation/native/android/mobile-openxr-sample/](https://developer.oculus.com/documentation/native/android/mobile-openxr-sample/)

See the [CONTRIBUTING](CONTRIBUTING.md) file for how to help out.

## License
Meta OpenXR SDK is subject to the Oculus SDK License Agreement, as found in the LICENSE file.
