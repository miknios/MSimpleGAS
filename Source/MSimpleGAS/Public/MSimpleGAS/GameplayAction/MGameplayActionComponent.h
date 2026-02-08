// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MManualTimer.h"
#include "Subsystems/WorldSubsystem.h"
#include "MGameplayActionComponent.generated.h"

class UMGameplayActionImplementation;
struct FMManualTimer;
class UMGameplayCondition_Base;
class UMMultisourceLockableBool;
class UMGameplayActionAsset;

DECLARE_MULTICAST_DELEGATE(FMOnActionTriggeredSignatureNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMOnActionTriggeredSignature);

DECLARE_MULTICAST_DELEGATE(FMOnActionStartedSignatureNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMOnActionStartedSignature);

DECLARE_MULTICAST_DELEGATE(FMOnActionCancelledSignatureNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMOnActionCancelledSignature);

DECLARE_MULTICAST_DELEGATE(FMOnActionFinishedSignatureNative);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMOnActionFinishedSignature);

USTRUCT()
struct FMGameplayAction_TriggerEntry
{
	GENERATED_BODY()

	UPROPERTY(Transient, VisibleAnywhere)
	double RequestTime = 0;
};

UCLASS(BlueprintType)
class MSIMPLEGAS_API UMGameplayActionInstance : public UObject
{
	GENERATED_BODY()

	friend class UMGameplayActionComponent;

public:
	void Initialize(UMGameplayActionAsset* InActionAsset);

	// Call to request action trigger. This doesn't mean the action is going to start at this exact moment or that its going to start at all
	UFUNCTION(BlueprintCallable)
	void RequestActionTrigger();

	// Called from an outside source whenever we want to cancel an action.
	// Action implementation should listen to @OnActionCancelledDelegate to cancel itself and then call @MarkActionFinished so the action knows that it finished
	UFUNCTION(BlueprintCallable)
	void RequestActionCancel();

	UFUNCTION(BlueprintCallable)
	void AddActionLockSource(UObject* LockSource);

	UFUNCTION(BlueprintCallable)
	void RemoveActionLockSource(UObject* LockSource);

	UFUNCTION(BlueprintCallable)
	bool CanTriggerAction() const;

	UFUNCTION(BlueprintCallable)
	bool IsActionActive() const;

	// This should be called at the start of action handle code so the system knows we handled action trigger and action is active
	UFUNCTION(BlueprintCallable)
	void MarkActionStarted();

	// This should be called at the end of action handle code so the system knows we action is inactive
	UFUNCTION(BlueprintCallable)
	void MarkActionFinished();

	FMOnActionTriggeredSignatureNative OnActionTriggeredDelegateNative;
	FMOnActionStartedSignatureNative OnActionStartedDelegateNative;
	FMOnActionCancelledSignatureNative OnActionCancelledDelegateNative;
	FMOnActionFinishedSignatureNative OnActionFinishedDelegateNative;

	// Called when action was successfully triggered. This doesn't mean the action is started.
	UPROPERTY(BlueprintAssignable)
	FMOnActionTriggeredSignature OnActionTriggered;

	// Called when action has actually started. This doesn't need to happen right after action trigger.
	UPROPERTY(BlueprintAssignable)
	FMOnActionStartedSignature OnActionStartedDelegate;

	// Action implementation should listen to @OnActionCancelledDelegate to cancel itself and then call @MarkActionFinished so the action knows that it finished
	UPROPERTY(BlueprintAssignable)
	FMOnActionCancelledSignature OnActionCancelledDelegate;

	// Called action implementation decides that it finished the action
	UPROPERTY(BlueprintAssignable)
	FMOnActionFinishedSignature OnActionFinishedDelegate;

protected:
	void Tick(float DeltaTime);
	void TryTriggerAction_Internal();
	void TriggerAction_Internal();

	UFUNCTION()
	void OnMultisourceBoolValueChanged(bool bIsActiveNew);

	UFUNCTION()
	void OnConditionSourceChanged();

	UPROPERTY(Transient, VisibleAnywhere)
	UMGameplayActionAsset* ActionAsset;

	UPROPERTY(Transient, VisibleAnywhere)
	bool bActionActive;

	UPROPERTY(Transient, VisibleAnywhere)
	TObjectPtr<UMGameplayCondition_Base> Condition;

	UPROPERTY(Transient, VisibleAnywhere)
	TObjectPtr<UMGameplayActionImplementation> ActionImplementation;

	// Array of activation requests that happened when action was locked
	UPROPERTY(Transient, VisibleAnywhere)
	TArray<FMGameplayAction_TriggerEntry> TriggerRequests;

	UPROPERTY(Transient, VisibleAnywhere)
	FMManualTimer CooldownTimer;

	UPROPERTY(Transient)
	TObjectPtr<UMMultisourceLockableBool> MultisourceLockableBool;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MSIMPLEGAS_API UMGameplayActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMGameplayActionComponent();

	// UActorComponent
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~ UActorComponent

	UFUNCTION(BlueprintCallable)
	static void RequestGameplayActionOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable)
	static void MarkActionStartedOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BLueprintCallable)
	static void MarkActionFinishedOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsActionActiveOnActor(AActor* ActionOwnerActor, UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable)
	void RequestGameplayAction(UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable)
	UMGameplayActionInstance* FindOrAddActionInstance(UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable)
	bool IsActionActive(UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable)
	void MarkActionStarted(UMGameplayActionAsset* ActionAsset);

	UFUNCTION(BlueprintCallable)
	void MarkActionFinished(UMGameplayActionAsset* ActionAsset);

protected:
	UPROPERTY(Transient, VisibleAnywhere)
	TMap<FName, TObjectPtr<UMGameplayActionInstance>> ActionInstanceForActionName;
};
