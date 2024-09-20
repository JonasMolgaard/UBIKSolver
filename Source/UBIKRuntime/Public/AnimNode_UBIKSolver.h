// 2020 Sticky Snout Studio (Jonas Molgaard)

#pragma once

#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "UBIK.h"
#include "AnimNode_UBIKSolver.generated.h"

/**
 * UBIK Solver
 */
USTRUCT(BlueprintInternalUseOnly)
struct UBIKRUNTIME_API FAnimNode_UBIKSolver : public FAnimNode_SkeletalControlBase
{
    GENERATED_BODY()
public:
    FAnimNode_UBIKSolver();

    /** Feed in the HMD transform in WorldSpace. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    FTransform InHeadTransformWorld;

    /** Feed in the Left MotionController in WorldSpace. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    FTransform InLeftHandTransformWorld;

    /** Feed in the Right MotionController in WorldSpace. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    FTransform InRightHandTransformWorld;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    bool bApplyHeadTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    bool bApplyRightHandTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    bool bApplyLeftHandTransform;

    /** By default Pelvis will be driven from Head location with an offset. Set to true to ignore Location. */
    UPROPERTY(EditAnywhere, Category = UBIK)
    bool bIgnorePelvisLocation;

    UPROPERTY(EditAnywhere, Category = UBIK, meta = (InlineEditConditionToggle))
    bool bApplyBoneAxis;

    UPROPERTY(EditAnywhere, Category = UBIK, meta = (EditCondition="bApplyBoneAxis"))
    TEnumAsByte<EBoneAxis> BoneAxis;

    /** These settings will be returned by calling the GetUBIKSettings function. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UBIK, meta = (PinShownByDefault))
    FUBIKSettings Settings;

    //** Head bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference HeadBoneToModify = FBoneReference("head");

    /** Left Clavicle bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference LeftClavicleBoneToModify = FBoneReference("clavicle_l");

    /** Right Clavicle Arm bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference RightClavicleBoneToModify = FBoneReference("clavicle_r");

    /** Left Upper Arm bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference LeftUpperArmBoneToModify = FBoneReference("upperarm_l");

    /** Right Upper Arm bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference RightUpperArmBoneToModify = FBoneReference("upperarm_r");

    /** Left Lower Arm bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference LeftLowerArmBoneToModify = FBoneReference("lowerarm_l");;

    /** Right Lower Arm bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference RightLowerArmBoneToModify = FBoneReference("lowerarm_r");

    /** Left Hand bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference LeftHandBoneToModify = FBoneReference("hand_l");

    /** Right Hand bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference RightHandBoneToModify = FBoneReference("hand_r");

    //** Spine01 bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference Spine01_BoneToModify = FBoneReference("spine_01");

    //** Spine02 bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference Spine02_BoneToModify = FBoneReference("spine_02");

    //** Spine03 bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference Spine03_BoneToModify = FBoneReference("spine_03");

    //** Pelvis bone to modify */
    UPROPERTY(EditAnywhere, Category = Bones)
    FBoneReference PelvisBoneToModify = FBoneReference("pelvis");

    UPROPERTY(EditAnywhere, Category = Debug)
    bool bDrawDebug;

    // FAnimNode_Base interface
    virtual void GatherDebugData(FNodeDebugData& DebugData) override;
    // End of FAnimNode_Base interface

    // FAnimNode_SkeletalControlBase interface
    virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
    virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
    virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
    virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
    virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
    //virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
    // End of FAnimNode_SkeletalControlBase interface

private:
    /** If true, this will adjust the offset of the Oculus controllers. This is done automatically if you are using the OpenXR SDK. **/
    bool bAdjustOculusOffset = false;

    TArray<FBoneReference> AllBones;

    UPROPERTY(Transient)
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(Transient)
    UWorld* World;

    float CachedDeltaTime;
private:
    FTransform LeftHandTransformWorld;
    FTransform RightHandTransformWorld;

