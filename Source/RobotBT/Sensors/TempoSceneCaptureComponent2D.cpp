// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoSceneCaptureComponent2D.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "TempoSensorsSettings.h"
#include "Engine/TextureRenderTarget2D.h"

UTempoSceneCaptureComponent2D::UTempoSceneCaptureComponent2D() {
	PrimaryComponentTick.bStartWithTickEnabled = false;
	ShowFlags.SetAntiAliasing(false);
	ShowFlags.SetMotionBlur(false);
	bCaptureEveryFrame = false;
	bCaptureOnMovement = false;
	bTickInEditor = false;
	bAlwaysPersistRenderingState = true;
}

void UTempoSceneCaptureComponent2D::BeginPlay() {
	Super::BeginPlay();

	/*
	if (ShouldCapture) {
		RestartCaptureTimer();
	}
	*/
	InitRenderTarget();
}

void UTempoSceneCaptureComponent2D::UpdateSceneCaptureContents(FSceneInterface* Scene) {
	TextureInitFence.Wait();

	if (TextureTarget == nullptr)
	{
		InitRenderTarget();
	}

	if (TextureTarget->SizeX != SizeXY.X || TextureTarget->SizeY != SizeXY.Y) {
		InitRenderTarget();
		return;
	}

	const FTextureRenderTargetResource* RenderTarget = TextureTarget->GameThread_GetRenderTargetResource();
	if (!ensureMsgf(RenderTarget && RenderTarget->IsInitialized(), TEXT("RenderTarget was not initialized. Skipping capture.")) ||
		!ensureMsgf(TextureRHICopy.IsValid() && TextureRHICopy->IsValid(), TEXT("TextureRHICopy was not valid. Skipping capture.")) ||
		!ensureMsgf(TextureRHICopy->GetFormat() == TextureTarget->GetFormat(), TEXT("RenderTarget and TextureRHICopy did not have same format. Skipping Capture."))) {
		return;
	}

	const int32 MaxTextureQueueSize = GetMaxTextureQueueSize();
	if (MaxTextureQueueSize > 0 && TextureReadQueue.Num() > MaxTextureQueueSize) {
		UE_LOG(LogTemp, Warning, TEXT("Fell behind while reading frames from sensor %s owner %s. Skipping capture."), *GetSensorName(), *GetOwnerName());
		return;
	}

	Super::UpdateSceneCaptureContents(Scene);

	ENQUEUE_RENDER_COMMAND(SetTempoSceneCaptureRenderFence)(
		[this](FRHICommandList& RHICmdList) {
			if (!RenderFence.IsValid()) {
				RenderFence = RHICreateGPUFence(TEXT("TempoCameraRenderFence"));
				RHICmdList.WriteGPUFence(RenderFence);
			}
		});

	SequenceId++;

	TextureReadQueue.Enqueue(MakeTextureRead());
}

FString UTempoSceneCaptureComponent2D::GetOwnerName() const {
	check(GetOwner());

	return GetOwner()->GetActorNameOrLabel();
}

FString UTempoSceneCaptureComponent2D::GetSensorName() const {
	return GetName();
}

bool UTempoSceneCaptureComponent2D::IsAwaitingRender() {
	return TextureReadQueue.IsNextAwaitingRender();
}

void UTempoSceneCaptureComponent2D::OnRenderCompleted() {
	if (!TextureReadQueue.IsNextAwaitingRender() || !RenderFence.IsValid()) {
		return;
	}

	/*
	if (GetDefault<UTempoCoreSettings>()->GetTimeMode() == ETimeMode::FixedStep) {
		while (!RenderFence->Poll()) {
			FPlatformProcess::Sleep(1e-4);
		}
	}
	else if (!RenderFence->Poll()) {
		return;
	}
	*/

	RenderFence.SafeRelease();

	const FRenderTarget* RenderTarget = TextureTarget->GetRenderTargetResource();
	if (!ensureMsgf(RenderTarget, TEXT("RenderTarget was not initialized. Skipping texture read.")) ||
		!ensureMsgf(TextureRHICopy.IsValid() && TextureRHICopy->IsValid(), TEXT("TextureRHICopy was not valid. Skipping texture read.")) ||
		!ensureMsgf(TextureRHICopy->GetFormat() == TextureTarget->GetFormat(), TEXT("RenderTarget and TextureRHICopy did not have same format. Skipping texture read."))) {
		TextureReadQueue.SkipNext();
		return;
	}

	TextureReadQueue.ReadNext(RenderTarget, TextureRHICopy);
}

void UTempoSceneCaptureComponent2D::BlockUntilMeasurementsReady() const {
	TextureReadQueue.BlockUntilNextReadComplete();
}

TOptional<TFuture<void>> UTempoSceneCaptureComponent2D::SendMeasurements() {
	if (TUniquePtr<FTextureRead> TextureRead = TextureReadQueue.DequeueIfReadComplete()) {
		return DecodeAndRespond(MoveTemp(TextureRead));
	}

	return TOptional<TFuture<void>>();
}

bool UTempoSceneCaptureComponent2D::HasPendingRequests() {


	return bHasRequest.load(); // leitura segura
}

