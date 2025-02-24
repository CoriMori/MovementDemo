// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/HealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UHealthAttributeSet::UHealthAttributeSet()
{
	MaxHealth = 0.0f;
	CurrentHealth = 0.0f;
	HealthRegen = 0.0f;
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetMaxHealthAttribute()) {
		OnMaxChange(CurrentHealth, MaxHealth, NewValue, GetCurrentHealthAttribute());
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute()) {
		//store local copy of damage
		const float LocalDamage = GetDamage();

		SetDamage(0.0f);

		if (LocalDamage > 0.0f) {
			//apply damage
			const float NewHealth = GetCurrentHealth() - LocalDamage;
			SetCurrentHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute()) {
		//store local copy of healing
		const float LocalHealing = GetHealing();

		SetHealing(0.0f);

		if (LocalHealing > 0.0f) {
			//apply healing
			const float NewHealth = GetCurrentHealth() + LocalHealing;
			SetCurrentHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute()) {
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetHealthRegenAttribute()) {
		SetHealthRegen(FMath::Clamp(GetHealthRegen(), 0.0f, GetMaxHealth()));
	}
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
}

void UHealthAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, CurrentHealth, OldValue);
}

void UHealthAttributeSet::OnRep_MaximumHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxHealth, OldValue);
}

void UHealthAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, HealthRegen, OldValue);
}
