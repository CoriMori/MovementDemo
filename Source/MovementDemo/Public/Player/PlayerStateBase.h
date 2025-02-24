// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PlayerStateBase.generated.h"

/**
 * 
 */
UCLASS()
class MOVEMENTDEMO_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerStateBase();

protected:
	//ASC for this player state
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
