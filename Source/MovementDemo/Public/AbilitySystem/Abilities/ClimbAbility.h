// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "AbilitySystem/AbilityTaskOnTick.h"
#include "Components/TimelineComponent.h"
#include "ClimbAbility.generated.h"

/**
 * 
 */
class UCurveFloat;
UCLASS()
class MOVEMENTDEMO_API UClimbAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UClimbAbility();

	/** Actually activate ability, do not call this directly. Called by the GameplayAbilityBase */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
protected:
	// runs when ability is first intitalized
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void Climb();

	void AttachToWall();

	bool ClimbTrace(FHitResult& OutResult, float TraceDistance);

	//on tick event->abilities don't have tick by default so the ticking will occur via an ability task
	UFUNCTION()
	void OnTick(float DeltaTime);

	UFUNCTION()
	void SmoothClimbRotation(float Alpha);

	void EndClimb();
private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbSpeed = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbBreak = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbing", meta = (AllowPrivateAccess = "true"))
	float ClimbTraceDistance = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Climbing", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ClimbRoationCurve;

	FTimeline ClimbRotationTimeline;

};
