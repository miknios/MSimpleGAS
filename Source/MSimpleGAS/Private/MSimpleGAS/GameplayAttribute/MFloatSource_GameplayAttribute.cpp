// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAttribute/MFloatSource_GameplayAttribute.h"

#include "MDebug.h"
#include "MSimpleGAS/GameplayAttribute/MGameplayAttribute_FloatBase.h"

float UMFloatSource_GameplayAttribute::GetFloatValue_Implementation(UObject* ContextObject) const
{
	AActor* OwnerActor = [this, ContextObject]
	{
		if (ContextObject != nullptr)
		{
			if (AActor* ContextActor = Cast<AActor>(ContextObject))
			{
				return ContextActor;
			}

			return ContextObject->GetTypedOuter<AActor>();
		}

		return GetTypedOuter<AActor>();
	}();

	if (OwnerActor == nullptr)
	{
		M::Debug::LogUserError(
			LogTemp,
			TEXT(
				"Trying to get Float Value from Gameplay Attribute Source, but Actor cannot be found in Float Source outer chain or provided Context Object outer chain"),
			ContextObject);
		return 0.f;
	}

	const UMGameplayAttribute_FloatBase* AttributeFloat =
		UMGameplayAttribute_FloatBase::GetAttributeFloat(OwnerActor, AttributeClass);
	if (AttributeFloat == nullptr)
	{
		return 0.f;
	}

	const float Result = AttributeFloat->GetCurrentValue();
	return Result;
}

void UMFloatSource_GameplayAttribute::ListenForChanges_Implementation()
{
	AActor* OwnerActor = GetTypedOuter<AActor>();
	if (OwnerActor == nullptr)
	{
		return;
	}

	UMGameplayAttribute_FloatBase* AttributeFloat =
		UMGameplayAttribute_FloatBase::GetAttributeFloat(OwnerActor, AttributeClass);
	if (AttributeFloat == nullptr)
	{
		return;
	}

	AttributeFloat->OnAttributeChangedDelegate.AddUniqueDynamic(this, &UMFloatSource_GameplayAttribute::OnAttributeChanged);
}

void UMFloatSource_GameplayAttribute::StopListeningForChanges_Implementation()
{
	AActor* OwnerActor = GetTypedOuter<AActor>();
	if (OwnerActor == nullptr)
	{
		return;
	}

	UMGameplayAttribute_FloatBase* AttributeFloat =
		UMGameplayAttribute_FloatBase::GetAttributeFloat(OwnerActor, AttributeClass);
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
