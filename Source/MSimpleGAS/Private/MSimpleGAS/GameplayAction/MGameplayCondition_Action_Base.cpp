// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAction/MGameplayCondition_Action_Base.h"

#include "MSimpleGAS/GameplayAction/MGameplayActionComponent.h"

AActor* UMGameplayCondition_Action_Base::GetActionOwnerActor() const
{
	UMGameplayActionComponent* GameplayActionComponent = GetActionComponent();
	if (GameplayActionComponent == nullptr)
	{
		return nullptr;
	}

	return GameplayActionComponent->GetOwner();
}

UMGameplayActionComponent* UMGameplayCondition_Action_Base::GetActionComponent() const
{
	UMGameplayActionComponent* GameplayActionComponent = GetTypedOuter<UMGameplayActionComponent>();
	ensureAlways(GameplayActionComponent != nullptr);
	return GameplayActionComponent;
}

UMGameplayActionInstance* UMGameplayCondition_Action_Base::GetActionInstance() const
{
	UMGameplayActionInstance* ActionInstance = GetTypedOuter<UMGameplayActionInstance>();
	ensureAlways(ActionInstance != nullptr);
	
	return ActionInstance;
}
