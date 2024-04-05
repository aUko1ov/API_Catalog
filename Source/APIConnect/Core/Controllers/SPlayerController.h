
#pragma once

#include "CoreMinimal.h"
#include "APIConnect/Structs/CatalogData.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


class UHttpSubsystem;
class UInputAction;
class UInputMappingContext;

UCLASS()
class APICONNECT_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;

	void OnLoginServer();
	void SetFetchingDelegateAPI();

	UFUNCTION()
	void OnFetchData_JSON_API(const FString& ResponseContent);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FetchData_JSON_API;
	
	FCatalogData CatalogData;

};
