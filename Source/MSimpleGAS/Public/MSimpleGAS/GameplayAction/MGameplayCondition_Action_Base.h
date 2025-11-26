// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Condition/MGameplayCondition_Base.h"
#include "MGameplayCondition_Action_Base.generated.h"

class UMGameplayActionInstance;
class UMGameplayActionComponent;

/*
 * Gameplay Condition implementation specific to Gameplay Actions
 * Works the same, but has utility functions for getting Action Owner Actor, UMGameplayActionComponent and UMGameplayActionInstance of the action itself
 */
UCLASS(Abstract)
class MSIMPLEGAS_API UMGameplayCondition_Action_Base : public UMGameplayCondition_Base
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	AActor* GetActionOwnerActor() const;
	
	UFUNCTION(BlueprintCallable)
	UMGameplayActionComponent* GetActionComponent() const;
	
	UFUNCTION(BlueprintCallable)
	UMGameplayActionInstance* GetActionInstance() const;
};
