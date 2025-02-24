// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "WidgetBase.generated.h"

/**
 * Base Widget class
 * Contains functions to get values for Health and Mana attribute sets -> bind to their value change events
 */
UCLASS()
class MOVEMENTDEMO_API UWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	//Should the widget bind to specific attribute sets -> will fail if sets not found
	UPROPERTY(BlueprintReadOnly, Editanywhere)
	bool bListenHealth = false;

	//call to initalize the widget and bind to attribute set changes
	UFUNCTION(BlueprintCallable, Category = "Ability System")
	bool InitWidget(UAbilitySystemComponent* OwnerASC);

	//return owner ASC
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

	// event for max health change
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_MaxHealthChanged(const float NewValue, const float OldValue, const float NewPercent);

	// event for current health change
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_CurrentHealthChanged(const float NewValue, const float OldValue, const float NewPercent);

	// event for health regen change
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability System")
	void On_HealthRegenChanged(const float NewValue, const float OldValue);

protected:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FDelegateHandle MaxHealthChangeDelegate;
	FDelegateHandle CurrentHealthChangeDelegate;
	FDelegateHandle HealthRegenChangeDelegate;

	void MaxHealthChanged(const FOnAttributeChangeData& Data);
	void CurrentHealthChanged(const FOnAttributeChangeData& Data);
	void HealthRegenChanged(const FOnAttributeChangeData& Data);

	static void ResetDelegateHandle(FDelegateHandle DelegateHandle, UAbilitySystemComponent* OldASC, const FGameplayAttribute& Attribute);	
};
