#pragma once

#include "CoreMinimal.h"
#include "RobotBT/Enum/SkillEnum.h"
#include "RobotProperties.generated.h"

USTRUCT(BlueprintType)
struct FBattery {
	GENERATED_BODY()

	// The actual charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Battery")
	float Charge = 1.0f;

	// Minmum Userful Level. Below this will fail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Battery")
	float MinimumUsefulLevel = 0.0f;

	// Discharge Rate. It will scale with the speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Battery")
	float DischargeRate = 0.1f;
};

USTRUCT(BlueprintType)
struct FSkill {
	GENERATED_BODY()

	// The skill
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Skills")
	ESkillEnum SkillEnum = ESkillEnum::NONE;

	// What is the chance to fail when performe this action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Skills")
	float ChanceToFail = 0.1f;

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
	FBattery Battery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	TArray<FSkill> Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	FString Color = "Black";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float Speed = 100;

	UPROPERTY()
	FTransform InitialTransform;

};
