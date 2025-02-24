// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemComponentBase.h"

UAbilitySystemComponentBase::UAbilitySystemComponentBase()
{
	//set ASC to mixed replication mode -> replicates Gameplay Effects minimally to simulated proxies and full info to everyone else
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;

	//set component to replicate by default
	SetIsReplicatedByDefault(true);
}

void UAbilitySystemComponentBase::InitializeAbilitySystemData(const FAbilitySystemInitData& InitData, AActor* OwningActor, AActor* Avatar)
{
	if (bDataInit) return;
	
	bDataInit = true;

	//set owning actor and avatar actor
	InitAbilityActorInfo(OwningActor, Avatar);

	//check for authority
	if (GetOwnerActor()->HasAuthority()) {
		//grant attribute sets
		if (!InitData.AttributeSets.IsEmpty()) {
			for (const TSubclassOf<UAttributeSet> AtrSet : InitData.AttributeSets) {
				GetOrCreateAttributeSet(AtrSet);
			}
		}

		//set attribute base values
		if (!InitData.AttributeBaseValues.IsEmpty()) {
			for (const TTuple<FGameplayAttribute, float>& AtrBaseVal : InitData.AttributeBaseValues) {
				if (HasAttributeSetForAttribute(AtrBaseVal.Key)) {
					SetNumericAttributeBase(AtrBaseVal.Key, AtrBaseVal.Value);
				}
			}
		}
		
		//grant abilities
		if (!InitData.GameplayAbilities.IsEmpty()) {
			for (const TSubclassOf<UGameplayAbility> GPAbility : InitData.GameplayAbilities) {
				FGameplayAbilitySpec GPSpec = FGameplayAbilitySpec(GPAbility, 1, INDEX_NONE, this);
				GiveAbility(GPSpec);
			}
		}

		//apply effects
		if (!InitData.GameplayEffects.IsEmpty()) {
			for (const TSubclassOf<UGameplayEffect> GPEffect : InitData.GameplayEffects) {
				if (IsValid(GPEffect)) {
					FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext();
					EffectContextHandle.AddSourceObject(this);

					FGameplayEffectSpecHandle GPEffectSpecHandle = MakeOutgoingSpec(GPEffect, 1, EffectContextHandle);
					if (GPEffectSpecHandle.IsValid()) {
						ApplyGameplayEffectSpecToTarget(*GPEffectSpecHandle.Data.Get(), this);
					}
				}
			}
		}

		//apply tags -> not replicated by default should be applied to client and server respectively
		if (!InitData.GameplayTags.IsEmpty()) {
			AddLooseGameplayTags(InitData.GameplayTags);
		}
	}

}


const UAttributeSet* UAbilitySystemComponentBase::GetOrCreateAttributeSet(const TSubclassOf<UAttributeSet>& InAttributeSet)
{
	return GetOrCreateAttributeSubobject(InAttributeSet);
}
