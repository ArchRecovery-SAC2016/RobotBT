#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Furniture.generated.h"

UCLASS()
class ROBOTBT_API AFurniture : public AActor {
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AFurniture();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Root component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Furniture")
	UArrowComponent* BaseLocation;

	/** Furniture mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Furniture")
	UStaticMeshComponent* FurnitureMesh;

	/** Where the Furniture need to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Furniture")
	UStaticMeshComponent* CorrectLocationMesh;

	/** Collision that will trigger the movement */
	UPROPERTY(EditDefaultsOnly, Category = "Furniture")
	class UBoxComponent* Collision;

	/** Collision that will trigger the movement */
	UPROPERTY(EditDefaultsOnly, Category = "Furniture")
	bool InPlace = false;

	UPROPERTY(EditDefaultsOnly, Category = "Furniture")
	FLinearColor RedColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Furniture")
	FLinearColor GreenColor = FLinearColor::Green;

	/* What happens when in place (change color and disable collision) */
	UFUNCTION()
	void SetInPlace(bool NewValue);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// used in tick
	bool ShouldMove = false;

	UFUNCTION()
	void ChangeColor();

	/* Called on tick, to move to the correct location */
	UFUNCTION()
	void MoveToCorrectLocation(float DeltaTime);

	// This is a chace variable set on the BeginPlay. We use when resent the map
	FVector InitialLocation;
	FRotator InitialRotation;
};
