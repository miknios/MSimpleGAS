// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MFloatSource.h"
#include "UObject/Object.h"
#include "MFloatSource_GameplayAttribute.generated.h"

class UMGameplayAttribute_FloatBase;

UCLASS(NotBLueprintType, DisplayName = "Gameplay Attribute")
class MSIMPLEGAS_API UMFloatSource_GameplayAttribute : public UMFloatSource_Base
{
	GENERATED_BODY()

public:
	// UMFloatSource_Base
	virtual float GetFloatValue_Implementation() const override;
	virtual void ListenForChanges_Implementation() override;
	virtual void StopListeningForChanges_Implementation() override;
	// ~ UMFloatSource_Base

protected:
	UFUNCTION()
	void OnAttributeChanged();

	UPROPERTY(EditAnywhere, Category = "Float Source")
	TSubclassOf<UMGameplayAttribute_FloatBase> AttributeClass;
};
