// 2020 Sticky Snout Studio (Jonas Molgaard)

#include "AnimNode_UBIKSolver.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"

/** STATS FOR USE WITH PROFILER **/
DECLARE_CYCLE_STAT(TEXT("UBIK_EvaluateThread"), STAT_UBIK_EvaluateThread, STATGROUP_Character);

FAnimNode_UBIKSolver::FAnimNode_UBIKSolver(): Super(),
                                              bApplyHeadTransform(true),
                                              bApplyRightHandTransform(true),
                                              bApplyLeftHandTransform(true),
                                              bIgnorePelvisLocation(false),
                                              bApplyBoneAxis(false),
                                              BoneAxis(EBoneAxis::BA_Y),
                                              bDrawDebug(false)


{
}

void FAnimNode_UBIKSolver::GatherDebugData(FNodeDebugData& DebugData)
{
    Super::GatherDebugData(DebugData);
}

void FAnimNode_UBIKSolver::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
    Super::Initialize_AnyThread(Context);

    Context.AnimInstanceProxy->GetSkelMeshComponent()->SetTickGroup(TG_DuringPhysics);
}

void FAnimNode_UBIKSolver::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
    SCOPE_CYCLE_COUNTER(STAT_UBIK_EvaluateThread);
    Super::EvaluateSkeletalControl_AnyThread(Output, OutBoneTransforms);

    if (Output.AnimInstanceProxy->GetSkelMeshComponent() && Output.AnimInstanceProxy->GetSkelMeshComponent()->GetWorld())
    {
        World = Output.AnimInstanceProxy->GetSkelMeshComponent()->GetWorld();
    }

    if (Alpha <= 0.0f)
    {
        return;
    }

    // check(OutBoneTransforms.Num() == 0);

    //UE_LOG(LogUBIKRuntime, Display, TEXT("DeltaTime: %f"), DeltaTime);
    SkeletalMeshComponent = Output.AnimInstanceProxy->GetSkelMeshComponent();
    ComponentSpaceWorld = Output.AnimInstanceProxy->GetComponentTransform();
    ComponentSpace = ComponentSpaceWorld.Inverse();

    ConvertTransforms();

    SetShoulder();
    SetLeftUpperArm();
    SetRightUpperArm();

    ResetUpperArmsLocation();
    SolveArms();

    BaseCharTransformComponentSpace = GetBaseCharTransform();

#if ENABLE_ANIM_DRAW_DEBUG
    if (bDrawDebug)
    {
        DrawDebug(Output.AnimInstanceProxy);
    }
