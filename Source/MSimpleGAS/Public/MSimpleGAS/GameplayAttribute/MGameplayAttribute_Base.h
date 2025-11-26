// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MGameplayAttribute_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMOnAttributeChangedSignature);

/**
 * Base class for implementing Gameplay Attribute
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class MSIMPLEGAS_API UMGameplayAttribute_Base : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	bool IsInitialized() const;

	UPROPERTY(BlueprintAssignable)
	FMOnAttributeChangedSignature OnAttributeChangedDelegate;

protected:
	UPROPERTY(EditAnywhere, Category = "GameplayAttribute")
	bool bInitialized = false;
};
