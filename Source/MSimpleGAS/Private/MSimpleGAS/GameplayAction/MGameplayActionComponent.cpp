// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAction//MGameplayActionComponent.h"

#include "MSimpleGAS/GameplayAction/MGameplayActionAsset.h"
#include "MDebug.h"
#include "MMath.h"
#include "MMultisourceLockableBool.h"
#include "Condition/MGameplayCondition_Base.h"
#include "MSimpleGAS/GameplayAction/MGameplayActionImplementation.h"

// UMGameplayActionInstance
void UMGameplayActionInstance::Initialize(UMGameplayActionAsset* InActionAsset)
{
	ActionAsset = InActionAsset;

	MultisourceLockableBool = NewObject<UMMultisourceLockableBool>(this);
	MultisourceLockableBool->OnValueChangedDelegate.AddUniqueDynamic(this, &UMGameplayActionInstance::OnMultisourceBoolValueChanged);

	if (InActionAsset->Condition != nullptr)
	{
		Condition = NewObject<UMGameplayCondition_Base>(this, InActionAsset->Condition->GetClass(), TEXT("ActionCondition"),
		                                                RF_Transient | RF_Public, InActionAsset->Condition);

		Condition->OnConditionSourceChangedDelegate.AddDynamic(this, &UMGameplayActionInstance::OnConditionSourceChanged);
		Condition->ListenForChanges(GetWorld());
	}

	if (InActionAsset->ActionImplementation != nullptr)
	{
		ActionImplementation = NewObject<UMGameplayActionImplementation>(this, InActionAsset->ActionImplementation,
		                                                                 TEXT("ActionImplementation"), RF_Transient | RF_Public);
	}

	CooldownTimer = FMManualTimer(ActionAsset->CooldownTime);
	CooldownTimer.Complete();
}

void UMGameplayActionInstance::RequestActionTrigger()
{
	if (CanTriggerAction())
	{
		TriggerAction_Internal();
		return;
	}

	// Add trigger request if action can't be triggered at this time
	const UWorld* World = GetWorld();
	if (!ensureAlways(World != nullptr))
	{
		return;
	}

	const double RequestTime = World->GetRealTimeSeconds();
	TriggerRequests.Emplace(FMGameplayAction_TriggerEntry{RequestTime});
}

void UMGameplayActionInstance::AddActionLockSource(UObject* LockSource)
{
	if (!ensureAlways(IsValid(MultisourceLockableBool)))
	{
		return;
	}

	MultisourceLockableBool->AddBlockSource(LockSource);
}

void UMGameplayActionInstance::RemoveActionLockSource(UObject* LockSource)
{
	if (!ensureAlways(IsValid(MultisourceLockableBool)))
	{
		return;
	}

	MultisourceLockableBool->RemoveBlockSource(LockSource);
}

bool UMGameplayActionInstance::CanTriggerAction() const
{
	bool bCanTrigger = true;

	if (ensureAlways(IsValid(MultisourceLockableBool)))
	{
		bCanTrigger &= MultisourceLockableBool->IsActive();
	}

	if (IsValid(Condition))
	{
		bCanTrigger &= Condition->Evaluate(GetWorld());
	}

	const bool bCooldownActive = !CooldownTimer.IsCompleted();
	bCanTrigger &= !bCooldownActive;

	return bCanTrigger;
}

bool UMGameplayActionInstance::IsActionActive() const
{
	return bActionActive;
}

void UMGameplayActionInstance::MarkActionStarted()
{
	bActionActive = true;
	OnActionStartedDelegateNative.Broadcast();
	OnActionStartedDelegate.Broadcast();
}

void UMGameplayActionInstance::MarkActionFinished()
{
	CooldownTimer.Reset();
	bActionActive = false;
	OnActionFinishedDelegateNative.Broadcast();
	OnActionFinishedDelegate.Broadcast();
}

void UMGameplayActionInstance::RequestActionCancel()
{
	OnActionCancelledDelegateNative.Broadcast();
	OnActionCancelledDelegate.Broadcast();
}

