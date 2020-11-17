// 2020 Sticky Snout Studio (Jonas Molgaard)

#pragma once

#include "AnimGraphNode_SkeletalControlBase.h"
//#include "AnimGraphNode_Base.h"
#include "AnimNode_UBIKSolver.h"
#include "AnimGraphNode_UBIKSolver.generated.h"

/**
 * Class that expose the actual node to the AnimGraph. 
 * Here we have stuff like the title, color of node, tooltips etc.
 */
UCLASS(meta = (Keywords = "UBIK Inverse Kinematics Solve IK Upper Body"))
class UBIKEDITOR_API UAnimGraphNode_UBIKSolver : public UAnimGraphNode_SkeletalControlBase
//class UBIKEDITOR_API UAnimGraphNode_UBIKSolver : public UAnimGraphNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_UBIKSolver Node;
	
public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	/* Only valid if i base this off of FAnimNode_SkeletalControlBase */
	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface
};
