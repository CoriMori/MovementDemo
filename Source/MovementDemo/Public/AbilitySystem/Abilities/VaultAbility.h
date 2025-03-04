// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PlayerBase.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "AbilitySystem/PlayMontageAndWaitForEvent.h"
#include "VaultAbility.generated.h"

/**
 * Ability to allow players to vault over short objects with motion warping
 * Multiplayer supported
 */
UCLASS()
class MOVEMENTDEMO_API UVaultAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UVaultAbility();

	/** Actually activate ability, do not call this directly. Called by the GameplayAbilityBase */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	// runs when ability is first intitalized
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Vault the player
	void Vault(const FGameplayAbilityActorInfo* ActorInfo);

	//Reset values so the player can vault again
	void HandleVaultEnd();

	UFUNCTION(Server, Reliable)
	void UpdateServer();

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vault", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> VaultAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vault", meta = (AllowPrivateAccess = "true"))
	float VaultDetectionDistance = 180.0f;

	int32 VaultTraceForwardSpacing = 30;

	int32 VaultTraceDownwardSpacing = 50;

	TArray<FVector>TraceVaultPath();

	bool bCanWarp = false;

	FVector LandingLocation = FVector(0.0f, 0.0f, 20000.0f);

	TObjectPtr<APlayerBase> OwningPlayer;

};
