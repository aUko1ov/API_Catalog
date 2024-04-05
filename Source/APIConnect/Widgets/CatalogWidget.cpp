
#include "CatalogWidget.h"

#include "CatalogItemWidget.h"
#include "APIConnect/Structs/CatalogData.h"
#include "Components/EditableTextBox.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"

void UCatalogWidget::InitializeCatalog(const FCatalogData& CatalogData)
{
	SectionName = CatalogData.SectionName;
	CatalogItems = CatalogData.Items;
	OriginalCatalogItems = CatalogData.Items;
	UpdateCatalogUI();
}

void UCatalogWidget::SortCatalogItems(bool bAscending)
{
	CatalogItems = OriginalCatalogItems;
	CatalogItems.Sort([bAscending](const FCatalogItem& A, const FCatalogItem& B)
		{
			return bAscending ? A.Price < B.Price : A.Price > B.Price;
		});

	UpdateCatalogUI();
}

void UCatalogWidget::FilterCatalogItemsByPrice(float MinPrice, float MaxPrice)
{
	CatalogItems = OriginalCatalogItems.FilterByPredicate([MinPrice, MaxPrice](const FCatalogItem& Item)
		{
			return Item.Price >= MinPrice && Item.Price <= MaxPrice;
		});

	UpdateCatalogUI();
}

void UCatalogWidget::GetFilterCostNumbers(int32& InputMinPriceOut, int32& InputMaxPriceOut)
{
	InputMinPriceOut = InputMinPrice;
	InputMaxPriceOut = InputMaxPrice;
}


void UCatalogWidget::UpdateCatalogUI()
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastUpdateTime < .3f) return;
	LastUpdateTime = CurrentTime;

	if (!GridCatalog) return;

	GridCatalog->ClearChildren();

	for (int32 Index = 0; Index < CatalogItems.Num(); ++Index)
	{
		const FCatalogItem& Item = CatalogItems[Index];

		UCatalogItemWidget* ItemWidget = CreateWidget<UCatalogItemWidget>(this, CatalogItemWidgetClass);
		if (ItemWidget)
		{
			ItemWidget->SetPhotoUrl(Item.PhotoUrl);
			ItemWidget->SetPrice(Item.PriceString);

			UGridSlot* GridSlot = GridCatalog->AddChildToGrid(ItemWidget);
			GridSlot->SetRow(Index / NumberOfColumns);
			GridSlot->SetColumn(Index % NumberOfColumns);
		}
	}
}