#endif

    HeadRotation = UKismetMathLibrary::ComposeRotators(FRotator(90.f, 0.f, 90.f), HeadTransformComponentSpace.Rotator());
    Spine03_Rotation = FMath::RInterpTo(Spine03_Rotation, BaseCharTransformComponentSpace.Rotator(), CachedDeltaTime,
                                        Settings.BodyInterSpeed);
    Spine02_Rotation = FMath::RInterpTo(Spine02_Rotation, Spine03_Rotation, CachedDeltaTime, Settings.BodyInterSpeed);
    Spine01_Rotation = FMath::RInterpTo(Spine01_Rotation, Spine02_Rotation, CachedDeltaTime, Settings.BodyInterSpeed);
    PelvisRotation = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(90.f, 0.f, 90.f), Spine01_Rotation),
                                BaseCharTransformComponentSpace.GetTranslation(), FVector::OneVector);

    ClavicleLRotation = LeftClavicleComponentSpace.Rotator();
    UpperArmLRotation = LeftUpperArmTransformComponentSpace.Rotator();
    LowerArmLRotation = LeftLowerArmTransformComponentSpace.Rotator();
    HandLRotation = FRotator(LeftHandTransformComponentSpace.Rotator().Quaternion() * FRotator(0.f, -25.f, 180.f).Quaternion());

    ClavicleRRotation = FRotator(RightClavicleComponentSpace.Rotator().Quaternion() * FRotator(180.f, 0.f, 0.f).Quaternion());
    UpperArmRRotation = FRotator(RightUpperArmTransformComponentSpace.Rotator().Quaternion() * FRotator(0.f, 180.f, 180.f).Quaternion());
    LowerArmRRotation = FRotator(RightLowerArmTransformComponentSpace.Rotator().Quaternion() * FRotator(0.f, 180.f, 180.f).Quaternion());
    HandRRotation = FRotator(RightHandTransformComponentSpace.Rotator().Quaternion() * FRotator(180.f, 25.f, 180.f).Quaternion());

    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();

    TArray<FBoneTransform> SortedBoneTransform;

    if (bApplyHeadTransform)
    {
        SetBoneTransform(SortedBoneTransform, PelvisBoneToModify, PelvisRotation, Output, BoneContainer, true, !bIgnorePelvisLocation);

        SetBoneTransform(SortedBoneTransform, Spine01_BoneToModify,
                         FTransform(UKismetMathLibrary::ComposeRotators(FRotator(83.f, 0.f, 90.f), Spine01_Rotation)), Output,
                         BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, Spine02_BoneToModify,
                         FTransform(UKismetMathLibrary::ComposeRotators(FRotator(104.0f, 0.f, 90.f), Spine02_Rotation)), Output,
                         BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, Spine03_BoneToModify,
                         FTransform(UKismetMathLibrary::ComposeRotators(FRotator(86.22f, 0.f, 90.f), Spine03_Rotation)), Output,
                         BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, HeadBoneToModify, FTransform(HeadRotation), Output, BoneContainer, true);
    }

    if (bApplyLeftHandTransform)
    {
        SetBoneTransform(SortedBoneTransform, LeftClavicleBoneToModify, FTransform(ClavicleLRotation), Output, BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, LeftUpperArmBoneToModify, FTransform(UpperArmLRotation), Output, BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, LeftLowerArmBoneToModify, FTransform(LowerArmLRotation), Output, BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, LeftHandBoneToModify, FTransform(HandLRotation), Output, BoneContainer, true);
    }

    if (bApplyRightHandTransform)
    {
        SetBoneTransform(SortedBoneTransform, RightClavicleBoneToModify, FTransform(ClavicleRRotation), Output, BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, RightUpperArmBoneToModify, FTransform(UpperArmRRotation), Output, BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, RightLowerArmBoneToModify, FTransform(LowerArmRRotation), Output, BoneContainer, true);
        SetBoneTransform(SortedBoneTransform, RightHandBoneToModify, FTransform(HandRRotation), Output, BoneContainer, true);
    }

    // We need to sort bones per bone index before adding to OutBoneTransform or we will fail the BonePose check.
    if (SortedBoneTransform.Num() > 0)
    {
        SortedBoneTransform.Sort(FCompareBoneTransformIndex());
        OutBoneTransforms.Append(SortedBoneTransform);
    }
}


void FAnimNode_UBIKSolver::SetBoneTransform(TArray<FBoneTransform>& OutBoneTransforms, const FBoneReference& BoneToModify,
                                            const FTransform& InTransform, FComponentSpacePoseContext& Output,
                                            const FBoneContainer& BoneContainer, bool bApplyRotation, bool bApplyTranslation)
{
    const FCompactPoseBoneIndex& CompactBoneIndex = BoneToModify.GetCompactPoseIndex(BoneContainer);

    if (CompactBoneIndex != INDEX_NONE)
    {
        FTransform NewBoneTransform = Output.Pose.GetComponentSpaceTransform(CompactBoneIndex);

        if (bApplyRotation)
        {
            if (bApplyBoneAxis)
            {
                const FVector RotationAxis = GetAxisVector(BoneAxis);

                // Fix per-skeleton bone orientation by composing given rotation with BoneAxis dir * 90.0f 
                NewBoneTransform.SetRotation(
                    UKismetMathLibrary::ComposeRotators(FRotator(RotationAxis.X * 90.0f, RotationAxis.Y * 90.0f, RotationAxis.Z * 90.0f),
                                                        InTransform.Rotator()).Quaternion());
            }
            else
            {
                NewBoneTransform.SetRotation(InTransform.GetRotation());
            }
        }

        if (bApplyTranslation)
        {
            NewBoneTransform.SetTranslation(InTransform.GetTranslation());
        }

        // @todo: OutBoneTransforms don't match LocalBlendCSBoneTransforms, even though they are both component space?
        // OutBoneTransforms.Add(FBoneTransform(CompactBoneIndex, NewBoneTransform));

        TArray<FBoneTransform> TempTransform;
        TempTransform.Add(FBoneTransform(CompactBoneIndex, NewBoneTransform));
        Output.Pose.LocalBlendCSBoneTransforms(TempTransform, Alpha);
        TRACE_ANIM_NODE_VALUE(Output, TEXT("Target"), BoneToModify.BoneName);
    }
}

FVector FAnimNode_UBIKSolver::GetAxisVector(const EBoneAxis Axis) const
{
    switch (Axis)
    {
    case BA_X:
    default:
        return FVector(1.f, 0.f, 0.f);
    case BA_Y:
        return FVector(0.f, 1.f, 0.f);
    case BA_Z:
        return FVector(0.f, 0.f, 1.f);
    }
}

