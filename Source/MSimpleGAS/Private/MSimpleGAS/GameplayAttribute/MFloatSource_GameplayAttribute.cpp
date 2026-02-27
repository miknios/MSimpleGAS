// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAttribute/MFloatSource_GameplayAttribute.h"

#include "MDebug.h"
#include "MSimpleGAS/GameplayAttribute/MGameplayAttribute_FloatBase.h"

float UMFloatSource_GameplayAttribute::GetFloatValue_Impl_Implementation(const UObject* ContextObject) const
{
	const AActor* OwnerActorConst = Cast<AActor>(ContextObject);
	if (OwnerActorConst == nullptr)
	{
		M::Debug::LogUserError(
			LogTemp,
			TEXT(
				"Trying to get Float Value from Gameplay Attribute Source, but Float Source context object is not an Actor"),
			ContextObject);
		return 0.f;
	}

	AActor* OwnerActor = const_cast<AActor*>(OwnerActorConst);
	const UMGameplayAttribute_FloatBase* AttributeFloat = UMGameplayAttribute_FloatBase::GetAttributeFloat(OwnerActor, AttributeClass);
	if (AttributeFloat == nullptr)
	{
		return 0.f;
	}

	const float Result = AttributeFloat->GetCurrentValue();
	return Result;
}

void UMFloatSource_GameplayAttribute::ListenForChanges_Impl_Implementation(const UObject* ContextObject)
{
	const AActor* OwnerActorConst = Cast<AActor>(ContextObject);
	if (OwnerActorConst == nullptr)
	{
		return;
	}

	AActor* OwnerActor = const_cast<AActor*>(OwnerActorConst);
	UMGameplayAttribute_FloatBase* AttributeFloat = UMGameplayAttribute_FloatBase::GetAttributeFloat(OwnerActor, AttributeClass);
	if (AttributeFloat == nullptr)
	{
		return;
	}

	AttributeFloat->OnAttributeChangedDelegate.AddUniqueDynamic(this, &UMFloatSource_GameplayAttribute::OnAttributeChanged);
}

void UMFloatSource_GameplayAttribute::StopListeningForChanges_Impl_Implementation(const UObject* ContextObject)
{
	const AActor* OwnerActorConst = Cast<AActor>(ContextObject);
	if (OwnerActorConst == nullptr)
	{
		return;
	}

	AActor* OwnerActor = const_cast<AActor*>(OwnerActorConst);
	UMGameplayAttribute_FloatBase* AttributeFloat = UMGameplayAttribute_FloatBase::GetAttributeFloat(OwnerActor, AttributeClass);
	if (AttributeFloat == nullptr)
	{
		return;
	}

	AttributeFloat->OnAttributeChangedDelegate.RemoveDynamic(this, &UMFloatSource_GameplayAttribute::OnAttributeChanged);
}

void UMFloatSource_GameplayAttribute::OnAttributeChanged()
{
	BroadcastOnValueChanged();
}
