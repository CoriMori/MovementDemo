// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem//AbilitySystemData.h"
#include "CharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class MOVEMENTDEMO_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	//Implement the IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	//Creates a pointer to the Ability System Component associated with this Character
	//Player Characters will set this OnRep_PlayerState() locally and in OnPossessed() server side, NPCs can be set in their constructor
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	//Data used to intantiate the Ability System Component -> AbilitySystemData.h
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System", Meta = (ShowOnlyInnerProperties))
	FAbilitySystemInitData AbilitySystemInitData;
};
