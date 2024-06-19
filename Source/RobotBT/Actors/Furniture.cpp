
#include "Furniture.h"

#include "Robot.h"
#include "Components/BoxComponent.h"

AFurniture::AFurniture() {
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(BaseMesh);
}

void AFurniture::BeginPlay() {
	Super::BeginPlay();

    if (Collision) {
        Collision->OnComponentBeginOverlap.AddDynamic(this, &AFurniture::OnOverlapBegin);
    }
	
}

void AFurniture::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

    if (ShouldMove) {
        // Verificar se a vari�vel GoToPosition est� definida
        if (GoToPosition != FVector::ZeroVector) {
            // Calcular o vetor de dire��o para o movimento
            FVector Direction = (GoToPosition - GetActorLocation()).GetSafeNormal();

            // Calcular a dist�ncia a ser percorrida neste quadro
            float DistanceThisFrame = 100 * DeltaTime;

            // Verificar se a dist�ncia at� a posi��o final � menor que a dist�ncia a ser percorrida neste quadro
            if (FVector::Dist(GetActorLocation(), GoToPosition) <= DistanceThisFrame) {
                // Se a dist�ncia for menor ou igual, apenas mova para a posi��o final
                SetActorLocation(GoToPosition);
                ShouldMove = false; // Parar de mover
            } else {
                // Caso contr�rio, mova gradualmente em dire��o � posi��o final
                SetActorLocation(GetActorLocation() + Direction * DistanceThisFrame);
            }
        }
    }
}

void AFurniture::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	ARobot* Robot = Cast<ARobot>(OtherActor);

	// marca que coletou o lixo e desaparece com o mesh
	if (Robot != nullptr) {
		ShouldMove = true;

	}
}

