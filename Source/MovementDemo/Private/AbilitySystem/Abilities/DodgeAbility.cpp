// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DodgeAbility.h"
#include "GameFramework/Character.h"
#include "Player/PlayerBase.h"

UDodgeAbility::UDodgeAbility()
{
	EGameplayAbilityReplicationPolicy::ReplicateYes; //allows RPCs to fire on this ability
}

void UDodgeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Dodge();
}

void UDodgeAbility::Dodge()
{
	if (!DodgeAnimation || bIsDodging) return;

	bIsDodging = true;
	//play animation montage and bind events
	UPlayMontageAndWaitForEvent* MontageTask = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, DodgeAnimation, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &UDodgeAbility::OnCompleted);

	MontageTask->ReadyForActivation();
}

void UDodgeAbility::UpdateServer_Implementation()
{
	bIsDodging = false;
}


void UDodgeAbility::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	bIsDodging = false;
	UpdateServer();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDodgeAbility::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	bIsDodging = false;
	UpdateServer();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
