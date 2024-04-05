
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CatalogWidget.generated.h"


class UEditableTextBox;
class UCatalogItemWidget;
class UGridPanel;
struct FCatalogItem;

UCLASS()
class APICONNECT_API UCatalogWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Catalog")
	void InitializeCatalog(const FCatalogData& CatalogData);

	UFUNCTION(BlueprintCallable, Category = "Catalog")
	void SortCatalogItems(bool bAscending);

	UFUNCTION(BlueprintCallable, Category = "Catalog")
	void FilterCatalogItemsByPrice(float MinPrice, float MaxPrice);

	UFUNCTION(BlueprintCallable, Category = "Catalog")
	void UpdateCatalogUI();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void GetFilterCostNumbers(int32& InputMinPriceOut, int32& InputMaxPriceOut);

protected:
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GridCatalog;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Catalog")
	int32 InputMinPrice = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Catalog")
	int32 InputMaxPrice = 2000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Catalog")
	int32 NumberOfColumns = 5;

	UPROPERTY(BlueprintReadOnly, Category = "Catalog")
	FString SectionName;

	UPROPERTY(BlueprintReadOnly, Category = "Catalog")
	TArray<FCatalogItem> CatalogItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Catalog")
	TSubclassOf<UCatalogItemWidget> CatalogItemWidgetClass;

private:
	TArray<FCatalogItem> OriginalCatalogItems;

	float LastUpdateTime = 0.0f;
};
