// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAttribute/MGameplayAttributeComponent.h"

#include "MDebug.h"
#include "MSimpleGAS/GameplayAttribute/MGameplayAttributeSetAsset.h"
#include "MSimpleGAS/GameplayAttribute/MGameplayAttribute_Base.h"

UMGameplayAttributeComponent::UMGameplayAttributeComponent()
{
}

void UMGameplayAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsureInitialized();
}

UMGameplayAttributeComponent* UMGameplayAttributeComponent::GetAttributeComponent(AActor* Actor)
{
	if (Actor == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Trying to get Attribute Component on nullptr Actor"));
		return nullptr;
	}

	return Actor->FindComponentByClass<UMGameplayAttributeComponent>();
}

void UMGameplayAttributeComponent::EnsureInitialized()
{
	if (bInitialized)
	{
		return;
	}

	bInitialized = true;
	if (InitialGameplayAttributeSet == nullptr)
	{
		return;
	}

	for (UMGameplayAttribute_Base* AttributeTemplate : InitialGameplayAttributeSet->Attributes)
	{
		UMGameplayAttribute_Base* AttributeInstance = NewObject<UMGameplayAttribute_Base>(
			this, AttributeTemplate->GetClass(), FName(AttributeTemplate->GetName()), RF_Transient | RF_Public, AttributeTemplate);
		AttributeInstanceForClass.Add(AttributeInstance->GetClass(), AttributeInstance);
	}
}

UMGameplayAttribute_Base* UMGameplayAttributeComponent::FindAttributeInstance(const TSubclassOf<UMGameplayAttribute_Base> AttributeClass)
{
	EnsureInitialized();

	if (AttributeClass == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Trying to get Attribute but provided attribute class in null"), GetOwner());
		return nullptr;
	}

	const bool bContainsAttribute = AttributeInstanceForClass.Contains(AttributeClass);
	if (!bContainsAttribute)
	{
#if WITH_EDITOR
		M::Debug::LogUserError(
			LogTemp, FString::Printf(
				TEXT("Trying to get Attribute with class %s, but it's not added to Gameplay Attribute Component"),
				*AttributeClass->GetDisplayNameText().ToString()), GetOwner());
#endif

		return nullptr;
	}

	return AttributeInstanceForClass[AttributeClass];
}
