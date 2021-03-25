#pragma once

#include "CoreMinimal.h"
#include "PawnBase.h"
#include "PawnTurret.generated.h"

class ATankGamemodeBase;

class APawnTank;
UCLASS()
class TOONTANKS_API APawnTurret : public APawnBase
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;

	void CheckFireCondition();

	float GetRangeToTank();

	virtual void HandleDestruction() override;

	FVector GetTurretLocation();

	FRotator GetTurretRotation();

	void SetTurretLocation(FVector DestroyedTurretLocation);

	void SetTurretRotation(FRotator DestroyedTurretRotation);

protected:

	virtual void BeginPlay() override;


private:

	APawnTank* PlayerPawn;

	FTimerHandle FireRateTimer;

	ATankGamemodeBase* GamemodeRef;

	FVector TurretLocation;
	FRotator TurretRotation;

	FVector TurretComponentLocation;
	FHitResult AimTrace;
	AActor* AimedActor;
	FVector TurretAimEnd;

	float Range = 0.f;
	float FireRate = 0.f;
};
