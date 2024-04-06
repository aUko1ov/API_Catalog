
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

void ASPlayerController::OnFetchData(const FCatalogData& CatalogData)
{
	CatalogWidgetInstance = CreateWidget<UCatalogWidget>(this, CatalogWidgetClass);
	if (!IsValid(CatalogWidgetInstance)) return;

	CatalogWidgetInstance->AddToViewport();
	CatalogWidgetInstance->InitializeCatalog(CatalogData);
}

#pragma endregion