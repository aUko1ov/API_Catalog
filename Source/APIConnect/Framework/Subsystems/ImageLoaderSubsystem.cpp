
#include "ImageLoaderSubsystem.h"

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"

void UImageLoaderSubsystem::LoadImageAsync(const FString& ImageUrl, const TFunction<void(UTexture2D*)>& Callback)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(ImageUrl);
    Request->SetVerb(TEXT("GET"));
    Request->OnProcessRequestComplete().BindLambda(
        [Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            UTexture2D* ResultTexture = nullptr;

            if (bWasSuccessful && Response.IsValid())
            {
                IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
                TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

                TArray<uint8> RawData;
                if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(Response->GetContent().GetData(), Response->GetContentLength()) &&
                    ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
                {
                    AsyncTask(ENamedThreads::GameThread, [RawData, ImageWrapper, Callback]()
                        {
                            UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
                            if (Texture)
                            {
                                Texture->AddToRoot(); // Prevent GC
                                void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
                                FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
                                Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
                                Texture->UpdateResource();
                                Callback(Texture);
                            }
                            else
                            {
                                Callback(nullptr);
                            }
                        });
                    return;
                }
            }
            Callback(ResultTexture);
        });
    Request->ProcessRequest();
}

