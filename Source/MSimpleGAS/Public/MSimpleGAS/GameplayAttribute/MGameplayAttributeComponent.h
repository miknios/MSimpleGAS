// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MDebug.h"
#include "Components/ActorComponent.h"
#include "MGameplayAttributeComponent.generated.h"


class UMGameplayAttributeSetAsset;
class UMGameplayAttribute_Base;

USTRUCT(BlueprintType)
struct FMGameplayAttributeName
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetOptions = "MSimpleGAS.MGameplayAttributeLibrary.GetAttributeNamesAll"))
	FName AttributeName;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MSIMPLEGAS_API UMGameplayAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMGameplayAttributeComponent();

	// UActorComponent
	virtual void BeginPlay() override;
	// ~ UActorComponent

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	static UMGameplayAttributeComponent* GetAttributeComponent(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	void EnsureInitialized();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Attribute")
	UMGameplayAttribute_Base* FindAttributeInstance(const TSubclassOf<UMGameplayAttribute_Base> AttributeClass);

	template <typename T>
	T* FindAttributeInstanceCasted()
	{
		static_assert(std::is_base_of_v<UMGameplayAttribute_Base, T>,
		              "T must inherit from UMGameplayAttribute_Base");

		UMGameplayAttribute_Base* BasePtr = FindAttributeInstance(T::StaticClass());
		if (BasePtr == nullptr)
		{
			return nullptr;
		}

		T* AttributeInstance = Cast<T>(BasePtr);
		if (AttributeInstance == nullptr)
		{
			M::Debug::LogUserError(LogTemp, TEXT("Attribute instance is not of type T"), GetOwner());
			return nullptr;
		}

		return AttributeInstance;
	}

	// Allows to cast to base type but use different specific type for identifying attribute
	template <typename T>
	T* FindAttributeInstanceCasted(TSubclassOf<T> AttributeClass)
	{
		static_assert(std::is_base_of_v<UMGameplayAttribute_Base, T>,
		              "T must inherit from UMGameplayAttribute_Base");

		UMGameplayAttribute_Base* BasePtr = FindAttributeInstance(AttributeClass);
		if (BasePtr == nullptr)
		{
			return nullptr;
		}

		T* AttributeInstance = Cast<T>(BasePtr);
		if (AttributeInstance == nullptr)
		{
			M::Debug::LogUserError(LogTemp, TEXT("Attribute instance is not of type T"), GetOwner());
			return nullptr;
		}

		return AttributeInstance;
	}

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMGameplayAttributeSetAsset> InitialGameplayAttributeSet;

	// Contains all the attributes added to the component
	UPROPERTY(Transient, VisibleAnywhere)
	TMap<TObjectPtr<UClass>, TObjectPtr<UMGameplayAttribute_Base>> AttributeInstanceForClass;

	UPROPERTY(Transient, VisibleAnywhere)
	bool bInitialized;
};
