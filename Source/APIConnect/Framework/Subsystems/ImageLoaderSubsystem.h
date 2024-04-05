
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "ImageLoaderSubsystem.generated.h"


UCLASS()
class APICONNECT_API UImageLoaderSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	void LoadImageAsync(const FString& ImageUrl, const TFunction<void(UTexture2D*)>& Callback);
};
