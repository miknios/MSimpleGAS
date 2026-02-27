// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAction/MGameplayCondition_Action_Base.h"

#include "MSimpleGAS/GameplayAction/MGameplayActionComponent.h"

const AActor* UMGameplayCondition_Action_Base::GetActionOwnerActor(const UObject* ContextObject) const
{
	const AActor* ContextActor = Cast<AActor>(ContextObject);
	if (!ensureAlways(ContextActor != nullptr))
	{
		return nullptr;
	}

	return ContextActor;
}

const UMGameplayActionComponent* UMGameplayCondition_Action_Base::GetActionComponent(const UObject* ContextObject) const
{
	const AActor* OwnerActor = GetActionOwnerActor(ContextObject);
	if (OwnerActor == nullptr)
	{
		return nullptr;
	}
	
	return OwnerActor->FindComponentByClass<UMGameplayActionComponent>();
}
