// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldKnowledgeWidget.h"

#include "Kismet/GameplayStatics.h"
#include "RobotBT/RobotBTGameMode.h"

void UWorldKnowledgeWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UWorldKnowledgeWidget::SetRooms(TArray<FRoomPreparationStruct> NewRooms) {
	Rooms = NewRooms;
}

void UWorldKnowledgeWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();

	InitialLoad();

	if (Rooms.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("[UWorldKnowledgeWidget::SynchronizeProperties] Rooms is empty"));
		return;
	}
}

void UWorldKnowledgeWidget::InitialLoad() {
	ARobotBTGameMode* GameMode = Cast<ARobotBTGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWorldKnowledgeWidget::InitialLoad] GameMode is null. You mus se the game mode on the project settings"));
		return;
	}

	Rooms = GameMode->LoadWorldKnoledgeWidget();


}