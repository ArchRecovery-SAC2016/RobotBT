// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "RobotBTCharacter.generated.h"

UCLASS(Blueprintable)
class ARobotBTCharacter : public ADefaultPawn {
	GENERATED_BODY()

public:
    ARobotBTCharacter();

protected:
    virtual void BeginPlay() override;
};

