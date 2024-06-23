// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "RobotBTPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class ARobotBTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARobotBTPlayerController();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MyComponent)
	TSubclassOf<class UWorldKnowledgeWidget> WorldKnowledgeWBP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MyComponent)
	class UWorldKnowledgeWidget* WorldKnowledgeWidget;

};


