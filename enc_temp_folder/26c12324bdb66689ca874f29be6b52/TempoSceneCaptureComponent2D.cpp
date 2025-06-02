// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoSceneCaptureComponent2D.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
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

    for (FColor& Color : Bitmap) {
        FVector LinearColor = FVector(Color.R / 255.0f, Color.G / 255.0f, Color.B / 255.0f);
        LinearColor.X = FMath::Pow(LinearColor.X, 1.0f / 2.2f);
        LinearColor.Y = FMath::Pow(LinearColor.Y, 1.0f / 2.2f);
        LinearColor.Z = FMath::Pow(LinearColor.Z, 1.0f / 2.2f);
        Color.R = FMath::Clamp(int32(LinearColor.X * 255.0f), 0, 255);
        Color.G = FMath::Clamp(int32(LinearColor.Y * 255.0f), 0, 255);
        Color.B = FMath::Clamp(int32(LinearColor.Z * 255.0f), 0, 255);
    }

    // Salvar imagem
    FString FileName = FString::Printf(TEXT("Captured_%04d.png"), ImageCounter++);
    FString FullPath = SaveDirectory / FileName;
    // FIntPoint Size = TextureTarget->GetSurfaceWidth(), TextureTarget->GetSurfaceHeight();

    FIntPoint DestSize(TextureTarget->SizeX, TextureTarget->SizeY);


    

    SaveAsJpeg(FullPath, Bitmap, TextureTarget->SizeX, TextureTarget->SizeY);
	// FFileHelper::CreateBitmap(*FullPath, DestSize.X, DestSize.Y, Bitmap.GetData());
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
