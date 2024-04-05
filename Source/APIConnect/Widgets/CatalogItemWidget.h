
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CatalogItemWidget.generated.h"


class UTextBlock;
class UImage;

UCLASS()
class APICONNECT_API UCatalogItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PriceText;

	UFUNCTION(BlueprintCallable)
	void SetPhotoUrl(const FString& Url) const;

	UFUNCTION(BlueprintCallable)
	void SetPrice(const FString& PriceString) const;
};