void UMGameplayActionInstance::Tick(float DeltaTime)
{
	if (!ensureAlways(IsValid(ActionAsset)))
	{
		return;
	}

	[this, DeltaTime]
	{
		if (ActionAsset->CooldownTime <= 0.0f)
		{
			return;
		}

		if (CooldownTimer.Tick(DeltaTime))
		{
			TryTriggerAction_Internal();
		}
	}();
}

void UMGameplayActionInstance::TryTriggerAction_Internal()
{
	if (!CanTriggerAction())
	{
		return;
	}

	const bool bValidRequestBuffered = [this]
	{
		const UWorld* World = GetWorld();
		if (!ensureAlways(World != nullptr))
		{
			return false;
		}

		const double RealTimeCurrent = World->GetRealTimeSeconds();

		for (const FMGameplayAction_TriggerEntry& Entry : TriggerRequests)
		{
			if (MMath::TimeSince(RealTimeCurrent, Entry.RequestTime) < ActionAsset->BufferTime)
			{
				return true;
			}
		}

		return false;
	}();

	if (bValidRequestBuffered)
	{
		TriggerAction_Internal();
	}
}

void UMGameplayActionInstance::TriggerAction_Internal()
{
	TriggerRequests.Empty();

	if (ActionImplementation != nullptr)
	{
		[this]
		{
			const UMGameplayActionComponent* ComponentOwner = GetTypedOuter<UMGameplayActionComponent>();
			if (!ensureAlways(ComponentOwner != nullptr))
			{
				return;
			}

			AActor* ActorOwner = ComponentOwner->GetOwner();
			if (!ensureAlways(ActorOwner != nullptr))
			{
				return;
			}

			ActionImplementation->TriggerGameplayActionImplementation(ActorOwner);
		}();
	}

	OnActionTriggeredDelegateNative.Broadcast();
	OnActionTriggered.Broadcast();
}

void UMGameplayActionInstance::OnMultisourceBoolValueChanged(bool bIsActiveNew)
{
	TryTriggerAction_Internal();
}

void UMGameplayActionInstance::OnConditionSourceChanged()
{
	TryTriggerAction_Internal();
}

// ~ UMGameplayActionInstance


UMGameplayActionComponent::UMGameplayActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// UMGameplayActionComponent
void UMGameplayActionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& [ActionName, ActionInstance] : ActionInstanceForActionName)
	{
		if (!ensureAlways(ActionInstance != nullptr))
		{
			continue;
		}

		ActionInstance->Tick(DeltaTime);
	}
}

void UMGameplayActionComponent::RequestGameplayActionOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset)
{
	if (ActionOwnerActor == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't request Gameplay Action on Actor, because ActionOwnerActor is nullptr"),
		                       ActionOwnerActor);

		return;
	}

	if (ActionAsset == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't request Gameplay Action on Actor, because ActionAsset is nullptr"), ActionOwnerActor);

		return;
	}

	UMGameplayActionComponent* GameplayActionComponent = ActionOwnerActor->FindComponentByClass<UMGameplayActionComponent>();
	if (GameplayActionComponent == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't request Gameplay Action on Actor, because it doesn't have UMGameplayActionComponent"),
		                       ActionOwnerActor);

		return;
	}

	GameplayActionComponent->RequestGameplayAction(ActionAsset);
}

void UMGameplayActionComponent::MarkActionStartedOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset)
{
	if (ActionOwnerActor == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't mark Gameplay Action on Actor as finished, because ActionOwnerActor is nullptr"),
							   ActionOwnerActor);

		return;
	}

	UMGameplayActionComponent* GameplayActionComponent = ActionOwnerActor->FindComponentByClass<UMGameplayActionComponent>();
	if (GameplayActionComponent == nullptr)
	{
		M::Debug::LogUserError(
			LogTemp, TEXT("Can't mark Gameplay Action on Actor as finished, because it doesn't have UMGameplayActionComponent"),
			ActionOwnerActor);

		return;
	}

	GameplayActionComponent->MarkActionStarted(ActionAsset);
}

