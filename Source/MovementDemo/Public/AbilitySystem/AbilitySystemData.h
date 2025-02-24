// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemData.generated.h"


/**
 * Ability System Data Struct
 * Used to apply default values on player
 */

class UAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilitySystemInitData {
	GENERATED_BODY()

	//Array of Attribute Sets
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (DisplayName = "Attribute Sets"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;

	//Map of Attributes and Floats to set as their base values
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (DisplayName = "Attribute Base Values"))
	TMap<FGameplayAttribute, float> AttributeBaseValues;

	//Array of Gameplay Abilities to be given
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (DisplayName = "Gameplay Abilities"))
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbilities;

	//Array of Gameplay Effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (DisplayName = "Gameplay Effects"))
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffects;

	//Container of Gameplay Tags
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (DisplayName = "Gameplay Tags"))
	FGameplayTagContainer GameplayTags;
};

UENUM(BlueprintType)
enum class EAttributeTypes : uint8 {
	FINAL UMETA(DisplayName = "Final"), //final val of attribute
	BASE UMETA(DisplayName = "Base"), //base val of attribute
	BONUS UMETA(DisplayName = "Bonus"), //final minus base val
};

