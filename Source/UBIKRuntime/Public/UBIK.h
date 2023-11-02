// 2020 Sticky Snout Studio (Jonas Molgaard)

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "UBIK.generated.h"

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKSettings
{
    GENERATED_USTRUCT_BODY()

public:
    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float DistinctShoulderRotationMultiplier = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float DistinctShoulderRotationLimit = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ClavicleOffset = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowBaseOffsetAngle = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowYDistanceStart = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowYWeight = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowHandsRotSpeed = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowRotFromHandRotAlpha = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float HeadHandAngleLimit = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float OkSpanAngle = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    FVector BaseCharOffset = FVector(0.f, 0.f, 0.f);

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float BodyInterSpeed = 0.f;

    /** This will be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (ToolTip = ""))
    float HeadHandAngleLimitDot = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ArmLength = 0.f;

    /** This will be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float UpperArmLength = 0.f;

    /** This will be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float LowerArmLength = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float UpperArmsDistance = 0.f;

    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    FVector LocalHandOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    FRotator LocalHandRotationOffset = FRotator::ZeroRotator;
    
    /** This should be set via call to GetUBIKSettings, typically from "Event Blueprint Begin Play" **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ShoulderHeadHandAlpha = 0.f;
};

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKDefaults
{
    GENERATED_USTRUCT_BODY()

public:
    /** Rotation multiplier, that determine how much Shoulder rotates to try reach hand, when it is moving away from ShoulderJoint. Usally keep between (0-70). 0 means the shoulder doesnt rotate, where 60 is a quite extreme shoulder rotation. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float DistinctShoulderRotationMultiplier = 60.f;

    /** Max shoulder rotation limit (in degrees). Controls how much they can turn forward **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float DistinctShoulderRotationLimit = 45.f;

    /** Base offset angle to bring Clavicle back in a relaxed position. Experiment with different offsets if shoulders look weird on your mesh. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ClavicleOffset = -32.f;

    /** Base offset angle to bring elbow down from a straight to the side to approximately down to the side (relaxed base pose). Keep between 70-110. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowBaseOffsetAngle = 90.f;

    /** Offset value for when Y-position relative to chest-direction should influence elbow rotation. Keep between 0-1. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowYDistanceStart = .2f;

    /** Determine how much Y-position relative to chest-direction influences elbow rotation. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowYWeight = 130.f;

    /** Interp speed for elbow rotation. Lower value gives more lag, where higher values gives a more snappy elbow. Try keep between 10-50. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowHandsRotSpeed = 15.f;

    /** How much influence the hands rotation will have on elbow rotation. Set between 0-1. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ElbowRotFromHandRotAlpha = 0.6f;

    /** Max angle of influence for head-to-hand relative to head forward vector
    * Note: I may remove this setting actually, and just hardcode **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float HeadHandAngleLimit = 150.f;

    /** Angle of influence for head-to-hand relative to head forward vector, 
    * if doing a circular movement over head, where the hand's position should again have influence.
    * Note: I may remove this setting actually, and just hardcode **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float OkSpanAngle = 80.f;

    /** Offset from Shoulder to Pelvis **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    FVector BaseCharOffset = FVector(0.f, 0.f, -55.25f);

    /** Interp speed between each bone from head to Pelvis-chain. Higher values give a more stiff upperbody that rotate more 1:1 with head. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float BodyInterSpeed = 10.f;

    /** Local Hand Offset that will be applied to MotionController transforms (LeftHandTransformW and RightHandTransformW) to better fit with your mesh.
    * These may vary for each type of MotionController too, so make sure to adjust this accordingly. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    FVector LocalHandOffset = FVector(14.f, -2.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    FRotator LocalHandRotationOffset = FRotator(0.0f, 0.0f, 0.0f);

    /** Determine where Shoulder rotation comes from. Pr. default it will be weighted between Hands position and Head direction. 
    * 0 = Head. UpperBody (Shoulder base) will be controlled entirely by Head direction.
    * 1 = Hands. UpperBody (Shoulder base) will be controlled by looking at the mean direction of the hands position relative to the Chest.
    **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float ShoulderHeadHandAlpha = 0.75f;
};

USTRUCT(BlueprintType)
struct UBIKRUNTIME_API FUBIKCalibrationData
{
    GENERATED_USTRUCT_BODY()

public:
    /** Actual Player height in cm. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    float Height = 184.f;

    /** Distance between Shoulders (where upperArms are hinged on to). This value also affects the value of UpperArmLength and LowerArmLength. **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinHiddenByDefault, ToolTip = ""))
    float UpperArmsDistance = 30.f;
};

/**
 * UBIK Initializer class
 */
UCLASS()
class UBIKRUNTIME_API UUBIK : public UObject
{
    GENERATED_BODY()

public:

    /** Call this to get the UBIKSettings struct, that is needed on the AnimGraph node. **/
    UFUNCTION(BlueprintCallable, meta = (DisplayName = "GetUBIKSettings", Keywords = "UBIK Calibrate"), Category = "UBIK")
    static FUBIKSettings Initialize(FUBIKDefaults Defaults, FUBIKCalibrationData Calibration);

    /** Adds Offset to Transform in LocalSpace. **/
    static FTransform AddLocalOffset(const FTransform Transform, const FVector Offset);

    /** Rotates the Point around Pivot, by given Delta rotation. **/
    UFUNCTION(BlueprintPure, meta = (DisplayName = "RotatePointAroundPivot", Keywords = "UBIK Rotate Pivot"), Category = "UBIK Utility")
    static FTransform RotatePointAroundPivot(FTransform Point, FTransform Pivot, FRotator Delta);

    /** Angle snapping mitigation function.
    * Supply last angle, current angle and what threshold not to pass.
    * If outside that limit, it will keep the last angle, otherwise the current. **/
    UFUNCTION(BlueprintPure, meta = (DisplayName = "SafeguardAngle", Keywords = "UBIK Safeguard Angle"), Category = "UBIK Utility")
    static float SafeguardAngle(float Last, float Current, float Threshold);

    /** Given lengths of the different sides in a triangle,
    * the angle between Adjacent1 and Adjacent2 are returned. **/
    UFUNCTION(BlueprintPure, meta = (DisplayName = "CosineRule", Keywords = "UBIK Cosine"), Category = "UBIK Utility")
    static float CosineRule(float Adjacent1, float Adjacent2, float Opposite);

    /** Exposes FQuat helper function FindBetweenNormals, which has will only have a singularity when the two vectors are pointing directly opposite.
    * Note: This may be preferable to using the standard FindLookatRotation, which has singularities directly Up and Down.
    **/
    UFUNCTION(BlueprintPure, meta = (DisplayName = "FindBetweenNormals", Keywords = "UBIK Cosine"), Category = "UBIK Utility")
    static FRotator FindBetweenNormals(const FVector& A, const FVector& B);
};
