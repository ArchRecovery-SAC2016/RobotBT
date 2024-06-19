#pragma once

#include "CoreMinimal.h"
#include "RoomTrash.h"
#include "GameFramework/Actor.h"
#include "DoorSensor.generated.h"

class AFurniturePlace;

UCLASS()
class ROBOTBT_API ADoorSensor : public AActor {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ADoorSensor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	int32 RoomId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	bool Opened = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	class UBoxComponent* Collision;

	// saves the furnitures of this room
	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	TArray<AFurniturePlace*> FurnituresPlace;

	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	TArray<ARoomTrash*> RoomTrash;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor GreenColor;

	UFUNCTION()
	FVector3d GetNextClearPosition();

	UFUNCTION()
	FVector3d GetNextForniturePosition();

	UFUNCTION(BlueprintCallable)
	bool IsPrepared();

	UFUNCTION()
	bool CheckFornitureIsArranged();

	UFUNCTION()
	bool CheckIsRooomClean();

private:
	void ChangeColor(bool NewValue);

	
};