void UMGameplayActionComponent::MarkActionFinishedOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset)
{
	if (ActionOwnerActor == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't mark Gameplay Action on Actor as finished, because ActionOwnerActor is nullptr"),
		                       ActionOwnerActor);

		return;
	}

	UMGameplayActionComponent* GameplayActionComponent = ActionOwnerActor->FindComponentByClass<UMGameplayActionComponent>();
	if (GameplayActionComponent == nullptr)
	{
		M::Debug::LogUserError(
			LogTemp, TEXT("Can't mark Gameplay Action on Actor as finished, because it doesn't have UMGameplayActionComponent"),
			ActionOwnerActor);

		return;
	}

	GameplayActionComponent->MarkActionFinished(ActionAsset);
}

bool UMGameplayActionComponent::IsActionActiveOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset)
{
	if (ActionOwnerActor == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't check if Gameplay Action is active on Actor, because ActionOwnerActor is nullptr"),
		                       ActionOwnerActor);

		return false;
	}

	UMGameplayActionComponent* GameplayActionComponent = ActionOwnerActor->FindComponentByClass<UMGameplayActionComponent>();
	if (GameplayActionComponent == nullptr)
	{
		M::Debug::LogUserError(
			LogTemp, TEXT("Can't check if Gameplay Action is active on Actor, because it doesn't have UMGameplayActionComponent"),
			ActionOwnerActor);

		return false;
	}

	return GameplayActionComponent->IsActionActive(ActionAsset);
}

void UMGameplayActionComponent::RequestGameplayAction(UMGameplayActionAsset* ActionAsset)
{
	UMGameplayActionInstance* GameplayActionInstance = FindOrAddActionInstance(ActionAsset);
	if (GameplayActionInstance == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't request Gameplay Action, because it couldn't be created"), this);
		return;
	}

	GameplayActionInstance->RequestActionTrigger();
}

UMGameplayActionInstance* UMGameplayActionComponent::FindOrAddActionInstance(UMGameplayActionAsset* ActionAsset)
{
	if (ActionAsset == nullptr)
	{
		M::Debug::LogUserError(LogTemp, TEXT("Can't find or add Gameplay Action Instance, because ActionAsset is nullptr"), GetOwner());
		return nullptr;
	}

	const TObjectPtr<UMGameplayActionInstance>* GameplayActionInstancePtrPtr =
		ActionInstanceForActionName.Find(ActionAsset->ActionName);

	if (GameplayActionInstancePtrPtr == nullptr)
	{
		UMGameplayActionInstance* NewInstance = NewObject<UMGameplayActionInstance>(this);
		if (!ensureAlways(NewInstance != nullptr))
		{
			return nullptr;
		}

		ActionInstanceForActionName.Emplace(ActionAsset->ActionName, NewInstance);
		NewInstance->Initialize(ActionAsset);
	}

	return ActionInstanceForActionName[ActionAsset->ActionName];
}

bool UMGameplayActionComponent::IsActionActive(UMGameplayActionAsset* ActionAsset)
{
	UMGameplayActionInstance* GameplayActionInstance = FindOrAddActionInstance(ActionAsset);
	if (!ensureAlways(GameplayActionInstance != nullptr))
	{
		return false;
	}

	return GameplayActionInstance->IsActionActive();
}

void UMGameplayActionComponent::MarkActionStarted(UMGameplayActionAsset* ActionAsset)
{
	UMGameplayActionInstance* GameplayActionInstance = FindOrAddActionInstance(ActionAsset);
	if (!ensureAlways(GameplayActionInstance != nullptr))
	{
		return;
	}

	GameplayActionInstance->MarkActionStarted();
}

void UMGameplayActionComponent::MarkActionFinished(UMGameplayActionAsset* ActionAsset)
{
	UMGameplayActionInstance* GameplayActionInstance = FindOrAddActionInstance(ActionAsset);
	if (!ensureAlways(GameplayActionInstance != nullptr))
	{
		return;
	}

	GameplayActionInstance->MarkActionFinished();
}

// ~ UMGameplayActionComponent
