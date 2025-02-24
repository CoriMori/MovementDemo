// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetBase.h"
#include "AbilitySystem/Attributes/HealthAttributeSet.h"

bool UWidgetBase::InitWidget(UAbilitySystemComponent* OwnerASC)
{
	UAbilitySystemComponent* OldASC = AbilitySystemComponent.Get();

	AbilitySystemComponent = OwnerASC;

	if (!GetOwnerAbilitySystemComponent()) return false;

	if (IsValid(OldASC)) {
		ResetDelegateHandle(MaxHealthChangeDelegate, OldASC, UHealthAttributeSet::GetMaxHealthAttribute());
		ResetDelegateHandle(CurrentHealthChangeDelegate, OldASC, UHealthAttributeSet::GetCurrentHealthAttribute());
		ResetDelegateHandle(HealthRegenChangeDelegate, OldASC, UHealthAttributeSet::GetHealthRegenAttribute());
	}

	//bind health delegates if possible and required
	if (bListenHealth) {
		if (AbilitySystemComponent->HasAttributeSetForAttribute(UHealthAttributeSet::GetMaxHealthAttribute())) {
			MaxHealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UWidgetBase::MaxHealthChanged);
			CurrentHealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetCurrentHealthAttribute()).AddUObject(this, &UWidgetBase::CurrentHealthChanged);
			HealthRegenChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthRegenAttribute()).AddUObject(this, &UWidgetBase::HealthRegenChanged);

			const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetMaxHealthAttribute());
			const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());

			//call blueprint events to initalize values
			On_MaxHealthChanged(MaxHealth, 0.0f, CurrentHealth / MaxHealth);
			On_CurrentHealthChanged(CurrentHealth, 0.0f, CurrentHealth / MaxHealth);
			On_HealthRegenChanged(AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetHealthRegenAttribute()), 0.0f);
		}
		else { return false; }
	}

	return true;
}

UAbilitySystemComponent* UWidgetBase::GetOwnerAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void UWidgetBase::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	const float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetCurrentHealthAttribute());
	On_MaxHealthChanged(Data.NewValue, Data.OldValue, CurrentHealth / Data.NewValue);
}

void UWidgetBase::CurrentHealthChanged(const FOnAttributeChangeData& Data)
{
	const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UHealthAttributeSet::GetMaxHealthAttribute());
	On_CurrentHealthChanged(Data.NewValue, Data.OldValue, Data.NewValue / MaxHealth);
}

void UWidgetBase::HealthRegenChanged(const FOnAttributeChangeData& Data)
{
	On_HealthRegenChanged(Data.NewValue, Data.OldValue);
}

void UWidgetBase::ResetDelegateHandle(FDelegateHandle DelegateHandle, UAbilitySystemComponent* OldASC, const FGameplayAttribute& Attribute)
{
	if (IsValid(OldASC)) {
		OldASC->GetGameplayAttributeValueChangeDelegate(Attribute).Remove(DelegateHandle);
		DelegateHandle.Reset();
	}
}
