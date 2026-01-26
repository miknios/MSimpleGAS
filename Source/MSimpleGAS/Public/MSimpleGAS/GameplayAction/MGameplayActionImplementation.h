// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MGameplayActionImplementation.generated.h"

/**
 * Implementation of Gameplay Action that can be assigned directly to the gameplay action asset
 */
UCLASS(Abstract, Blueprintable)
class MSIMPLEGAS_API UMGameplayActionImplementation : public UObject
{
	GENERATED_BODY()

public:
	// UObject
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override;
#endif
	virtual UWorld* GetWorld() const override;
	// ~ UObject

	UFUNCTION(BlueprintNativeEvent)
	void TriggerGameplayActionImplementation(AActor* OwnerActor);
};
