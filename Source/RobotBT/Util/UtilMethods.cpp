#include "UtilMethods.h"

void UUtilMethods::ShowLogMessage(const FString& Message, EMessageColorEnum Type) {
    FColor Color = FColor::Emerald;

    if (Type == EMessageColorEnum::ERROR) {
        Color = FColor::Red;
    }
    else if (Type == EMessageColorEnum::WARNING) {
        Color = FColor::Yellow;
    }
    else if (Type == EMessageColorEnum::INFO) {
        Color = FColor::Green;
    }
    else if (Type == EMessageColorEnum::SUCCESS) {
        Color = FColor::Blue;
    }

    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, Message);
    }

    UE_LOG(LogTemp, Display, TEXT("[UMyJsonReader::ShowLogMessage] %s"), *Message);
}