#include "TankGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/Pawn/PawnTank.h"
#include "ToonTanks/Pawn/PawnTurret.h"
#include "ToonTanks/PlayerController/PlayerControllerBase.h"

void ATankGamemodeBase::BeginPlay()
{
    Super::BeginPlay();

    PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));
    PlayerControllerRef = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(this, 0));

    HandleGameStart();

    FTimerHandle RefreshHealthTimerHandle;
    FTimerDelegate  RefreshHealthDelegate = FTimerDelegate::CreateUObject(this, &ATankGamemodeBase::RefreshHealth);
    GetWorld()->GetTimerManager().SetTimer(RefreshHealthTimerHandle, RefreshHealthDelegate, 0.5f, true);

    FTimerHandle ReloadTimerHandle;
    FTimerDelegate ReloadDelegate = FTimerDelegate::CreateUObject(this, &ATankGamemodeBase::SetCurrentreloadTime);
    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, ReloadDelegate, 0.005f, true);

    FTimerHandle CountdownTimerHandle;
    FTimerDelegate CountdownDelegate = FTimerDelegate::CreateUObject(this, &ATankGamemodeBase::CheckForCountdown);
    GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, CountdownDelegate, 1.f, true);

    FTimerHandle StartCountdownTimerHandle;
    FTimerDelegate StartCountdownDelegate = FTimerDelegate::CreateUObject(this, &ATankGamemodeBase::PlayStartCountdownSound);
    GetWorld()->GetTimerManager().SetTimer(StartCountdownTimerHandle, StartCountdownDelegate, 0.8f, false);

    TankFullReloadTime = PlayerTank->GetFullReloadTime(); 
    // UE_LOG(LogTemp, Warning, TEXT("Full Reload Time is: %f"), TankFullReloadTime);          Full Reload Time
} 

void ATankGamemodeBase::ActorDied(AActor* DeadActor)
{
    if (DeadActor == PlayerTank)
    {
        PlayerTank->HandleDestruction();
        HandleGameOver(false);

        if (PlayerControllerRef)
        {
            PlayerControllerRef->SetPlayerEnabledState(false);
        }
    }
    else if (APawnTurret* DestroyedTurret = Cast<APawnTurret>(DeadActor))
    {
        if (DestroyedTurret)
        {
            TurretLocationArray.Add(DestroyedTurret->GetTurretLocation());
            TurretRotationArray.Add(DestroyedTurret->GetTurretRotation());

            SetSpawnActorTimer(DestroyedTurret);

            DestroyedTurret->HandleDestruction();

            TurretRange *= UpgradeMultiply;
            TurretFireRate /= UpgradeMultiply;

            /*                             Combat params
            UE_LOG(LogTemp, Warning, TEXT("Turret Range is now: %f"), TurretRange);
            UE_LOG(LogTemp, Warning, TEXT("Turret Firerate is now: %f"), FireRate);
            */

            Score += PointsPerTurret;
            PointsPerTurret *= UpgradeMultiply;

           // UE_LOG(LogTemp, Warning, TEXT("Score: %f"), Score);         Show Score Log
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Occured error while getting access to DestroyedTurret pointer"));
        }
    }
}

void ATankGamemodeBase::HandleGameStart()
{
    GameStart();

    if (PlayerControllerRef)
    {
        PlayerControllerRef->SetPlayerEnabledState(false);

        FTimerHandle PlayerEnableHandle;
        FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerControllerRef, &APlayerControllerBase::SetPlayerEnabledState, true);
        GetWorld()->GetTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);

        FTimerHandle GamePeriodTimerHandle;
        FTimerDelegate GamePeriodDelegate = FTimerDelegate::CreateUObject(this, &ATankGamemodeBase::HandleGameOver, true);
        GetWorld()->GetTimerManager().SetTimer(GamePeriodTimerHandle, GamePeriodDelegate, GamePeriod + StartDelay, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Couldn't find PlayerController"));
    }
}

void ATankGamemodeBase::HandleGameOver(bool IsEndOfTime)
{
    if (!IsFinished)
    {
        PlayerControllerRef->SetPlayerEnabledState(false);
        PlayerTank->HandleDestruction();
        GameOver(IsEndOfTime);

        IsFinished = true;
    }
}

void ATankGamemodeBase::SetSpawnActorTimer(APawnTurret* DestroyedTurret)
{
    if (TurretClass)
    {
        FTimerHandle RespawnTurretTimer;
        FTimerDelegate RespawnTurretDelegate = FTimerDelegate::CreateUObject(this, &ATankGamemodeBase::SpawnActor);
        GetWorld()->GetTimerManager().SetTimer(RespawnTurretTimer, RespawnTurretDelegate, RespawnDelay, false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No Turret Class Found"));
    }
}


void ATankGamemodeBase::SpawnActor()
{
    //UE_LOG(LogTemp, Warning, TEXT("Actor have been spawned !")); Actor spawn state

    TempTurret = GetWorld()->SpawnActor<APawnTurret>(TurretClass, TurretLocationArray[0], TurretRotationArray[0]);
    SetOwner(TempTurret);

    TempTurret->SetTurretLocation(TurretLocationArray[0]);
    TempTurret->SetTurretRotation(TurretRotationArray[0]);

    TurretLocationArray.RemoveAt(0);
    TurretRotationArray.RemoveAt(0);
}

float ATankGamemodeBase::GetRespawnDelay()
{
    return RespawnDelay;
}

void ATankGamemodeBase::SetActorLocationRotation(APawnTurret* NewTurret, FVector NewTurretLocation, FRotator NewtTurretRotation)
{
    NewTurret->SetTurretLocation(NewTurretLocation);
    NewTurret->SetTurretRotation(NewtTurretRotation);

    /*                                           Actor Rotation and Location log
    UE_LOG(LogTemp, Warning, TEXT("%s location: %s"), *GetOwner()->GetName(), *NewTurretLocation.ToString());     
    UE_LOG(LogTemp, Warning, TEXT("%s rotation: %s"), *GetOwner()->GetName(), *NewtTurretRotation.ToString());
    */
}

void ATankGamemodeBase::RefreshHealth()
{
    if (PlayerTank)
    {
        DefaultTankHealth = PlayerTank->TankDefaultHealth;
        CurrentTankHealth = PlayerTank->TankCurrentHealth;
    }
    
    /*                          Get Tank Health
    UE_LOG(LogTemp, Warning, TEXT("Tank Health is: %f"), DefaultTankHealth); 
    UE_LOG(LogTemp, Warning, TEXT("Current Tank Health is: %f"), CurrentTankHealth); 
    */
}

void ATankGamemodeBase::CheckForCountdown()
{
    if (((--GamePeriod) == 5) && CountdownSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), CountdownSound);
    }
}

float ATankGamemodeBase::GetTurretRange()
{
    return TurretRange;
}

float ATankGamemodeBase::GetFireRate()
{
    return TurretFireRate;
}

void ATankGamemodeBase::SetCurrentreloadTime()
{
    TankCurrentReloadTime = PlayerTank->GetCurrentReloadTime();
    //UE_LOG(LogTemp, Warning, TEXT("Current Reload Time is: %f"), TankCurrentReloadTime);              Current Reload Time
}

void ATankGamemodeBase::PlayStartCountdownSound()
{
    if (StartCountdownSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), StartCountdownSound);
    }
}

int32 ATankGamemodeBase::GetScore()
{
    return Score;
}

