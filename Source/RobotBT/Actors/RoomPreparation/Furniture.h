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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	UArrowComponent* BaseLocation;

	/** Furniture mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* FurnitureMesh;

	/** Where the Furniture need to move */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* CorrectLocationMesh;

	/** Collision that will trigger the movement */
	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	class UBoxComponent* Collision;

	/** Collision that will trigger the movement */
	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	bool InPlance = false;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor GreenColor;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ShouldMove = false;

	/* What happens when in place (change color and disable collision) */
	UFUNCTION()
	void FinishPlacement();

	UFUNCTION()
	void ChangeColor();

	/* Called on tick, to move to the correct location */
	UFUNCTION()
	void MoveToCorrectLocation(float DeltaTime);
};
