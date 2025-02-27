// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTaskOnTick.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
/**
 * Task for abilities that supplies tick and its delta time
 */
UCLASS()
class MOVEMENTDEMO_API UAbilityTaskOnTick : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAbilityTaskOnTick(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTaskOnTick* AbilityTaskOnTick(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
};