bool FAnimNode_UBIKSolver::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    for (const FBoneReference& Bone : AllBones)
    {
        if (!Bone.IsValidToEvaluate(RequiredBones))
        {
            return false;
        }
    }

    return true;
}

void FAnimNode_UBIKSolver::UpdateInternal(const FAnimationUpdateContext& Context)
{
    Super::UpdateInternal(Context);

    CachedDeltaTime = Context.GetDeltaTime();
}

void FAnimNode_UBIKSolver::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
    Super::InitializeBoneReferences(RequiredBones);

    AllBones.Reset();

    HeadBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(HeadBoneToModify);

    LeftClavicleBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(LeftClavicleBoneToModify);

    RightClavicleBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(RightClavicleBoneToModify);

    LeftUpperArmBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(LeftUpperArmBoneToModify);

    RightUpperArmBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(RightUpperArmBoneToModify);

    LeftLowerArmBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(LeftLowerArmBoneToModify);

    RightLowerArmBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(RightLowerArmBoneToModify);

    LeftHandBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(LeftHandBoneToModify);

    RightHandBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(RightHandBoneToModify);

    Spine01_BoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(Spine01_BoneToModify);

    Spine02_BoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(Spine02_BoneToModify);

    Spine03_BoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(Spine03_BoneToModify);

    PelvisBoneToModify.Initialize(RequiredBones);
    AllBones.Emplace(PelvisBoneToModify);
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, ShoulderTransform, LeftTransformW, RightTransformW, 
* HeadTransformC, LeftTransformC, RightTransformC
**/
void FAnimNode_UBIKSolver::ConvertTransforms()
{
    // Technically ShoulderTransformC should be set 1st
    ShoulderTransformWorld = ShoulderTransformComponentSpace * ComponentSpaceWorld;
    ShoulderTransform = ShoulderTransformWorld.Inverse();

    //UE_LOG(LogUBIKRuntime, Display, TEXT("1:LeftHandTransformW: %s"), *LeftHandTransformW.GetTranslation().ToString());
    //UE_LOG(LogUBIKRuntime, Display, TEXT("Settings.LocalHandOffset: %s"), *Settings.LocalHandOffset.GetTranslation().ToString());
    LeftHandTransformWorld = UUBIK::AddLocalOffset(InLeftHandTransformWorld, Settings.LocalHandOffset);
    LeftHandTransformWorld.SetRotation(LeftHandTransformWorld.Rotator().Add(Settings.LocalHandRotationOffset.Pitch,
                                                                            Settings.LocalHandRotationOffset.Yaw,
                                                                            Settings.LocalHandRotationOffset.Roll).Quaternion());
    //DebugDrawAxes(LeftHandTransformW, true);

    //UE_LOG(LogUBIKRuntime, Display, TEXT("2:LeftHandTransformW: %s"), *LeftHandTransformW.GetTranslation().ToString());
    // Mirror Y-direction otherwise it will move opposite ways.
    RightHandTransformWorld = UUBIK::AddLocalOffset(InRightHandTransformWorld, Settings.LocalHandOffset * FVector(1.f, -1.f, 1.f));
    RightHandTransformWorld.SetRotation(RightHandTransformWorld.Rotator().Add(Settings.LocalHandRotationOffset.Pitch,
                                                                              Settings.LocalHandRotationOffset.Yaw,
                                                                              Settings.LocalHandRotationOffset.Roll).Quaternion());


    HeadTransformComponentSpace = InHeadTransformWorld * ComponentSpace;
    LeftHandTransformComponentSpace = LeftHandTransformWorld * ComponentSpace;
    RightHandTransformComponentSpace = RightHandTransformWorld * ComponentSpace;
}

/** SideEffects: Updates the following MemberVariables:
* ShoulderTransformW, HeadTransformS, LeftTransformS, RightTransformS
**/
void FAnimNode_UBIKSolver::SetShoulder()
{
    FRotator RotationFromHead = GetShoulderRotationFromHead();
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RotationFromHead: %s"), *RotationFromHead.ToString());

    FRotator RotationFromHands = GetShoulderRotationFromHands();
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RotationFromHands: %s"), *RotationFromHands.ToString());

    FRotator Rotation = FRotator(
        0.f, UKismetMathLibrary::RLerp(RotationFromHead, RotationFromHands, Settings.ShoulderHeadHandAlpha, true).Yaw, 0.f);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("Rotation: %s"), *Rotation.ToString());

    FVector Translation = GetShoulderLocation();
    //UE_LOG(LogUBIKRuntime, Display, TEXT("GetShoulderLocation: %s"), *Translation.ToString());

    ShoulderTransformComponentSpace = FTransform(Rotation, Translation, FVector::OneVector);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("ShoulderTransformC: %s"), *ShoulderTransformC.Rotator().ToString());

    ShoulderTransformWorld = ShoulderTransformComponentSpace * ComponentSpaceWorld;
    HeadTransformS = InHeadTransformWorld * ShoulderTransform;
    LeftHandTransformS = LeftHandTransformWorld * ShoulderTransform;
    RightHandTransformS = RightHandTransformWorld * ShoulderTransform;
}

