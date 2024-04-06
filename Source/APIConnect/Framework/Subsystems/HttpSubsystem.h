
#pragma once

#include "CoreMinimal.h"
#include "APIConnect/Structs/CatalogData.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "HttpSubsystem.generated.h"


UCLASS()
class APICONNECT_API UHttpSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHttpResponseReceived, const FCatalogData&, CatalogData);

public:
	FOnHttpResponseReceived OnHttpResponseReceived;

	TFuture<bool> Login(const FString& Login, const FString& Password, const FString& Fingerprint);

private:
	FString AccessToken;

	TFuture<bool> PerformPostRequestAsync(const FString& URL, const FString& Content);

	void HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TPromise<bool>& Promise);
	void FetchCatalogDataAsync() const;

	bool FetchJsonData(const FString& ResponseContent) const;
};