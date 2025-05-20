#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigableArea.generated.h"


/**
 * Base class for all navigable areas in the game.
 * Used to hide or show the roof
 */
UCLASS()
class ROBOTBT_API ANavigableArea : public AActor {
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

public:	
	ANavigableArea();

	/* Put all Base Meshes here */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* BasecMeshes;

	/* Put all Roof meshes here */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RoofMeshes;

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void HideRoof(bool NewValue);


};