FVector FAnimNode_UBIKSolver::GetShoulderLocation()
{
    FVector Offset = FVector(-9.f, 0.f, -7.f) + FVector(Settings.BaseCharOffset.X, Settings.BaseCharOffset.Y, 0.f);
    FRotator HeadRotator = HeadTransformComponentSpace.Rotator();
    FRotator HeadYaw = FRotator(0.f, HeadRotator.Yaw, HeadRotator.Roll);

    Offset = HeadYaw.RotateVector(Offset);

    FRotator Delta = FRotator(HeadRotator.Pitch, 0.f, 0.f);

    FVector ShoulderTranslation = HeadTransformComponentSpace.GetTranslation() + Offset;
    FTransform TempShoulder = FTransform(FRotator::ZeroRotator, ShoulderTranslation, FVector::OneVector);

    FTransform TempRotatedShoulder = UUBIK::RotatePointAroundPivot(TempShoulder, HeadTransformComponentSpace, Delta);

    return TempRotatedShoulder.GetTranslation() + FVector(0.f, 0.f, -17.f);
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHead()
{
    return FRotator(0.f, HeadTransformComponentSpace.Rotator().Yaw, 0.f);
}

FRotator FAnimNode_UBIKSolver::GetShoulderRotationFromHands()
{
    FVector Offset = FVector(0.f, 0.f, 15.f);
    FVector TopHeadTranslation = InHeadTransformWorld.GetTranslation() + InHeadTransformWorld.Rotator().RotateVector(Offset);

    FVector Hand = (LeftHandTransformWorld * InHeadTransformWorld.Inverse()).GetTranslation();
    FVector HandHeadDelta = LeftHandTransformWorld.GetTranslation() - TopHeadTranslation;
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LEFTHAND: LeftTransformW: %s LastAngle: %f Hand: %s HandHeadDelta: %s"), *LeftHandTransformW.ToString(), LeftHeadHandAngle, *Hand.ToString(), *HandHeadDelta.ToString());

    LeftHeadHandAngle = GetHeadHandAngle(LeftHeadHandAngle, Hand, HandHeadDelta);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LeftHeadHandAngle: %f"), LeftHeadHandAngle);

    Hand = (RightHandTransformWorld * InHeadTransformWorld.Inverse()).GetTranslation();
    HandHeadDelta = RightHandTransformWorld.GetTranslation() - TopHeadTranslation;
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RIGHTHAND: RightTransformW: %s LastAngle: %f Hand: %s HandHeadDelta: %s"), *RightTransformW.ToString(), RightHeadHandAngle, *Hand.ToString(), *HandHeadDelta.ToString());
    RightHeadHandAngle = GetHeadHandAngle(RightHeadHandAngle, Hand, HandHeadDelta);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RightHeadHandAngle: %f"), RightHeadHandAngle);

    FRotator YawRotator = FRotator(0.f, ((LeftHeadHandAngle + RightHeadHandAngle) / 2.f), 0.f);
    FTransform TempTransform = FTransform(YawRotator, FVector::ZeroVector, FVector::OneVector);

    // Back in WorldSpace
    TempTransform = TempTransform * InHeadTransformWorld;

    TempTransform = TempTransform * ComponentSpace;

    return TempTransform.Rotator();
}

float FAnimNode_UBIKSolver::GetHeadHandAngle(float LastAngle, const FVector& Hand, const FVector& HandHeadDelta)
{
    float HeadHandAlpha = UKismetMathLibrary::MapRangeClamped(FVector(HandHeadDelta.X, HandHeadDelta.Y, 0.f).Size(), 20.f, 50.f, 0.f, 1.f);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("HeadHandAlpha: %f"), HeadHandAlpha);

    FVector Hand2D = FVector(Hand.X, Hand.Y, 0.f);
    FVector Hand2DNormalized = Hand2D.GetSafeNormal();
    float Angle = FMath::RadiansToDegrees(FMath::Atan2(Hand2DNormalized.Y, Hand2DNormalized.X));
    //UE_LOG(LogUBIKRuntime, Display, TEXT("Angle: %f"), Angle);

    bool Selector1 = FVector::DotProduct(Hand2DNormalized, FVector::ForwardVector) > Settings.HeadHandAngleLimitDot;
    bool Selector2 = (FMath::Sign(LastAngle) == FMath::Sign(Angle)) ||
        (Angle < Settings.OkSpanAngle && Angle > -1.0f * Settings.OkSpanAngle);
    bool Selector = Selector1 && Selector2;

    float SelectedFloat = (Selector) ? (Angle) : (Settings.HeadHandAngleLimit * FMath::Sign(LastAngle));
    //UE_LOG(LogUBIKRuntime, Display, TEXT("SelectedFloat: %f"), SelectedFloat);

    return FMath::Lerp(0.f, SelectedFloat, HeadHandAlpha);
}