    FTransform ComponentSpaceWorld;
    FTransform ShoulderTransformWorld;
    FTransform LeftUpperArmTransformWorld;
    FTransform RightUpperArmTransformWorld;
    FTransform LeftLowerArmTransformWorld;
    FTransform RightLowerArmTransformWorld;

    FTransform HeadTransformComponentSpace;
    FTransform LeftHandTransformComponentSpace;
    FTransform RightHandTransformComponentSpace;
    FTransform ShoulderTransformComponentSpace;
    FTransform LeftClavicleComponentSpace; // TODO: May be able to turn into a FRotator instead
    FTransform RightClavicleComponentSpace; // TODO: May be able to turn into a FRotator instead
    FTransform BaseCharTransformComponentSpace;
    FTransform LeftUpperArmTransformComponentSpace;
    FTransform LeftLowerArmTransformComponentSpace;
    FTransform RightUpperArmTransformComponentSpace;
    FTransform RightLowerArmTransformComponentSpace;

    /** WorldSpace inverted **/
    FTransform ShoulderTransform;
    FTransform ComponentSpace;

    FTransform HeadTransformS;
    FTransform LeftHandTransformS;
    FTransform RightHandTransformS;
    FTransform LeftUpperArmTransformS;
    FTransform RightUpperArmTransformS;
    FTransform LeftLowerArmTransformS;
    FTransform RightLowerArmTransformS;

    float LeftHeadHandAngle;
    float RightHeadHandAngle;

    float LeftElbowHandAngle;
    float RightElbowHandAngle;

    FRotator HeadRotation;
    FRotator Spine03_Rotation;
    FRotator Spine02_Rotation;
    FRotator Spine01_Rotation;
    FTransform PelvisRotation;
    FRotator ClavicleLRotation;
    FRotator UpperArmLRotation;
    FRotator LowerArmLRotation;
    FRotator HandLRotation;
    FRotator ClavicleRRotation;
    FRotator UpperArmRRotation;
    FRotator LowerArmRRotation;
    FRotator HandRRotation;

private:
    void ConvertTransforms();
    void SetShoulder();
    FVector GetShoulderLocation();
    FRotator GetShoulderRotationFromHead();
    FRotator GetShoulderRotationFromHands();
    float GetHeadHandAngle(float LastAngle, const FVector& Hand, const FVector& HandHeadDelta);

    void SetLeftUpperArm();
    void SetRightUpperArm();
    FTransform RotateUpperArm(bool IsLeftArm, const FVector& HandTranslation);

    void ResetUpperArmsLocation();
    void SolveArms();
    void SetElbowBasePosition(const FVector& UpperArm, const FVector& Hand, bool bIsLeftArm, FTransform& UpperArmTransform,
                              FTransform& LowerArmTransform);
    float RotateElbowByHandPosition(const FVector& Hand, bool bIsLeftArm);
    float RotateElbowByHandRotation(const FTransform& LowerArm, FRotator Hand);
    void RotateElbow(float Angle, const FTransform& UpperArm, const FTransform& LowerArm, const FVector& HandLoc, bool bIsLeftArm,
                     FTransform& NewUpperArm, FTransform& NewLowerArm);

    FTransform GetBaseCharTransform();

    void DrawDebug(FAnimInstanceProxy* AnimInstanceProxy);
    void DebugDrawAxes(FAnimInstanceProxy* AnimInstanceProxy, const FTransform& Transform, bool DrawAxis = true,
                       FColor SphereColor = FColor::Silver);

    /** The given Transform will only apply Translation if explicitly defined. */
    void SetBoneTransform(TArray<FBoneTransform>& OutBoneTransforms, const FBoneReference& BoneToModify, const FTransform& InTransform,
                          FComponentSpacePoseContext& Output, const FBoneContainer& BoneContainer, bool bApplyRotation,
                          bool bApplyTranslation = false);

    FVector GetAxisVector(const EBoneAxis Axis) const;
};
