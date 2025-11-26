// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MGameplayAttributeSetAsset.generated.h"

class UMGameplayAttribute_Base;

/**
 * Data Asset defining what base attributes an actor has from the start 
 */
UCLASS()
class MSIMPLEGAS_API UMGameplayAttributeSetAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(Instanced, EditAnywhere)
	TArray<TObjectPtr<UMGameplayAttribute_Base>> Attributes;
};
