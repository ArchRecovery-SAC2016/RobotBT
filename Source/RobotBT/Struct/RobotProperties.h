#pragma once

#include "CoreMinimal.h"
#include "RobotProperties.generated.h"

USTRUCT(BlueprintType)
struct FBattery {
	GENERATED_BODY()

	// The actual charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Battery")
	float Charge = 1.0f;

	// Minmum Userful Level. Below this will fail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Battery")
	float MinimumUsefulLevel = 0.05f;

	// Discharge Rate. It will scale with the speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Battery")
	float DischargeRate = 0.1f;
};

USTRUCT(BlueprintType)
struct FSkill {
	GENERATED_BODY()

	// Capacidade total da bateria
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Skills")
	FString Name = "SkillName";

	// What is the chance to fail when performe this action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Skills")
	float ChanceToFail = 1.0f;

	// N�vel m�nimo �til da bateria
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Skills")
	FString Goal = "Goal";

	// How much battery will consume when performe this skill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Skills")
	float BatteryConsumeDischargeRate = 0.1f;
};


USTRUCT(BlueprintType)
struct FRobotProperties {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString Name = "Robot";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString SquadName = "Squad";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FBattery Battery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	TArray<FSkill> Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float Speed = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FVector InitialLocation;
};