TFuture<void> UTempoSceneCaptureComponent2D::DecodeAndRespond(TUniquePtr<FTextureRead> TextureRead) {
	bHasRequest.store(true);

	// Verifica se TextureTarget é válido
	if (!TextureTarget) {
		UE_LOG(LogTemp, Warning, TEXT("TextureTarget is null."));
		return MakeFulfilledPromise<void>().GetFuture();
	}

	FRenderTarget* RenderTarget = TextureTarget->GameThread_GetRenderTargetResource();
	if (!RenderTarget) {
		UE_LOG(LogTemp, Warning, TEXT("RenderTarget is null."));
		return MakeFulfilledPromise<void>().GetFuture();
	}

	// Lê os pixels da imagem
	TArray<FColor> Bitmap;
	FIntRect Rect(0, 0, TextureTarget->SizeX, TextureTarget->SizeY);
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	ReadPixelFlags.SetLinearToGamma(true);

	RenderTarget->ReadPixels(Bitmap, ReadPixelFlags);

	// Compacta a imagem em PNG
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	ImageWrapper->SetRaw(
		Bitmap.GetData(),
		Bitmap.GetAllocatedSize(),
		TextureTarget->SizeX,
		TextureTarget->SizeY,
		ERGBFormat::RGBA,
		8
	);

	const TArray64<uint8>& PNGData = ImageWrapper->GetCompressed();

	// Define o caminho para salvar
	const FString FileName = FString::Printf(TEXT("Capture_%s.png"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	const FString SavePath = FPaths::ProjectSavedDir() / TEXT("Captured");
	const FString FullPath = SavePath / FileName;

	// Garante que o diretório existe
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*SavePath)) {
		PlatformFile.CreateDirectory(*SavePath);
	}

	// Salva o arquivo
	if (FFileHelper::SaveArrayToFile(PNGData, *FullPath)) {
		UE_LOG(LogTemp, Log, TEXT("Salvou captura em: %s"), *FullPath);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Falha ao salvar a imagem em: %s"), *FullPath);
	}

	bHasRequest.store(false);
	return MakeFulfilledPromise<void>().GetFuture();
}

void UTempoSceneCaptureComponent2D::InitRenderTarget() {
	TextureTarget = NewObject<UTextureRenderTarget2D>(this);

	TextureTarget->TargetGamma = GetDefault<UTempoSensorsSettings>()->GetSceneCaptureGamma();
	TextureTarget->RenderTargetFormat = RenderTargetFormat;
	TextureTarget->bGPUSharedFlag = true;
	if (PixelFormatOverride == EPixelFormat::PF_Unknown) {
		TextureTarget->InitAutoFormat(SizeXY.X, SizeXY.Y);
	}
	else {
		TextureTarget->InitCustomFormat(SizeXY.X, SizeXY.Y, PixelFormatOverride, true);
	}

	struct FInitCPUCopyContext {
		FString Name;
		int32 SizeX;
		int32 SizeY;
		EPixelFormat PixelFormat;
		FTextureRHIRef* TextureRHICopy;
	};

	FInitCPUCopyContext Context = {
		FString::Printf(TEXT("%s TextureRHICopy"), *GetName()),
		TextureTarget->SizeX,
		TextureTarget->SizeY,
		TextureTarget->GetFormat(),
		&TextureRHICopy
	};

	ENQUEUE_RENDER_COMMAND(InitTempoSceneCaptureTextureCopy)(
		[Context](FRHICommandListImmediate& RHICmdList) {
			// Create the TextureRHICopy, where we will copy our TextureTarget's resource before reading it on the CPU.
			constexpr ETextureCreateFlags TexCreateFlags = ETextureCreateFlags::Shared | ETextureCreateFlags::CPUReadback;

			const FRHITextureCreateDesc Desc =
				FRHITextureCreateDesc::Create2D(*Context.Name)
				.SetExtent(Context.SizeX, Context.SizeY)
				.SetFormat(Context.PixelFormat)
				.SetFlags(TexCreateFlags);

			*Context.TextureRHICopy = RHICreateTexture(Desc);
		});

	TextureInitFence.BeginFence();

	// Any pending texture reads might have the wrong pixel format.
	TextureReadQueue.Empty();
}

float GetTimerPeriod(float RateHz) {
	// Don't allow a negative or zero rate.
	return 1.0 / FMath::Max(UE_KINDA_SMALL_NUMBER, RateHz);
}

void UTempoSceneCaptureComponent2D::RestartCaptureTimer() {
	const float TimerPeriod = GetTimerPeriod(RateHz);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTempoSceneCaptureComponent2D::MaybeCapture, TimerPeriod, true);
}

void UTempoSceneCaptureComponent2D::MaybeCapture() {
	const float TimerPeriod = GetTimerPeriod(RateHz);
	if (!FMath::IsNearlyEqual(GetWorld()->GetTimerManager().GetTimerRate(TimerHandle), TimerPeriod)) {
		RestartCaptureTimer();
	}

	/*
	if (!HasPendingRequests()) {
		return;
	}
	*/

	CaptureSceneDeferred();
}