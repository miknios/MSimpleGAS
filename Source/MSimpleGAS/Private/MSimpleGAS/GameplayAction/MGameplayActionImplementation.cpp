// Copyright (c) Miknios. All rights reserved.


#include "MSimpleGAS/GameplayAction/MGameplayActionImplementation.h"

void UMGameplayActionImplementation::TriggerGameplayActionImplementation_Implementation(AActor* OwnerActor)
{
}

#if WITH_EDITOR
bool UMGameplayActionImplementation::ImplementsGetWorld() const
{
	return true;
}
#endif

UWorld* UMGameplayActionImplementation::GetWorld() const
{
	return UObject::GetWorld();
}
