// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CharacterBase.h"

ACharacterBase::ACharacterBase()
{

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


