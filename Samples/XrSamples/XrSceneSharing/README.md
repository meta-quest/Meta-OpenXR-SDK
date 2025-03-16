# OpenXR Scene Sharing Sample

## Overview
Scene Sharing enables developers to build synchronous and asynchronous colocated experiences where the captured scene - a list of rooms and collections of scene anchors, e.g., wall, floor, ceiling, furnishing and a global mesh, can be shared among participants to create a colocated mixed reality experience.


## The Sample
The sample demonstrates how to use Scene Sharing capabilities:
* Host client loading/visualizing their locally capture scene using XrQuerySpacesFB API ( and running Space Setup if necessary )
* Host client sharing scene data to a group using the XrShareSpacesMETA API in XR_META_spatial_entity_group_sharing extension , and saving the group uuid to a file
* Guest clients loading the groupUuid from a file and loading/visualizing the shared scene data using XrQuerySpacesFB API

### Controls
On the Host Client:
- Button A: Query the scene by first querying all the rooms (spatial entities that have the room layout component enabled), and then querying all the spatial entities in each rooms. Upon completion, render the scene for visuzliation
- Button B: Create a group uuid and share the queryed scene to the group. Upon completion, save the group uuid to a file

On the Guest Client:
- Button X: Load the group uuid from a file and query the shared scene from the group. Upon completion, render the scene for visaulization
