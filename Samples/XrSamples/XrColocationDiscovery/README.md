# OpenXR Colocation Discovery Sample

## Overview
Colocation Discovery allows apps to discover physically colocated devices running the same app.

The `XR_META_colocation_discovery` extension allows an app to:
* Start/stop advertising a unique colocation advertisement UUID + an optional buffer of metadata
* Start/stop discoverying nearby colocation advertisements started by the same app.

## The Sample
The sample shows a simple workflow of how `XR_META_colocation_discovery` can be used to discovery nearby devices. Additionally, the sample shows how this can be used with the `XR_META_spatial_entity_group_sharing` extension to share a Spatial Anchor with these nearby devices to create common reference frames.
