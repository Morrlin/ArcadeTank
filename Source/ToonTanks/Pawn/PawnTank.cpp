#include "ToonTanks/Pawn/PawnTank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

class APawnBase;

APawnTank::APawnTank()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(TurretMesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	/*                       Show Tank Health
	UE_LOG(LogTemp, Warning, TEXT("Tank Health is: %f"), TankDefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("Current Tank Health is: %f"), TankCurrentHealth);
	*/
}

void APawnTank::BeginPlay()
{ 
	Super::BeginPlay();

	PlayerControllerRef = Cast<APlayerController>(GetController());
}

void APawnTank::HandleDestruction()
{
	Super::HandleDestruction();
	
	bIsPlayerAlive = false;

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

bool APawnTank::GetIsPlayerAlive()
{
	return bIsPlayerAlive;
}

float APawnTank::GetFullReloadTime()
{
	return FullReloadTime;
}

float APawnTank::GetCurrentReloadTime()
{
	return CurrentReloadTime;
}

void APawnTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
	Rotate();

	if (PlayerControllerRef)
	{
		FHitResult TraceHitResult;
		PlayerControllerRef->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult);
		FVector HitLocation = TraceHitResult.ImpactPoint;

		RotateTurret(HitLocation);
	} 
	/*                          Tank Health
	UE_LOG(LogTemp, Warning, TEXT("Tank Health is: %f"), TankDefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("Current Tank Health is: %f"), TankCurrentHealth); 
	*/

	CurrentReloadTime = GetWorld()->GetTimeSeconds() - TimeLastShotten;
}

void APawnTank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APawnTank::CalculateMoveInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawnTank::CalculateRotateInput);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APawnTank::CheckIsReloaded);
}

void APawnTank::CalculateMoveInput(float Value)
{
	MoveDirection = FVector(Value * MovementSpeed * GetWorld()->DeltaTimeSeconds, 0, 0);
}

void APawnTank::CalculateRotateInput(float Value)
{
	float RotateAmount = Value * RotationSpeed * GetWorld()->DeltaTimeSeconds;
	FRotator Rotation(0, RotateAmount, 0);
	RotationDirection = FQuat(Rotation);
}

void APawnTank::CheckIsReloaded()
{
	if (CurrentReloadTime > FullReloadTime) 
	{
		Shoot();
		TimeLastShotten = GetWorld()->GetTimeSeconds();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Reloading ! %f seconds left"), FullReloadTime - (GetWorld()->GetTimeSeconds() - TimeLastShotten));
	}
}

void APawnTank::Move()
{
	AddActorLocalOffset(MoveDirection, true);
}

void APawnTank::Rotate()
{
	AddActorLocalRotation(RotationDirection, true);
}

