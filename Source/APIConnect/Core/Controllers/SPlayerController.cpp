
#include "SPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "APIConnect/Framework/Subsystems/HttpSubsystem.h"


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

void ASPlayerController::SetFetchingDelegateAPI()
{
	if (UHttpSubsystem* HttpSubsystem = GetLocalPlayer()->GetSubsystem<UHttpSubsystem>())
	{
		HttpSubsystem->OnHttpResponseReceived.AddDynamic(this, &ASPlayerController::OnFetchData_JSON_API);
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(FetchData_JSON_API, ETriggerEvent::Started, this, &ASPlayerController::OnLoginServer);

}

void ASPlayerController::OnLoginServer()
{
	if (UHttpSubsystem* HttpSubsystem = GetLocalPlayer()->GetSubsystem<UHttpSubsystem>())
	{
		HttpSubsystem->Login("candidate@asdfizac.org", "asdfizac_tpsxP3f5*s", "fingerprint");
	}
}

void ASPlayerController::OnFetchData_JSON_API(const FString& ResponseContent)
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

        	FString PriceString = FString::Printf(TEXT("%s %s"), *ItemObject->GetStringField("price_1"), *ItemObject->GetStringField("currency_1"));
            CatalogItem.Price = FCString::Atof(*PriceString);

            CatalogData.Items.Add(CatalogItem);
        }
    	
        UE_LOG(LogTemp, Warning, TEXT("Section Name: %s"), *CatalogData.SectionName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
    }
}