void FAnimNode_UBIKSolver::SetLeftUpperArm()
{
    LeftUpperArmTransformS = RotateUpperArm(true, LeftHandTransformS.GetTranslation());
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LeftUpperArmTransformS %s"), *LeftUpperArmTransformS.ToString());

    const FVector& X = (LeftUpperArmTransformS * ShoulderTransformWorld).GetTranslation() - ShoulderTransformWorld.GetTranslation();
    const FVector& Z = FVector::UpVector;

    LeftClavicleComponentSpace = FTransform(UKismetMathLibrary::MakeRotFromXZ(X, Z), FVector::ZeroVector, FVector::OneVector) *
        ComponentSpace;
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LeftClavicleC %s"), *LeftClavicleC.ToString());
}

void FAnimNode_UBIKSolver::SetRightUpperArm()
{
    RightUpperArmTransformS = RotateUpperArm(false, RightHandTransformS.GetTranslation());
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RightUpperArmTransformS %s"), *RightUpperArmTransformS.ToString());

    const FVector& X = (RightUpperArmTransformS * ShoulderTransformWorld).GetTranslation() - ShoulderTransformWorld.GetTranslation();
    const FVector& Z = FVector::UpVector;

    RightClavicleComponentSpace = FTransform(UKismetMathLibrary::MakeRotFromXZ(X, Z), FVector::ZeroVector, FVector::OneVector) *
        ComponentSpace;
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RightClavicleC %s"), *RightClavicleC.ToString());
}

FTransform FAnimNode_UBIKSolver::RotateUpperArm(bool IsLeftArm, const FVector& HandTranslation)
{
    const float Sign = (IsLeftArm) ? (1.f) : (-1.f);

    const FVector InitialUpperArmTranslation = FVector::RightVector * Settings.UpperArmsDistance / (2.f * Sign);
    const FVector HandUpperArmDirection = HandTranslation - InitialUpperArmTranslation;

    const float ForwardDistanceRatio = FVector::DotProduct(HandUpperArmDirection, FVector::ForwardVector) / Settings.ArmLength;
    const float UpwardsDistanceRatio = FVector::DotProduct(HandUpperArmDirection, FVector::UpVector) / Settings.ArmLength;

    float Yaw;
    if (ForwardDistanceRatio > 0)
    {
        const float TempYaw = (ForwardDistanceRatio - 0.5f) * Settings.DistinctShoulderRotationMultiplier;
        Yaw = FMath::Clamp(TempYaw, 0.f, Settings.DistinctShoulderRotationLimit) + Settings.ClavicleOffset;
    }
    else
    {
        const float TempYaw = (ForwardDistanceRatio - 0.08f) * Settings.DistinctShoulderRotationMultiplier;
        Yaw = FMath::Clamp(TempYaw, -Settings.DistinctShoulderRotationLimit, 0.f) + Settings.ClavicleOffset;
    }

    const float TempRoll = (UpwardsDistanceRatio - 0.2f) * Settings.DistinctShoulderRotationMultiplier;
    const float Roll = FMath::Clamp(TempRoll, 0.f, Settings.DistinctShoulderRotationLimit);

    const float NotSign = (IsLeftArm) ? (-1.f) : (1.f);
    const FRotator Rotation = FRotator(0.f, Yaw * NotSign, Roll * NotSign);

    return FTransform(Rotation, InitialUpperArmTranslation, FVector::OneVector).Inverse();
}

