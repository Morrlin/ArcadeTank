#include "ToonTanks/Pawn/PawnTurret.h"
#include "ToonTanks/Pawn/PawnTank.h"
#include "Kismet/GameplayStatics.h"
#include "PawnTank.h"
#include "DrawDebugHelpers.h"
#include "ToonTanks/PlayerController/PlayerControllerBase.h"
#include "ToonTanks/Gamemode/TankGamemodeBase.h"

void APawnTurret::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
	GamemodeRef = Cast<ATankGamemodeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GetOwner())
	{
		TurretLocation = GetOwner()->GetActorLocation();
		TurretRotation = GetOwner()->GetActorRotation();
	}

	TurretComponentLocation = ProjectileSpawnPoint->GetComponentLocation();

	if (GamemodeRef)
	{
		Range = GamemodeRef->GetTurretRange();
		FireRate = GamemodeRef->GetFireRate();
	}

	//UE_LOG(LogTemp, Warning, TEXT("Fire Rate: %f"), FireRate);

	GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &APawnTurret::CheckFireCondition, FireRate, true);
}

void APawnTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerPawn && (GetRangeToTank() < Range))
	{
		RotateTurret(PlayerPawn->GetActorLocation());
	}

	AimedActor = AimTrace.GetActor();
	TurretAimEnd = TurretMesh->GetComponentLocation() + TurretMesh->GetComponentRotation().Vector() * Range;

	/*			Aim Line Trace
	DrawDebugLine
	(
		GetWorld(),
		TurretComponentLocation,
		TurretAimEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0,
		1.f
	);
	*/
}

void APawnTurret::CheckFireCondition()
{
	const FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType
	(
		AimTrace,
		TurretComponentLocation,
		TurretAimEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);

													// AIMED ACTOR NAME
	if (AimedActor && GetOwner())
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Turret is: %s, and Aimed Actor is: %s"),  *GetOwner()->GetName(), *AimedActor->GetName());        
	}


	if (PlayerPawn && PlayerPawn->GetIsPlayerAlive() && (GetRangeToTank() < Range) && (!AimedActor))
	{
		Shoot();
	}
}

float APawnTurret::GetRangeToTank()
{
	if (PlayerPawn)
	{
		return FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
	}
	return 0.f;
}

void APawnTurret::HandleDestruction()
{
	Super::HandleDestruction();

	Destroy();
}

FVector APawnTurret::GetTurretLocation()
{
	return TurretLocation;
}

FRotator APawnTurret::GetTurretRotation()
{
	return TurretRotation;
}

void APawnTurret::SetTurretLocation(FVector DestroyedTurretLocation)
{
	TurretLocation = DestroyedTurretLocation;
}

void APawnTurret::SetTurretRotation(FRotator DestroyedTurretRotation)
{
	TurretRotation = DestroyedTurretRotation;
}
