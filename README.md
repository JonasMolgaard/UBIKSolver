# UpperBody IK (UBIK) #

UBIK is an Upper Body IK solution for VR inspired by Mathias Parger's master thesis http://diglib.tugraz.at/download.php?id=5c4a48dc5a282&location=browse
which was implemented in Unity and shared here: https://github.com/dabeschte/VRArmIK

## Features ##

* VR Upper Body based on two controllers and hmd
* 360deg movement
* Simple support for skeletal meshes
* Support for IK chain going from Pelvis and up
* Easy to use Plugin
* Bonus utility functions (RotatePointAroundPivot, SafeguardAngle, CosineRule, FindBetweenNormals)
* Compatible with UE4.24+

### Requirements ###
* SkeletalMesh must be rigged with the default UE4 mannequin skeleton.

## Versionhistory ##
# 1.1 (Contributed by @MarkJGx)
* UBIKSolver is a seperate plugin now.
* Added Solver node bone axis orientation support, toggleable with a boolean switch on the node.
* Added Solver node boolean switches for Head, Right and Left hand.
* Solver node now previews within the editor.
* Re-enabled the debug boolean switch.
* Bone categories have been compacted into one common "Bones" category.
* LFS removed, no more file missing shenanigans.
* Code: renamed variables to use PascalCase for engine code continuity
* Code: replaced GENERATED_USTRUCT_BODY with GENERATED_BODY
* Code: replaced hardcoded parent function calls with Super on overridden functions
* Code: replaced debug draw with anim proxy based debug draw under engine's ENABLE_ANIM_DRAW_DEBUG
* Code: added new array called AllBones, used to validate evaluation.
* Code: use const ref where possible, optimization.

# 1.03 #
* Incorporated BaseCharOffset (X and Y component) into the local ShoulderLocation offset. This allow us to tweak the positioning back and forth of the body.

# 1.01 #
* Fix: Assume basepose in designtime, so you can see your mesh.
* Changed: Default LocalHandOffset (14,-2,0).

# 1.00 #
* First version that packages and has all the basic original BP features implemented.
* Missing feature: Upper and Lower wristbones are not implemented atm.




