
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
        // Verificar se a variável GoToPosition está definida
        if (GoToPosition != FVector::ZeroVector) {
            // Calcular o vetor de direção para o movimento
            FVector Direction = (GoToPosition - GetActorLocation()).GetSafeNormal();

            // Calcular a distância a ser percorrida neste quadro
            float DistanceThisFrame = 100 * DeltaTime;

            // Verificar se a distância até a posição final é menor que a distância a ser percorrida neste quadro
            if (FVector::Dist(GetActorLocation(), GoToPosition) <= DistanceThisFrame) {
                // Se a distância for menor ou igual, apenas mova para a posição final
                SetActorLocation(GoToPosition);
                ShouldMove = false; // Parar de mover
            } else {
                // Caso contrário, mova gradualmente em direção à posição final
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

