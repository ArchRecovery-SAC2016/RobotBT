#include "UtilMethods.h"

#include "RobotBT/Struct/RobotProperties.h"

void UUtilMethods::ShowLogMessage(const FString& Message, EMessageColorEnum Type) {
    FColor Color = FColor::Emerald;

    if (Type == EMessageColorEnum::ERROR) {
        Color = FColor::Red;
        UE_LOG(LogTemp, Error, TEXT("[UtilMethods::ShowLogMessage] %s"), *Message);
    }
    else if (Type == EMessageColorEnum::WARNING) {
        Color = FColor::Yellow;
        UE_LOG(LogTemp, Warning, TEXT("[UtilMethods::ShowLogMessage] %s"), *Message);
    }
    else if (Type == EMessageColorEnum::INFO) {
        Color = FColor::Green;
        UE_LOG(LogTemp, Display, TEXT("[UtilMethods::ShowLogMessage] %s"), *Message);
    }
    else if (Type == EMessageColorEnum::SUCCESS) {
        Color = FColor::Blue;
        UE_LOG(LogTemp, Display, TEXT("[UtilMethods::ShowLogMessage] %s"), *Message);
    }

    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 25.f, Color, Message);
    }
    
}

void UUtilMethods::PrintFailureMessage(EFailureReasonEnum FailureReason, FRobotProperties RobotProperties) {
    const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EFailureReasonEnum"), true);
    if (!EnumPtr) return ShowLogMessage(TEXT("Invalid Reason"), EMessageColorEnum::ERROR);

	// get the string value of the enum
    FString Reason= EnumPtr->GetDisplayNameTextByIndex(static_cast<int32>(FailureReason)).ToString();

	FString Message = FString::Printf(TEXT("Robot %s failed due to: %s"), *RobotProperties.Name, *Reason);
	ShowLogMessage(Message, EMessageColorEnum::ERROR);

}


