// MyGameHUD.h
#pragma once

#include "CoreMinimal.h"
#include "ExperimentSetupWidget.h"
#include "GameFramework/HUD.h"
#include "MyGameHUD.generated.h"

UCLASS()
class ROBOTBT_API AMyGameHUD : public AHUD {
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ExperimentSetupWidgetClass;

private:
    UPROPERTY()
	UUserWidget* ExperimentSetupWidget;
};