/** The purpose of this function, is to make sure the UpperArm_l and UpperArm_r locations 
* are used in the coming calculations to ensure they reach right angle.
* They do that by setting LeftUpperArmTransformS and RightUpperArmTransformS.
*/
void FAnimNode_UBIKSolver::ResetUpperArmsLocation()
{
    if (SkeletalMeshComponent && SkeletalMeshComponent->GetSkinnedAsset())
    {
        const FVector& LeftUpperArm = SkeletalMeshComponent->GetSocketTransform(LeftUpperArmBoneToModify.BoneName, RTS_World).GetTranslation();
        LeftUpperArmTransformS = FTransform(UKismetMathLibrary::InverseTransformLocation(ShoulderTransformWorld, LeftUpperArm));
        //UE_LOG(LogUBIKRuntime, Display, TEXT("LeftUpperArm %s"), *LeftUpperArm.ToString());

        const FVector& RightUpperArm = SkeletalMeshComponent->GetSocketTransform(RightUpperArmBoneToModify.BoneName, RTS_World).GetTranslation();
        RightUpperArmTransformS = FTransform(UKismetMathLibrary::InverseTransformLocation(ShoulderTransformWorld, RightUpperArm));
    }
}

void FAnimNode_UBIKSolver::SolveArms()
{
    /** LEFT ARM **/
    FVector HandLoc = LeftHandTransformS.GetTranslation();
    FRotator HandRot = LeftHandTransformS.GetRotation().Rotator();
    FVector UpperArmLoc = LeftUpperArmTransformS.GetTranslation();

    FTransform UpperArmBase = FTransform::Identity;
    FTransform LowerArmBase = FTransform::Identity;
    FTransform UpperArm = FTransform::Identity;
    FTransform LowerArm = FTransform::Identity;

    //UE_LOG(LogUBIKRuntime, Display, TEXT("UA: %s H: %s"), *UpperArmLoc.ToString(), *HandLoc.ToString()); 
    SetElbowBasePosition(UpperArmLoc, HandLoc, true, UpperArmBase, LowerArmBase);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LowerArmBase: %s"), *LowerArmBase.ToString());

    float BaseAngle = RotateElbowByHandPosition(HandLoc, true);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("BaseAngle: %f"), BaseAngle);

    RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, UpperArm, LowerArm);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LowerArm: %s"), *LowerArm.ToString());

    float Angle = RotateElbowByHandRotation(LowerArm, HandRot);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("Angle: %f"), Angle);

    float SafeAngle = UUBIK::SafeguardAngle(LeftElbowHandAngle, Angle, 120.f);

    LeftElbowHandAngle = FMath::FInterpTo(LeftElbowHandAngle, SafeAngle, CachedDeltaTime, Settings.ElbowHandsRotSpeed);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LeftElbowHandAngle: %f"), LeftElbowHandAngle);

    RotateElbow(LeftElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, true, LeftUpperArmTransformS, LeftLowerArmTransformS);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LeftElbowHandAngle + BaseAngle: %f"), LeftElbowHandAngle + BaseAngle);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LUAS: %s LLAS: %s"), *LeftUpperArmTransformS.ToString(), *LeftLowerArmTransformS.ToString());

    LeftUpperArmTransformWorld = LeftUpperArmTransformS * ShoulderTransformWorld;
    LeftLowerArmTransformWorld = LeftLowerArmTransformS * ShoulderTransformWorld;

    float Roll = FMath::Max((LeftHandTransformWorld.GetTranslation() - LeftUpperArmTransformWorld.GetTranslation()).Z, 0.f);
    FTransform UpperArmS = LeftUpperArmTransformWorld * ComponentSpace;

    LeftUpperArmTransformComponentSpace = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(0.f, 0.f, Roll), UpperArmS.Rotator()),
                                                     UpperArmS.GetTranslation());
    LeftLowerArmTransformComponentSpace = LeftLowerArmTransformWorld * ComponentSpace;

    /** RIGHT ARM **/
    HandLoc = RightHandTransformS.GetTranslation();
    HandRot = RightHandTransformS.GetRotation().Rotator();
    UpperArmLoc = RightUpperArmTransformS.GetTranslation();

    SetElbowBasePosition(UpperArmLoc, HandLoc, false, UpperArmBase, LowerArmBase);

    BaseAngle = RotateElbowByHandPosition(HandLoc, false);

    UpperArm = FTransform::Identity;
    LowerArm = FTransform::Identity;

    RotateElbow(BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, UpperArm, LowerArm);

    Angle = RotateElbowByHandRotation(LowerArm, HandRot);

    SafeAngle = UUBIK::SafeguardAngle(RightElbowHandAngle, Angle, 120.f);

    RightElbowHandAngle = FMath::FInterpTo(RightElbowHandAngle, SafeAngle, CachedDeltaTime, Settings.ElbowHandsRotSpeed);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("RightElbowHandAngle: %f"), RightElbowHandAngle);

    RotateElbow(RightElbowHandAngle + BaseAngle, UpperArmBase, LowerArmBase, HandLoc, false, RightUpperArmTransformS,
                RightLowerArmTransformS);
    RightUpperArmTransformWorld = RightUpperArmTransformS * ShoulderTransformWorld;
    RightLowerArmTransformWorld = RightLowerArmTransformS * ShoulderTransformWorld;

    Roll = -1.f * FMath::Max((RightHandTransformWorld.GetTranslation() - RightUpperArmTransformWorld.GetTranslation()).Z, 0.f);
    UpperArmS = RightUpperArmTransformWorld * ComponentSpace;

    RightUpperArmTransformComponentSpace = FTransform(UKismetMathLibrary::ComposeRotators(FRotator(0.f, 0.f, Roll), UpperArmS.Rotator()),
                                                      UpperArmS.GetTranslation());
    RightLowerArmTransformComponentSpace = RightLowerArmTransformWorld * ComponentSpace;
}

