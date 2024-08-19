# OpenXR Scene Model Sample

## Overview
Scene empowers you to quickly build complex and scene-aware experiences with rich interactions in the user's physical environment. Combined with Passthrough and Spatial Anchors, Scene capabilities enable you to build Mixed Reality experiences and create new possibilities for social connections, entertainment, productivity, and more.

Scene is enabled by a set of extensions that expands on the concept of spatial entities to include methods for spatial entities to represent rooms, planes, objects, and meshes. Each of the geometric and semantic properties of spatial entities is represented as a component of the Entity Component System. The components and the functions for accessing them are defined in the following extensions:
- `XR_FB_scene`
- `XR_FB_spatial_entity_container`
- `XR_META_spatial_entity_mesh`

Meta Quest operating system manages and persists the spatial entities of the user's physical environment as the Scene Model, via the process of Space Setup. You can request the operating system to invoke Space Setup using the `XR_FB_scene_capture` extension.

## The Sample
The sample demonstrates how to use Scene capabilities: (1) how to request Space Setup to the operating system and (2) how to use several different queries to selectively load the Scene Model persisted in the system and visualize them. The sample also uses a relevant capability of toggling boundary visibility between suppressed and not suppressed (defined in the `XR_META_boundary_visibility` extension), so that the user can move outside of the boundaries while observing the Scene Model in the Mixed Reality environment.

### Controls
- Right Index Trigger: Request the system to invoke Space Setup by using `xrRequestSceneCaptureFB`.
- Right Grip: Request the system to toggle boundary visibility between suppressed and not suppressed by using `xrRequestBoundaryVisibilityMETA`.
- Right Thumb Click: Toggle Passthrough rendering between on and off.
- Button A: Load the Scene Model by first querying all the rooms (spatial entities that have the room layout component enabled), and then querying all the spatial entities defined in each room.
- Button B: Load only planes in the Scene Model by querying spatial entities that have the bounded 2D component enabled.
- Button X: Load the Scene Model by querying all spatial entities.
- Button Y: Load only floors/ceilings/walls by first querying all the rooms (spatial entities that have the room layout component enabled), and then querying only the spatial entities corresponding to the floors/ceilings/walls.
