// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AttributeSetBase.h"
#include "HealthAttributeSet.generated.h"

/**
 * Contains attributes related to health -> apply to all characters that take damage
 */
UCLASS()
class MOVEMENTDEMO_API UHealthAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	UHealthAttributeSet();
	
	//overrides from attribute set
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	//set replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//local values

	//local damage amount
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", meta = (HideFromLevelInfos))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Damage)

	//local healing amount
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", meta = (HideFromLevelInfos))
	FGameplayAttributeData Healing;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Healing)


	//replicated values

	//current health
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", ReplicatedUsing = OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, CurrentHealth)

	//max health
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", ReplicatedUsing = OnRep_MaximumHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)

	//health regen rate
	UPROPERTY(BlueprintReadOnly, Category = "Health Attribute Set", ReplicatedUsing = OnRep_HealthRegen)
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, HealthRegen)

protected:
	UFUNCTION()
	virtual void OnRep_CurrentHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaximumHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HealthRegen(const FGameplayAttributeData& OldValue);
};
