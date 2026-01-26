// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MGameplayActionAsset.generated.h"

class UMGameplayActionImplementation;
class UMGameplayCondition_Base;

UCLASS(BlueprintType)
class MSIMPLEGAS_API UMGameplayActionAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActionName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BufferTime = 0.3f;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMGameplayCondition_Base> Condition = nullptr;
	
	// Assign implementation of Gameplay Action if all the logic can be contained within a single Blueprint class for easier setup
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMGameplayActionImplementation> ActionImplementation = nullptr;
};
