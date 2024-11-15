#pragma once

#include "CoreMinimal.h"
#include "SplinePath.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpenChange, bool, NewState);

UCLASS()
class ROBOTBT_API ARoom : public AActor {
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

public:	
	ARoom();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	UArrowComponent* BaseLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Room")
	class UBoxComponent* DoorCollision;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
	TArray<ASplinePath*> Path;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	bool DoorOpened = true;

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	FLinearColor GreenColor;

	UPROPERTY()
	FOnDoorOpenChange OnDoorOpen;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	FVector GetDoorEntrance(int32 PathIndex);

	UFUNCTION()
	void OpenDoor(bool NewValue);

	void ChangeColorToOK(bool NewValue);
private:
	UFUNCTION()
	void ControlDoorOpen();
};
