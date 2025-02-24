// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeSetBase.generated.h"

//use macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Base class for attribute sets -> includes macros and helper functions
 */
UCLASS()
class MOVEMENTDEMO_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
protected:
	void OnMaxChange(const FGameplayAttributeData& Attribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AttributeProperty) const;
};
