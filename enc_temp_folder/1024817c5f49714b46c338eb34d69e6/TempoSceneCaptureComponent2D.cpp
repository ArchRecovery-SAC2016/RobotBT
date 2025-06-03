// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoSceneCaptureComponent2D.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "TempoSensorsSettings.h"
#include "Engine/TextureRenderTarget2D.h"

UTempoSceneCaptureComponent2D::UTempoSceneCaptureComponent2D() {
	PrimaryComponentTick.bCanEverTick = false;

	bIsCapturing = false;
	SaveDirectory = FPaths::ProjectSavedDir() / TEXT("CapturedImages/");

    PrimaryComponentTick.bCanEverTick = false;

    // Criar o render target
    TextureTarget = NewObject<UTextureRenderTarget2D>();
    TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
    TextureTarget->InitAutoFormat(1920, 1080); // ou qualquer resolução que você deseje
    TextureTarget->UpdateResourceImmediate(true);

	ShowFlags.SetPostProcessing(true);
    ShowFlags.SetLighting(true);
    ShowFlags.SetGlobalIllumination(true);
    this->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    this->TextureTarget = TextureTarget;

    PostProcessSettings.AutoExposureMethod = AEM_Basic;
    PostProcessSettings.AutoExposureSpeedUp = 20.0;
    PostProcessSettings.AutoExposureSpeedDown = 20.0;
    // Auto exposure percentages chosen to better match their own recommended settings (see Scene.h).
    PostProcessSettings.AutoExposureLowPercent = 75.0;
    PostProcessSettings.AutoExposureHighPercent = 85.0;
    PostProcessSettings.MotionBlurAmount = 0.0;
    ShowFlags.SetAntiAliasing(true);
    ShowFlags.SetTemporalAA(true);
    ShowFlags.SetMotionBlur(false);
}




void UTempoSceneCaptureComponent2D::BeginPlay() {
	Super::BeginPlay();

	/*
	if (ShouldCapture) {
		RestartCaptureTimer();
	}
	*/



	
	IFileManager::Get().MakeDirectory(*SaveDirectory, true);
}

void UTempoSceneCaptureComponent2D::StartCapture() {
    if (bIsCapturing) return;

    bIsCapturing = true;
    bDepthEnabled = true;
    ApplyDepthEnabled();
    GetWorld()->GetTimerManager().SetTimer(CaptureTimerHandle, this, &UTempoSceneCaptureComponent2D::CaptureAndSave, 0.5f, true); // captura a cada 0.5s
}

void UTempoSceneCaptureComponent2D::StopCapture() {
    bIsCapturing = false;
    GetWorld()->GetTimerManager().ClearTimer(CaptureTimerHandle);
}

void UTempoSceneCaptureComponent2D::CaptureAndSave() {
    if (!TextureTarget) return;

    // Força atualização do conteúdo e captura real
    UpdateContent();
    CaptureScene();
    FlushRenderingCommands();

    FRenderTarget* RenderTarget = TextureTarget->GameThread_GetRenderTargetResource();
    if (!RenderTarget) return;

    // Cria bitmap
    TArray<FColor> Bitmap;
    RenderTarget->ReadPixels(Bitmap);

    // Salvar imagem
    FString FileName = FString::Printf(TEXT("Captured_%04d.png"), ImageCounter++);
    FString FullPath = SaveDirectory / FileName;
    // FIntPoint Size = TextureTarget->GetSurfaceWidth(), TextureTarget->GetSurfaceHeight();

    FIntPoint DestSize(TextureTarget->SizeX, TextureTarget->SizeY);


    

    SaveAsJpeg(FullPath, Bitmap, TextureTarget->SizeX, TextureTarget->SizeY);
	// FFileHelper::CreateBitmap(*FullPath, DestSize.X, DestSize.Y, Bitmap.GetData());
}


void UTempoSceneCaptureComponent2D::ApplyDepthEnabled() {
    if (bDepthEnabled) {
        this->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
        // PixelFormatOverride = EPixelFormat::PF_A16B16G16R16;

		if (BasePostProcessMaterial_WithDepth != nullptr) {
			CameraPostProcess = UMaterialInstanceDynamic::Create(BasePostProcessMaterial_WithDepth, this);
		}

        if (CameraPostProcess != nullptr) {
            MinDepth = GEngine->NearClipPlane;
            CameraPostProcess->SetScalarParameterValue(TEXT("MinDepth"), MinDepth);
            CameraPostProcess->SetScalarParameterValue(TEXT("MaxDepth"), MaxDepth);
            CameraPostProcess->SetScalarParameterValue(TEXT("MaxDiscreteDepth"), kMaxDiscreteDepth);
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("PostProcessMaterialWithDepth is not set"));
        }
    }
    else {
        this->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8; // Corresponds to PF_B8G8R8A8
        // this->TextureTarget->PixelFormatOverride = EPixelFormat::PF_Unknown;
    }

    /*
    if (CameraPostProcess_WithDepth) {
        PostProcessSettings.WeightedBlendables.Array.Empty();
        PostProcessSettings.WeightedBlendables.Array.Init(FWeightedBlendable(1.0, CameraPostProcess_WithDepth), 1);
        CameraPostProcess_WithDepth->EnsureIsComplete();
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("CameraPostProcess_WithDepth is not set."));
    }
    */
}

bool UTempoSceneCaptureComponent2D::SaveAsJpeg(const FString& Filename, const TArray<FColor>& Bitmap, int32 Width, int32 Height) {
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

    if (ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8)) {
        const TArray64<uint8>& JpegData = ImageWrapper->GetCompressed(100); // Qualidade: 100 = máxima
        return FFileHelper::SaveArrayToFile(JpegData, *Filename);
    }

    return false;
}
