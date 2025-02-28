// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
{

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