void FAnimNode_UBIKSolver::SetElbowBasePosition(const FVector& UpperArm, const FVector& Hand, bool bIsLeftArm,
                                                FTransform& UpperArmTransform, FTransform& LowerArmTransform)
{
    const float UpperArmToHandLen = (UpperArm - Hand).Size();

    const float Sign = (bIsLeftArm) ? (-1.f) : (1.f);

    const float Beta = UUBIK::CosineRule(Settings.UpperArmLength, UpperArmToHandLen, Settings.LowerArmLength) * Sign;
    //UE_LOG(LogUBIKRuntime, Display, TEXT("Beta: %f"), Beta);

    float Omega = UUBIK::CosineRule(Settings.LowerArmLength, Settings.UpperArmLength, UpperArmToHandLen);
    Omega = (bIsLeftArm) ? (180.f - Omega) : (180.f + Omega);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("Omega: %f"), Omega);

    const FRotator A = FQuat::FindBetweenNormals(FVector::ForwardVector, (Hand - UpperArm).GetSafeNormal()).Rotator();
    //UE_LOG(LogUBIKRuntime, Display, TEXT("FindBetweenNormals: %s"), *A.ToString());
    const FRotator B = FQuat(UKismetMathLibrary::GetUpVector(A), FMath::DegreesToRadians(Beta)).Rotator();
    //UE_LOG(LogUBIKRuntime, Display, TEXT("AxisAngleRot: %s"), *B.ToString());

    UpperArmTransform = FTransform((UKismetMathLibrary::ComposeRotators(A, B)), UpperArm, FVector::OneVector);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("UpperArmTransform: %s"), *UpperArmTransform.ToString());

    const FTransform TempLowerArm = FTransform(FRotator(0.f, Omega, 0.f), FVector::ForwardVector * Settings.UpperArmLength,
                                               FVector::OneVector);
    LowerArmTransform = TempLowerArm * UpperArmTransform;
    //UE_LOG(LogUBIKRuntime, Display, TEXT("LowerArmTransform: %s"), *LowerArmTransform.ToString());
}

float FAnimNode_UBIKSolver::RotateElbowByHandPosition(const FVector& Hand, bool bIsLeftArm)
{
    const FVector TempHand = Hand / Settings.ArmLength;

    const float Sign = (bIsLeftArm) ? (1.f) : (-1.f);

    const float Angle = Settings.ElbowBaseOffsetAngle +
        (Settings.ElbowYWeight * FMath::Max(0.f, (TempHand.Y * Sign) + Settings.ElbowYDistanceStart));

    return Angle;
}

float FAnimNode_UBIKSolver::RotateElbowByHandRotation(const FTransform& LowerArm, FRotator Hand)
{
    const FRotator LowerArmRot = LowerArm.Rotator();

    const FVector LowerArmRight = UKismetMathLibrary::GetRightVector(LowerArmRot);
    //UE_LOG(LogUBIKRuntime, Display, TEXT("GetRightVector:: %s"), *LowerArmRight.ToString());

    const FVector ProjectedForward = UKismetMathLibrary::ProjectVectorOnToPlane(
        UKismetMathLibrary::GetForwardVector(Hand), LowerArmRight
    ).GetSafeNormal();
    //UE_LOG(LogUBIKRuntime, Display, TEXT("ProjectedForward:: %s"), *ProjectedForward.ToString());

    const FVector LowerArmForward = UKismetMathLibrary::GetForwardVector(LowerArmRot);

    float TempAngle = UUBIK::CosineRule(1.f, 1.f, (ProjectedForward - LowerArmForward).Size());

    const float Sign = (FVector::DotProduct(FVector::CrossProduct(ProjectedForward, LowerArmForward), LowerArmRight) < 0.f)
                           ? (1.f)
                           : (-1.f);
    TempAngle = TempAngle * Sign;
    TempAngle = TempAngle * Settings.ElbowRotFromHandRotAlpha;

    return TempAngle;
}

