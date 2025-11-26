// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAttribute/MGameplayAttribute_FloatBase.h"

#include "MDebug.h"
#include "MSimpleGAS/GameplayAttribute/MGameplayAttributeComponent.h"

UMGameplayAttribute_FloatBase* UMGameplayAttribute_FloatBase::GetAttributeFloat(
	AActor* Actor,
	const TSubclassOf<UMGameplayAttribute_FloatBase> InAttributeClass)
{
	UMGameplayAttributeComponent* AttributeComponent = UMGameplayAttributeComponent::GetAttributeComponent(Actor);
	UMGameplayAttribute_FloatBase* Result =
		AttributeComponent->FindAttributeInstanceCasted<UMGameplayAttribute_FloatBase>(InAttributeClass);
	return Result;
}

float UMGameplayAttribute_FloatBase::GetCurrentAttributeValueFloat(
	AActor* Actor,
	const TSubclassOf<UMGameplayAttribute_FloatBase> InAttributeClass)
{
	const UMGameplayAttribute_FloatBase* Attribute = GetAttributeFloat(Actor, InAttributeClass);
	if (Attribute == nullptr)
	{
		return 0.0f;
	}

	return Attribute->GetCurrentValue();
}

void UMGameplayAttribute_FloatBase::SetAttributeBaseValueFloat(
	AActor* Actor,
	const TSubclassOf<UMGameplayAttribute_FloatBase> InAttributeClass,
	const float NewValue)
{
	UMGameplayAttribute_FloatBase* Attribute = GetAttributeFloat(Actor, InAttributeClass);
	if (Attribute == nullptr)
	{
		return;
	}

	Attribute->SetBaseValue(NewValue);
}

void UMGameplayAttribute_FloatBase::Initialize(const float InitialValue)
{
	BaseValue = InitialValue;
	bInitialized = true;
}

void UMGameplayAttribute_FloatBase::SetBaseValue(const float NewValue)
{
	if (!IsInitialized())
	{
		M::Debug::LogUserError(LogTemp, TEXT(
			                       "Can't set Base Value, because attribute is not initialized. Make sure to call Initialize with initial Attribute value"),
		                       this);
	}

	BaseValue = NewValue;
	OnAttributeChangedDelegate.Broadcast();
}

// TODO: calculate current value based on modifiers
float UMGameplayAttribute_FloatBase::GetCurrentValue() const
{
	if (!IsInitialized())
	{
		M::Debug::LogUserError(LogTemp, TEXT(
			                       "Trying to get Current Value, but attribute is not initialized. Make sure to call Initialize with initial Attribute value"),
		                       this);
	}

	return BaseValue;
}

float UMGameplayAttribute_FloatBase::GetBaseValue() const
{
	if (!IsInitialized())
	{
		M::Debug::LogUserError(LogTemp, TEXT(
			                       "Trying to get Base value, but attribute is not initialized. Make sure to call Initialize with initial Attribute value"),
		                       this);
	}

	return BaseValue;
}
