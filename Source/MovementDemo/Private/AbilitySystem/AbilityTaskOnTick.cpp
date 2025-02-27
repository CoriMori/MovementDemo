// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTaskOnTick.h"

UAbilityTaskOnTick::UAbilityTaskOnTick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true; //enable tick for this task
}

UAbilityTaskOnTick* UAbilityTaskOnTick::AbilityTaskOnTick(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAbilityTaskOnTick* MyObj = NewAbilityTask<UAbilityTaskOnTick>(OwningAbility, TaskInstanceName);
	return MyObj;
}

void UAbilityTaskOnTick::Activate()
{
	Super::Activate();
}

void UAbilityTaskOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