void FAnimNode_UBIKSolver::RotateElbow(float Angle, const FTransform& UpperArm, const FTransform& LowerArm, const FVector& HandLoc,
                                       bool bIsLeftArm, FTransform& NewUpperArm, FTransform& NewLowerArm)
{
    const FVector PivotLoc =
        UpperArm.GetTranslation() +
        UKismetMathLibrary::ProjectVectorOnToVector(UpperArm.GetTranslation() - LowerArm.GetTranslation(),
                                                    UpperArm.GetTranslation() - HandLoc);

    const FVector Forward = UpperArm.GetTranslation() - HandLoc;
    const FVector Right = FVector::CrossProduct(UKismetMathLibrary::GetUpVector(UpperArm.Rotator()), Forward);
    const FVector Up = UKismetMathLibrary::GetUpVector(UpperArm.Rotator());

    FRotator PivotRot = UKismetMathLibrary::MakeRotationFromAxes(Forward, Right, Up);

    const float Roll = (bIsLeftArm) ? (180.f - Angle) : (180.f + Angle);
    const FRotator Delta = FRotator(0.f, 0.f, Roll);
    // UE_LOG(LogUBIKRuntime, Display, TEXT("!!! Delta: %s"), *Delta.ToString());

    const FTransform Pivot = FTransform(PivotRot, PivotLoc, FVector::OneVector);
    // UE_LOG(LogUBIKRuntime, Display, TEXT("!!! Pivot: %s"), *Pivot.ToString()); --> Pivot is correct!

    NewUpperArm = UUBIK::RotatePointAroundPivot(UpperArm, Pivot, Delta);
    NewLowerArm = UUBIK::RotatePointAroundPivot(LowerArm, Pivot, Delta);
    // UE_LOG(LogUBIKRuntime, Display, TEXT("!!! NewUpperArm: %s - NewLowerArm: %s"), *NewUpperArm.ToString(), *NewLowerArm.ToString());
}

FTransform FAnimNode_UBIKSolver::GetBaseCharTransform()
{
    // I've already added the X and the Y component in SetShoulder(), that's why i negate these here.
    return FTransform(ShoulderTransformComponentSpace.Rotator(),
                      ShoulderTransformComponentSpace.GetTranslation() + FVector(0.f, 0.f, Settings.BaseCharOffset.Z), FVector::OneVector);
}


void FAnimNode_UBIKSolver::DrawDebug(FAnimInstanceProxy* AnimInstanceProxy)
{
#if ENABLE_ANIM_DRAW_DEBUG
    if (bApplyLeftHandTransform)
    {
        // Actual world transform
        DebugDrawAxes(AnimInstanceProxy, InLeftHandTransformWorld, true, FColor::Orange);

        // Calculated hand transform with offsets
        DebugDrawAxes(AnimInstanceProxy, LeftHandTransformWorld, true, FLinearColor::Gray.ToFColor(true));

        // Upper arm/lower arm
        DebugDrawAxes(AnimInstanceProxy, LeftUpperArmTransformWorld, true, FColor::Blue);
        DebugDrawAxes(AnimInstanceProxy, LeftLowerArmTransformWorld, true, FColor::Cyan);
    }

    if (bApplyRightHandTransform)
    {
        // Actual world transform
        DebugDrawAxes(AnimInstanceProxy, InRightHandTransformWorld, true, FColor::Orange);

        // Calculated hand transform with offsets
        DebugDrawAxes(AnimInstanceProxy, RightHandTransformWorld, true, FLinearColor::Gray.ToFColor(true));

        // Upper arm/lower arm
        DebugDrawAxes(AnimInstanceProxy, RightUpperArmTransformWorld, true, FColor::Blue);
        DebugDrawAxes(AnimInstanceProxy, RightLowerArmTransformWorld, true, FColor::Cyan);
    }

    if (bApplyHeadTransform)
    {
        // Actual head transform
        DebugDrawAxes(AnimInstanceProxy, InHeadTransformWorld, true, FColor::Red);
    }
#endif
}

void FAnimNode_UBIKSolver::DebugDrawAxes(FAnimInstanceProxy* AnimInstanceProxy, const FTransform& Transform, bool bDrawAxes,
                                         FColor SphereColor)
{
#if ENABLE_ANIM_DRAW_DEBUG

    AnimInstanceProxy->AnimDrawDebugSphere(Transform.GetTranslation(), 5.2f, 20.0f, SphereColor);
    if (bDrawAxes)
    {
        AnimInstanceProxy->AnimDrawDebugCoordinateSystem(Transform.GetLocation(), Transform.Rotator(), 5.0f, false, -1.0f,.1f);
    }
#endif
}
