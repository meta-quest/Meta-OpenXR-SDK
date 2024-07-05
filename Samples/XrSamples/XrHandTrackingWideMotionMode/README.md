# OpenXR Hands Tracking Wide Motion Mode Sample

## Overview
The `XR_META_hand_tracking_wide_motion_model` augments the `XR_EXT_hand_tracking` extension.

This extension allows the application to request that the hand tracking data provided via the runtime go beyond normal performance considerations in order to provide poses outside of the platform's normal tracking area. This may require significantly more processing power then normally used by the runtime and may not result in pose changes if the system is not capable of leveraging additional resources.

## The Sample
Wide Motion Mode is a feature for hand tracking that can approximately position hands based on what the cameras can see of your arms if the hands themselves are hidden. The user should be able to obscure their hands (e.g. under a desk) and the device will approximately position the virtual hands in the right place. Note: It won't be able to detect gestures.
