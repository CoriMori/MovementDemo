// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"

UGameplayAbilityBase::UGameplayAbilityBase()
{
	//set instance per actor for replication purposes
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	//set Avatar ref
	Avatar = Cast<ACharacter>(ActorInfo->AvatarActor);

	//Set up enhanced input binding
	SetupEnhancedInputBindings(ActorInfo, Spec);

	if (bActivateAbilityOnGranted) {
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UGameplayAbilityBase::SetupEnhancedInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	//make sure we have all the relevant components required for input
	UEnhancedInputComponent* InputComponent = GetAvatarInputComponent(ActorInfo);
	if (InputComponent) {
		UGameplayAbilityBase* AbilityInstance = Cast<UGameplayAbilityBase>(Spec.Ability.Get());
		if (AbilityInstance) {
			//check if the activation input action is valid
			if (IsValid(AbilityInstance->ActivationInputAction)) {
				if (bRequireUserConfirmation) {
					if (ConfirmInputAction && CancelInputAction) {
						InputComponent->BindAction(
							AbilityInstance->ConfirmInputAction,
							ETriggerEvent::Triggered,
							GetAbilitySystemComponentFromActorInfo(),
							&UAbilitySystemComponent::LocalInputConfirm
						);

						InputComponent->BindAction(
							AbilityInstance->CancelInputAction,
							ETriggerEvent::Triggered,
							GetAbilitySystemComponentFromActorInfo(),
							&UAbilitySystemComponent::LocalInputCancel
						);
					}

					UE_LOG(LogTemp, Warning, TEXT("Confirm and Cancel input actions are both required for User Confirmation. Please ensure both Input Actions have been set."))

				}
				//bind pressed event we have one
				if (InputTypeTrigger != ETriggerEvent::None) {
					InputComponent->BindAction(
						AbilityInstance->ActivationInputAction, 
						AbilityInstance->InputTypeTrigger, 
						AbilityInstance, 
						&ThisClass::HandleInputPressed, 
						ActorInfo, 
						Spec.Handle
					);
				}

				//bind released event if we have one
				if (InputTypeTrigger != ETriggerEvent::None) {
					InputComponent->BindAction(
						AbilityInstance->ActivationInputAction, 
						AbilityInstance->InputTypeReleased, 
						AbilityInstance, 
						&ThisClass::HandleInputReleased, 
						ActorInfo, 
						Spec.Handle
					);
				}
			}
		}
	}
	
}

void UGameplayAbilityBase::HandleInputPressed(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{
	// Find the Ability Spec based on the passed in information and set a reference.
	FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	if (!Spec || !Spec->Ability || !ActorInfo->AbilitySystemComponent.IsValid()) return;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return;

	Spec->InputPressed = true;
	if (!Spec->IsActive()) {
		// Ability is not active, so try to activate it
		ASC->TryActivateAbility(SpecHandle);
		return;
	}
	if (Spec->Ability->bReplicateInputDirectly && !ASC->IsOwnerActorAuthoritative()) {
		ASC->ServerSetInputPressed(Spec->Ability.Get()->GetCurrentAbilitySpecHandle());
	}

	ASC->AbilitySpecInputPressed(*Spec);

	// Invoke the InputPressed event
	ASC->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec->Handle, Spec->ActivationInfo.GetActivationPredictionKey());
}

void UGameplayAbilityBase::HandleInputReleased(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle)
{

	// Find the Ability Spec based on the passed in information and set a reference.
	FGameplayAbilitySpec* Spec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(SpecHandle);
	if (!Spec || !Spec->Ability || !ActorInfo->AbilitySystemComponent.IsValid()) return;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return;

	Spec->InputPressed = false;

	if (Spec->IsActive()) {
		if (Spec->Ability->bReplicateInputDirectly && !ASC->IsOwnerActorAuthoritative()) {
			ASC->ServerSetInputReleased(SpecHandle);
		}

		ASC->AbilitySpecInputReleased(*Spec);

		// Invoke the InputReleased event
		ASC->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle, Spec->ActivationInfo.GetActivationPredictionKey());
	}
}

void UGameplayAbilityBase::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	UEnhancedInputComponent* InputComponent = GetAvatarInputComponent(ActorInfo);

	if (InputComponent) {
		// clear bindings
		InputComponent->ClearBindingsForObject(Spec.Ability.Get());
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}


UEnhancedInputComponent* UGameplayAbilityBase::GetAvatarInputComponent(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!IsValid(ActorInfo->AvatarActor.Get())) return nullptr;
	
	const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!AvatarPawn) return nullptr;

	const AController* PawnController = AvatarPawn->GetController();
	if (!PawnController) return nullptr;

	return Cast<UEnhancedInputComponent>(PawnController->InputComponent.Get());
}


