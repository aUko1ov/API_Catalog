
#pragma once

#include "CoreMinimal.h"
#include "APIConnect/Structs/CatalogData.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


class UCatalogWidget;
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

#pragma region API_Server
	void OnLoginServer();
	void SetFetchingDelegateAPI();
	bool FetchJsonData(const FString& ResponseContent, FCatalogData& CatalogData) const;

	UFUNCTION()
	void OnFetchData(const FString& ResponseContent);

	void OnSwitchCatalogAscendingCost();
	void OnSwitchCatalogDescendingCost();
	void OnTurnCatalogFilterCost();
#pragma endregion

#pragma region Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FetchDataAPI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SwitchCatalogAscendingCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SwitchCatalogDescendingCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TurnCatalogFilterCost;
#pragma endregion

#pragma region Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCatalogWidget> CatalogWidgetClass;

	TObjectPtr<UCatalogWidget> CatalogWidgetInstance;
#pragma endregion 
};
