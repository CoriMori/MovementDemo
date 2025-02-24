// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerStateBase.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"

APlayerStateBase::APlayerStateBase()
{
	//increase Net frequency to prevent delay between activation and application on the client
	NetUpdateFrequency = 100.0f;

	//create the component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>("AbilitySystemComponent");
}

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
