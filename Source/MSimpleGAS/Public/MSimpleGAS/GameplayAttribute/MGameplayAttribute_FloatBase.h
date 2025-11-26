// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGameplayAttribute_Base.h"
#include "MGameplayAttribute_FloatBase.generated.h"

/**
 * Base class for implementing float-based attributes
 */
UCLASS(Abstract)
class MSIMPLEGAS_API UMGameplayAttribute_FloatBase : public UMGameplayAttribute_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	static UMGameplayAttribute_FloatBase* GetAttributeFloat(AActor* Actor, const TSubclassOf<UMGameplayAttribute_FloatBase> InAttributeClass);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	static float GetCurrentAttributeValueFloat(AActor* Actor, const TSubclassOf<UMGameplayAttribute_FloatBase> InAttributeClass);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	static void SetAttributeBaseValueFloat(AActor* Actor, const TSubclassOf<UMGameplayAttribute_FloatBase> InAttributeClass, const float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	void Initialize(const float InitialValue);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	void SetBaseValue(const float NewValue);

	// The current value is a Base Value with all modifiers applied
	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	float GetCurrentValue() const;

	// Base Value is a value without any modifiers applied to it
	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	float GetBaseValue() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayAttribute")
	float BaseValue = 0.0f;
};
