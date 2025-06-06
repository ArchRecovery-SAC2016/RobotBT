// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoSceneCaptureComponent2D.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RobotBT/Actors/Robot.h"

UTempoSceneCaptureComponent2D::UTempoSceneCaptureComponent2D() {
	PrimaryComponentTick.bCanEverTick = false;

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
    bCaptureEveryFrame = false;
    bCaptureOnMovement = false;
    bAlwaysPersistRenderingState = true; // sem isso aki nao funciona
    ShowFlags.SetAntiAliasing(true);
    ShowFlags.SetTemporalAA(true);
    ShowFlags.SetMotionBlur(false);
}

void UTempoSceneCaptureComponent2D::BeginPlay() {
	Super::BeginPlay();
	
	IFileManager::Get().MakeDirectory(*SaveDirectory, true);

    RobotOwnerInstance = Cast<ARobot>(GetOwner());
}

void UTempoSceneCaptureComponent2D::StartCapture(ECaptureType NewCaptureType) {
    StopCapture();

	if (NewCaptureType == ECaptureType::NONE) {
        return;
	}

	if (NewCaptureType == ECaptureType::DEPTH) {
        ApplyDepthFilter();
    }

	if (NewCaptureType == ECaptureType::COLOR) {
        ApplyColorFilter();
    }

    if (NewCaptureType == ECaptureType::LABEL) {
        ApplyColorFilter();
    }

    GetWorld()->GetTimerManager().SetTimer(CaptureTimerHandle, this, &UTempoSceneCaptureComponent2D::CaptureAndSave, 0.5f, true); // captura a cada 0.5s
}

void UTempoSceneCaptureComponent2D::StopCapture() {
    GetWorld()->GetTimerManager().ClearTimer(CaptureTimerHandle);
}

void UTempoSceneCaptureComponent2D::ApplyDepthFilter() {
    this->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
    bCaptureEveryFrame = false;
    bCaptureOnMovement = false;
    CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

    if (BasePostProcessMaterial_WithDepth != nullptr) {
        CameraPostProcess = UMaterialInstanceDynamic::Create(BasePostProcessMaterial_WithDepth, this);
    }

    if (CameraPostProcess != nullptr) {
        MinDepth = GEngine->NearClipPlane;
        CameraPostProcess->SetScalarParameterValue(TEXT("MinDepth"), MinDepth);
        CameraPostProcess->SetScalarParameterValue(TEXT("MaxDepth"), MaxDepth);
        CameraPostProcess->SetScalarParameterValue(TEXT("MaxDiscreteDepth"), kMaxDiscreteDepth);

        //  Aplica o material ao sistema de pós-processamento da cena capturada
        PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, CameraPostProcess));
    } else {
        UE_LOG(LogTemp, Error, TEXT("PostProcessMaterialWithDepth is not set"));
    }
}

void UTempoSceneCaptureComponent2D::ApplyColorFilter() {
    this->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
    CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    CameraPostProcess = nullptr;
}

void UTempoSceneCaptureComponent2D::ApplyLabelFilter() {
    this->TextureTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
    CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    CameraPostProcess = nullptr;
}

void UTempoSceneCaptureComponent2D::CaptureAndSave() {
    if (!TextureTarget || !RobotOwnerInstance) return;
    

    if (RobotOwnerInstance->IsMoving == false) return; // just capture if is moving

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

bool UTempoSceneCaptureComponent2D::SaveAsJpeg(const FString& Filename, const TArray<FColor>& Bitmap, int32 Width, int32 Height) {
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

    if (ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.Num() * sizeof(FColor), Width, Height, ERGBFormat::BGRA, 8)) {
        const TArray64<uint8>& JpegData = ImageWrapper->GetCompressed(100); // Qualidade: 100 = máxima
        return FFileHelper::SaveArrayToFile(JpegData, *Filename);
    }

    return false;
}