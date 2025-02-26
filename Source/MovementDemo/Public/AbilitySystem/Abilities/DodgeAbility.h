// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "AbilitySystem/PlayMontageAndWaitForEvent.h"
#include "DodgeAbility.generated.h"

/**
 * 
 */
UCLASS()
class MOVEMENTDEMO_API UDodgeAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UDodgeAbility();

	/** Actually activate ability, do not call this directly. Called by the GameplayAbilityBase */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	void Dodge();

	UFUNCTION(Server, Reliable)
	void UpdateServer();

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vault", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DodgeAnimation;

	bool bIsDodging = false;
};
