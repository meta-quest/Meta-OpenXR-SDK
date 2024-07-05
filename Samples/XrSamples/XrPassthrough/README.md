# OpenXR Passthrough Sample

## Overview
Passthrough is a method that allows users to view their physical environment through a light-blocking VR headset. The `XR_FB_passthrough` feature allows:
* An application to request that passthrough be composited with the application content.
* An application to specify the compositing and blending rules between passthrough and VR content.
* An application to apply styles, such as color mapping and edge rendering, to passthrough.
* An application to provide a geometry to be used in place of the user's physical environment. Camera images will be projected onto the surface provided by the application. In some cases where a part of the environment, such as a desk, can be approximated well, this provides a better visual experience.

## The Sample
This sample demonstrates the use of both still and animated styles, as well as selective and projected passthrough.
