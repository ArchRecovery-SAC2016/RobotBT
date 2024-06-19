// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Struct/RoomPreparationStruct.h"
#include "RobotBTGameMode.generated.h"

UCLASS(minimalapi)
class ARobotBTGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARobotBTGameMode();
	virtual void BeginPlay() override;

	/** Load the start widget. */
	UFUNCTION(BlueprintCallable)
	void LoadStartWidget();

	/** Load the start widget. */
	UFUNCTION(BlueprintCallable)
	TArray<FRoomPreparationStruct> LoadWorldKnoledgeWidget();
};



