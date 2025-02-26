// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "ClimbAbility.generated.h"

/**
 * 
 */
UCLASS()
class MOVEMENTDEMO_API UClimbAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UClimbAbility();

	/** Actually activate ability, do not call this directly. Called by the GameplayAbilityBase */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	// runs when ability is first intitalized
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void Climb();

	void AttachToWall();

	bool ClimbTrace(FHitResult& OutResult, float TraceDistance);

private:

	float ClimbSpeed = 350.0f;

	float ClimbBreak = 1000.0f;

	float ClimbTraceDistance = 100.0f;
	
};
