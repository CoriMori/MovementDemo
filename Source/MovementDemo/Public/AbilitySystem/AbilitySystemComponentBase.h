// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemComponentBase.generated.h"

/**
 * Custom Ability System Component
 */

UCLASS()
class MOVEMENTDEMO_API UAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UAbilitySystemComponentBase();

	//called to init ASC with provided data -> AbilitySystemData.h
	//call on both server and client to init properly
	UFUNCTION(BlueprintCallable)
	void InitializeAbilitySystemData(const FAbilitySystemInitData& InitData, AActor* OwningActor, AActor* Avatar);

	// Wrapper for the "GetOrCreateAttributeSubobject" function. Returns the specified Attribute Set / creates one if it isn't found.
	const UAttributeSet* GetOrCreateAttributeSet(const TSubclassOf<UAttributeSet>& InAttributeSet);

protected:
	bool bDataInit = false; //track if we've already initalized the data for this componenet
};
