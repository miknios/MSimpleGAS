# MSimpleGAS

A simplified Gameplay Ability System (GAS) inspired framework for Unreal Engine, designed for defining actions, applying statuses, and managing attributes. This plugin provides a flexible system for handling gameplay actions that can be started and stopped from outside the action implementation, making it more adaptable than traditional ability systems.

## Features

- **Gameplay Actions**: Define reusable actions with cooldowns, conditions, and custom implementations.
- **Gameplay Attributes**: Manage character stats and values with base and modified values.
- **Conditions System**: Evaluate prerequisites for actions using a flexible condition framework.
- **Blueprint Integration**: Fully integrated with Blueprints for easy setup and customization.
- **External Control**: Actions can be triggered, cancelled, and managed from external sources.

## Components Overview

MSimpleGAS consists of two main subsystems:

### Gameplay Actions
- **UMGameplayActionComponent**: Manages actions on an actor, handles triggering, starting, cancelling, and finishing actions.
- **UMGameplayActionAsset**: Data asset defining action properties like name, cooldown, buffer time, conditions, and implementation.
- **UMGameplayActionImplementation**: Abstract base class for implementing action logic.
- **UMGameplayActionInstance**: Runtime instance of an action, tracks state and handles lifecycle.
- **Conditions**: Evaluate whether an action can be triggered.

### Gameplay Attributes
- **UMGameplayAttributeComponent**: Manages attribute instances on an actor.
- **UMGameplayAttributeSetAsset**: Data asset containing initial attribute definitions.
- **UMGameplayAttribute_Base**: Abstract base class for all attributes.
- **UMGameplayAttribute_FloatBase**: Base class for float-based attributes with base and current values.
- **UMFloatSource_GameplayAttribute**: Float source that reads from gameplay attributes.

## Detailed Components

### Gameplay Actions

#### UMGameplayActionComponent
The core component attached to actors to manage their actions. Provides static functions for global action control and instance methods for component-specific operations.

Key functions:
- `RequestGameplayAction()`: Request to trigger an action.
- `RequestGameplayActionCancel()`: Cancel a running action.
- `MarkActionStarted()` / `MarkActionFinished()`: Mark action lifecycle states.
- `IsActionActive()`: Check if an action is currently running.

#### UMGameplayActionAsset
Blueprintable data asset that defines an action's properties:
- **ActionName**: Unique identifier for the action.
- **CooldownTime**: Time before the action can be triggered again.
- **BufferTime**: Window for action requests during cooldown and when it's blocked by defined conditions.
- **Condition**: Prerequisite condition for triggering.
- **ActionImplementation**: Blueprint class implementing the action logic.

#### UMGameplayActionImplementation
Allows to optionally have action implementation logic defined in the asset itself. Override `TriggerGameplayActionImplementation()` to define what happens when the action is triggered.

#### UMGameplayActionInstance
Runtime representation of an action. Handles:
- Trigger requests and lifecycle management.
- Cooldown and buffer timing.
- Action locking to prevent concurrent execution.
- Delegate broadcasting for action events.

#### Conditions
Conditions determine if an action can be executed. Extend `UMGameplayCondition_Action_Base` for action-specific conditions.

Built-in condition: `UMGameplayCondition_Action_IsActionActive` - checks if another action is currently active.

### Gameplay Attributes

#### UMGameplayAttributeComponent
Manages all attributes for an actor. Automatically initializes attributes from a `UMGameplayAttributeSetAsset`.

Key functions:
- `FindAttributeInstance()`: Get an attribute instance by class.
- `EnsureInitialized()`: Force initialization of attributes.

#### UMGameplayAttributeSetAsset
Data asset containing an array of initial attribute instances to add to actors.

#### UMGameplayAttribute_Base
Base class for all attributes. Provides:
- Initialization state tracking.
- `OnAttributeChangedDelegate`: Broadcasts when the attribute changes.

#### UMGameplayAttribute_FloatBase
Specialized for float attributes:
- **BaseValue**: The unmodified value.
- **CurrentValue**: Base value with modifiers applied (framework for future modifier system).
- Static functions for easy access: `GetCurrentAttributeValueFloat()`, `SetAttributeBaseValueFloat()`.

#### UMFloatSource_GameplayAttribute
Integrates with the MUtility float source system, allowing attributes to be used as float sources in other systems.

## Usage Examples

### Setting Up a Basic Action

1. Create a `UMGameplayActionAsset` in the Content Browser.
2. Set the ActionName (e.g., "Jump").
3. Configure CooldownTime and BufferTime.
4. Create a Blueprint class inheriting from `UMGameplayActionImplementation`.
5. Override `TriggerGameplayActionImplementation` to implement the jump logic.
6. Assign the implementation class to the asset.

### Using Actions in Blueprints

```blueprint
// Trigger an action
UMGameplayActionComponent::RequestGameplayActionOnActor(MyActor, MyActionAsset);

// Check if action is active
bool bIsActive = UMGameplayActionComponent::IsActionActiveOnActor(MyActor, MyActionAsset);

// Cancel an action
UMGameplayActionComponent::RequestGameplayActionCancelOnActor(MyActor, MyActionAsset);
```

### Setting Up Attributes

1. Create a `UMGameplayAttributeSetAsset`.
2. Add instances of attribute classes (e.g., `UMyHealthAttribute`).
3. Attach `UMGameplayAttributeComponent` to your actor.
4. Assign the attribute set to the component's `InitialGameplayAttributeSet`.

### Creating a Custom Float Attribute

```cpp
UCLASS()
class UMyHealthAttribute : public UMGameplayAttribute_FloatBase
{
    GENERATED_BODY()
    
    // Add custom logic here
};
```

### Using Attributes in Blueprints

```blueprint
// Get current health
float Health = UMGameplayAttribute_FloatBase::GetCurrentAttributeValueFloat(MyActor, UMyHealthAttribute::StaticClass());

// Set base health
UMGameplayAttribute_FloatBase::SetAttributeBaseValueFloat(MyActor, UMyHealthAttribute::StaticClass(), 100.0f);
```

### Action Implementation Example

In your `UMGameplayActionImplementation` Blueprint:

1. Bind to the action instance's delegates in `TriggerGameplayActionImplementation`.
2. Call `MarkActionStarted` when the action begins.
3. Perform action logic (e.g., play animation, apply effects).
4. Call `MarkActionFinished` when complete.

```blueprint
void UMyJumpActionImplementation::TriggerGameplayActionImplementation(AActor* OwnerActor)
{
    // Get the action instance (assuming it's passed or accessible)
    ActionInstance->OnActionCancelledDelegate.AddDynamic(this, &UMyJumpActionImplementation::OnActionCancelled);
    
    // Start the action
    ActionInstance->MarkActionStarted();
    
    // Perform jump logic
    OwnerActor->LaunchCharacter(FVector(0, 0, 500), false, true);
    
    // Simulate action duration
    FTimerHandle TimerHandle;
    OwnerActor->GetWorldTimerManager().SetTimer(TimerHandle, this, &UMyJumpActionImplementation::FinishJump, 1.0f, false);
}

void UMyJumpActionImplementation::FinishJump()
{
    ActionInstance->MarkActionFinished();
}

void UMyJumpActionImplementation::OnActionCancelled()
{
    // Handle cancellation (e.g., stop jump)
    ActionInstance->MarkActionFinished();
}
```


## Dependencies

- **MUtility**: Required for utility functions and base classes.

