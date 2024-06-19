// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTGameMode.h"
#include "RobotBTPlayerController.h"
#include "RobotBTCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARobotBTGameMode::ARobotBTGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARobotBTPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}


void ARobotBTGameMode::BeginPlay() {
    Super::BeginPlay();
}

void ARobotBTGameMode::LoadStartWidget() {

}


TArray<FRoomPreparationStruct> ARobotBTGameMode::LoadWorldKnoledgeWidget() {
    FRoomPreparationStruct RoomA = FRoomPreparationStruct(TEXT("RoomA"), TEXT("c3"),
        false,
        false,
        false);

    FRoomPreparationStruct RoomB = FRoomPreparationStruct(TEXT("RoomB"), TEXT("c6"),
        false,
        false,
        true);

    FRoomPreparationStruct RoomC = FRoomPreparationStruct(TEXT("RoomC"), TEXT("c8"),
        true,
        true,
        true);

    FRoomPreparationStruct SanitizationRoom = FRoomPreparationStruct(TEXT("SanitizationRoom"), TEXT("c10"),
        true,
        true,
        true);

    TArray<FRoomPreparationStruct> NewRooms;
    NewRooms.Add(RoomA);
    NewRooms.Add(RoomB);
    NewRooms.Add(RoomC);
    NewRooms.Add(SanitizationRoom);

    return NewRooms;

}

