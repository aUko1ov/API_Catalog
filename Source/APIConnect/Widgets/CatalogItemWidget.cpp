
#include "CatalogItemWidget.h"

#include "APIConnect/Framework/Subsystems/ImageLoaderSubsystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCatalogItemWidget::SetPhotoUrl(const FString& Url) const
{
	UImageLoaderSubsystem* ImageLoader = GEngine->GetEngineSubsystem<UImageLoaderSubsystem>();
	ImageLoader->LoadImageAsync(Url, [this](UTexture2D* LoadedTexture)
		{
			AsyncTask(ENamedThreads::GameThread, [this, LoadedTexture]()
				{
					if (LoadedTexture && ItemImage)
					{
						ItemImage->SetBrushFromTexture(LoadedTexture);
					}
				});
		});
}

void UCatalogItemWidget::SetPrice(const FString& PriceString) const
{
	if (PriceText)
	{
		const FText PriceTextValue = FText::FromString(PriceString);
		PriceText->SetText(PriceTextValue);
	}
}