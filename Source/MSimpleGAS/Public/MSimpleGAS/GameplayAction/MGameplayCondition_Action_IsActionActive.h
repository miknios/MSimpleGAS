// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGameplayCondition_Action_Base.h"
#include "MGameplayCondition_Action_IsActionActive.generated.h"

class UMGameplayActionAsset;

/**
 * @class UMGameplayCondition_Action_IsActionActive
 * @brief Checks if a specific gameplay action is currently active.
 */
UCLASS()
class MSIMPLEGAS_API UMGameplayCondition_Action_IsActionActive : public UMGameplayCondition_Action_Base
{
	GENERATED_BODY()

public:
	// UMGameplayCondition_Action_Base
	virtual bool Evaluate_Impl_Implementation(const UWorld* World) override;
	virtual void ListenForChanges_Implementation(const UWorld* World) override;
	virtual void StopListeningForChanges_Implementation(const UWorld* World) override;
	virtual FString GetConditionDescriptionString_Implementation() const override;
	// ~ UMGameplayCondition_Action_Base

protected:
	UFUNCTION()
	void OnActionStarted();
	
	UFUNCTION()
	void OnActionFinished();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMGameplayActionAsset> ActionAsset;
};
