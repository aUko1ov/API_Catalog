

#pragma once
#include "CoreMinimal.h"
#include "CatalogData.generated.h"


USTRUCT(BlueprintType)
struct FCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	FString PhotoUrl;

	UPROPERTY(BlueprintReadOnly)
	FString PriceString;

	UPROPERTY(BlueprintReadOnly)
	float Price;

};

USTRUCT(BlueprintType)
struct FCatalogData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString SectionName;

	UPROPERTY(BlueprintReadOnly)
	TArray<FCatalogItem> Items;
};