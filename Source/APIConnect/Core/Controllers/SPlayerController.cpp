
#include "SPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "APIConnect/Framework/Subsystems/HttpSubsystem.h"
#include "APIConnect/Widgets/CatalogWidget.h"


void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	SetFetchingDelegateAPI();
}

void ASPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(FetchDataAPI, ETriggerEvent::Started, this, &ASPlayerController::OnLoginServer);
	EnhancedInputComponent->BindAction(SwitchCatalogAscendingCost, ETriggerEvent::Started, this, &ASPlayerController::OnSwitchCatalogAscendingCost);
	EnhancedInputComponent->BindAction(SwitchCatalogDescendingCost, ETriggerEvent::Started, this, &ASPlayerController::OnSwitchCatalogDescendingCost);
	EnhancedInputComponent->BindAction(TurnCatalogFilterCost, ETriggerEvent::Started, this, &ASPlayerController::OnTurnCatalogFilterCost);
}

#pragma region API_Server

void ASPlayerController::SetFetchingDelegateAPI()
{
	if (UHttpSubsystem* HttpSubsystem = GetLocalPlayer()->GetSubsystem<UHttpSubsystem>())
	{
		HttpSubsystem->OnHttpResponseReceived.AddDynamic(this, &ASPlayerController::OnFetchData);
	}
}

void ASPlayerController::OnLoginServer()
{
	if (UHttpSubsystem* HttpSubsystem = GetLocalPlayer()->GetSubsystem<UHttpSubsystem>())
	{
		HttpSubsystem->Login("candidate@asdfizac.org", "asdfizac_tpsxP3f5*s", "fingerprint");
	}
}

void ASPlayerController::OnSwitchCatalogAscendingCost()
{
	if (!IsValid(CatalogWidgetInstance)) return;
	CatalogWidgetInstance->SortCatalogItems(true);
}

void ASPlayerController::OnSwitchCatalogDescendingCost()
{
	if (!IsValid(CatalogWidgetInstance)) return;
	CatalogWidgetInstance->SortCatalogItems(false);
}

void ASPlayerController::OnTurnCatalogFilterCost()
{
	if (!IsValid(CatalogWidgetInstance)) return;
	int32 InputMinPrice = -1, InputMaxPrice = -1;
	CatalogWidgetInstance->GetFilterCostNumbers(InputMinPrice, InputMaxPrice);
	CatalogWidgetInstance->FilterCatalogItemsByPrice(InputMinPrice, InputMaxPrice);
}

void ASPlayerController::OnFetchData(const FString& ResponseContent)
{
	FCatalogData CatalogData;
	if (FetchJsonData(ResponseContent, CatalogData))
	{
		CatalogWidgetInstance = CreateWidget<UCatalogWidget>(this, CatalogWidgetClass);
		if (!IsValid(CatalogWidgetInstance)) return;

		CatalogWidgetInstance->AddToViewport();
		CatalogWidgetInstance->InitializeCatalog(CatalogData);
	}
}

bool ASPlayerController::FetchJsonData(const FString& ResponseContent, FCatalogData& CatalogData) const
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// Пытаемся разобрать JSON
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TSharedPtr<FJsonObject> CatalogObject = JsonObject->GetObjectField("catalog");

		CatalogData.SectionName = CatalogObject->GetStringField("title");

		TArray<TSharedPtr<FJsonValue>> ItemsArray = CatalogObject->GetArrayField("children");

		for (const TSharedPtr<FJsonValue> Item : ItemsArray)
		{
			const TSharedPtr<FJsonObject> ItemObject = Item->AsObject();
			FCatalogItem CatalogItem;

			CatalogItem.Name = ItemObject->GetStringField("object");
			TArray<TSharedPtr<FJsonValue>> PhotoArray = ItemObject->GetArrayField("photo");
			if (PhotoArray.Num() > 0)
			{
				const TSharedPtr<FJsonObject> PhotoObject = PhotoArray[0]->AsObject();
				CatalogItem.PhotoUrl = PhotoObject->GetStringField("preview");
			}

			CatalogItem.PriceString = FString::Printf(TEXT("%s %s"), *ItemObject->GetStringField("price_1"), *ItemObject->GetStringField("currency_1"));
			CatalogItem.Price = FCString::Atof(*CatalogItem.PriceString);

			CatalogData.Items.Add(CatalogItem);
		}

		UE_LOG(LogTemp, Warning, TEXT("Section Name: %s"), *CatalogData.SectionName);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
		return false;
	}
}

#pragma endregion