# OpenXR Passthrough Occlusion Sample

## Overview
The `XR_META_environment_depth` extension allows the application to request depth maps of the real-world environment surrounding the headset and perform passthrough occlusion based on that. Occlusion is one of the mixed reality (MR) software features that support the hiding or masking of virtual/augmented objects behind real-world objects (e.g., furniture, limbs, people). In MR, occlusions are essential for rendering believable visual interactions between virtual and physical content. They are one of the key components in enabling a coherent sense of reality for users. When a virtual object hides behind a physical object, but the application cannot render it appropriately, the believability breaks and the user experience suffers.

## The Sample
This native sample app demonstrates Passthrough and "hard occlusions" implemented using the Depth API. It allows the user to move a cube attached to both the left and right controller and observe how it gets occluded by the real world around the user.
