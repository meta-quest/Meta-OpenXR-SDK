# OpenXR Spatial Anchors Sample

## Overview
Spatial entities are application-defined content that persist in real-world locations.

* `XR_FB_spatial_entity`: This extension enables applications to persist the real-world location of content over time and contains definitions for the Entity-Component System. All Facebook spatial entities and scene extensions are dependent on this one.
* `XR_FB_spatial_entity_query`: This extension enables an application to query for shared spatial entities in the area.
* `XR_META_spatial_entity_discovery`: This extension enables an application to discover persistent spatial entities in the area and restore them.
* `XR_META_spatial_entity_persistence`: This extension enables spatial entities to be persisted such that they're retrievable across sessions and sharable.
* `XR_FB_spatial_entity_sharing`: This extension enables spatial entities to be shared between users.
* `XR_FB_spatial_entity_user`: This extension enables the creation and management of user objects which can be used by the application to reference a user other than the current user.

## The Sample
In this sample, the orange cuboid is an object rendered with respect to Spatial Anchors that are tracked in the scene. The user is able to place an anchor using the 'A' button at the controller's position, after successfully completing an orange cuboid should appear at the position. The user can erase and destroy previously created/queried/discovered anchors by using the 'B' button. The user can retrieve shared anchors and persisted anchors by querying and discovering all anchors using the 'X' button. The user can share all tracked anchors using the 'Y' button.
