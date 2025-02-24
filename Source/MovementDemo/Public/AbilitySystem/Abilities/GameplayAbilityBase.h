// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

/**
 * Custom Gameplay Ability Base
 * Allows for Enhanced Input Hookup
 */
UCLASS()
class MOVEMENTDEMO_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbilityBase();

	// return the avatar associated with this ability
	// will return null if avatar isnt derived from character
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACharacter* GetAvatarCharacter() const { return Avatar.Get(); }

	void SetActivateAbilityOnGranted(bool bValue) { bActivateAbilityOnGranted = bValue; }
	bool GetActivateAbilityOnGranted() const { return bActivateAbilityOnGranted; }

protected:
	// Keep a pointer to the avatar so we don't have to cast to Character in instanced abilities owned by a Character derived class
	TWeakObjectPtr<ACharacter> Avatar = nullptr;

	// runs when ability is first intitalized
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Called to bind Input Pressed and Input Released events to the Avatar Actor's Enhanced Input Component if it is reachable. 
	void SetupEnhancedInputBindings(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec);

	void HandleInputPressed(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle);

	void HandleInputReleased(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpecHandle SpecHandle);

	// Override "OnRemoveAbility" to clean up Enhanced Input Bindings.
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

private:
	UEnhancedInputComponent* GetAvatarInputComponent(const FGameplayAbilityActorInfo* ActorInfo);

	// Tells an ability to activate immediately when it's granted
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom Gameplay Ability", meta = (AllowPrivateAccess = "true"))
	bool bActivateAbilityOnGranted = false;

	// bind ativation to an input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ActivationInputAction = nullptr;

	// input action trigger type on trigger
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	ETriggerEvent InputTypeTrigger = ETriggerEvent::Started;

	// input action trigger type on release
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	ETriggerEvent InputTypeReleased = ETriggerEvent::Completed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	bool bRequireUserConfirmation = false;

	// bind ativation to an input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true", EditCondition = "bRequireUserConfirmation"))
	TObjectPtr<UInputAction> ConfirmInputAction = nullptr;

	// bind ativation to an input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true", EditCondition = "bRequireUserConfirmation"))
	TObjectPtr<UInputAction> CancelInputAction = nullptr;
};
