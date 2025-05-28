// MyGameHUD.cpp
#include "MyGameHUD.h"
#include "Blueprint/UserWidget.h"

void AMyGameHUD::BeginPlay() {
    Super::BeginPlay();

 
    // Experiment Setup Widget
    if (ExperimentSetupWidgetClass) {
        ExperimentSetupWidget = CreateWidget<UUserWidget>(GetWorld(), ExperimentSetupWidgetClass);
        if (ExperimentSetupWidget) {
            ExperimentSetupWidget->AddToViewport();
        }
    }
}
