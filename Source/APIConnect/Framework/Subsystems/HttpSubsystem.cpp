
#include "HttpSubsystem.h"

#include "HttpModule.h"
#include "APIConnect/Structs/CatalogData.h"
#include "Interfaces/IHttpResponse.h"


TFuture<bool> UHttpSubsystem::Login(const FString& Login, const FString& Password, const FString& Fingerprint)
{
	const FString Content = FString::Printf(TEXT("{\"login\":\"%s\",\"password\":\"%s\",\"fingerprint\":\"%s\"}"), *Login, *Password, *Fingerprint);
	return PerformPostRequestAsync("https://stanzza-api.aicrobotics.ru/api/auth/login", Content);
}

TFuture<bool> UHttpSubsystem::PerformPostRequestAsync(const FString& URL, const FString& Content)
{
	return Async(EAsyncExecution::ThreadPool, [this, URL, Content]() -> bool
		{
			const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
			Request->SetURL(URL);
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
			Request->SetContentAsString(Content);

			auto PromisePtr = MakeShared<TPromise<bool>>();
			const auto Future = PromisePtr->GetFuture();

			Request->OnProcessRequestComplete().BindLambda([this, PromisePtr](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) mutable
				{
					HandleResponse(Request, Response, bWasSuccessful, *PromisePtr);
				});

			Request->ProcessRequest();
			return Future.Get();
		});
}

void UHttpSubsystem::HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TPromise<bool>& Promise)
{
	if (bWasSuccessful && Response.IsValid())
	{
		if (Response->GetResponseCode() == 200)
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				AccessToken = JsonObject->GetStringField("access_token");
				Promise.SetValue(true);

				FetchCatalogDataAsync();
				return;
			}
		}
	}

	Promise.SetValue(false);
}

void UHttpSubsystem::FetchCatalogDataAsync() const
{
	Async(EAsyncExecution::ThreadPool, [this]()
		{
			if (AccessToken.IsEmpty())
			{
				return;
			}

			const FString URL = "https://stanzza-api.aicrobotics.ru/api/v1/catalog/06-02-013";
			const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
			Request->SetURL(URL);
			Request->SetVerb(TEXT("GET"));
			Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));

			Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) mutable
				{
					if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
					{
						FString ResponseContent = Response->GetContentAsString();
						FetchJsonData(ResponseContent);
					}
					else
					{
						// TODO: Обрабатываем ошибку
					}
				});

			Request->ProcessRequest();
		});
}

bool UHttpSubsystem::FetchJsonData(const FString& ResponseContent) const
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// Пытаемся разобрать JSON
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		FCatalogData CatalogData;
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
		OnHttpResponseReceived.Broadcast(CatalogData);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
		return false;
	}
}