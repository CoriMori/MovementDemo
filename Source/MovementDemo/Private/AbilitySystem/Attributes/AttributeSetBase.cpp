// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/AttributeSetBase.h"
#include "AbilitySystemComponent.h"

void UAttributeSetBase::OnMaxChange(const FGameplayAttributeData& Attribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AttributeProperty) const
{
	//get ref to the ASC
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && ASC) {
		//change current value to maintain percentage

		const float CurrentValue = Attribute.GetCurrentValue();
		const float NewChange = CurrentMaxValue > 0.0f ? CurrentValue * NewMaxValue / CurrentMaxValue - CurrentValue : NewMaxValue;

		ASC->ApplyModToAttributeUnsafe(AttributeProperty, EGameplayModOp::Additive, NewChange);
	}
}
