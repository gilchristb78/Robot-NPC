// Fill out your copyright notice in the Description page of Project Settings.


#include "Robot.h"
#include "Engine/World.h"

// Sets default values
ARobot::ARobot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(GetRootComponent());
	Spline->SetClosedLoop(true);

	Character = CreateDefaultSubobject<USkeletalMeshComponent>("Character");
	Character->SetupAttachment(Spline); 


	DistanceAlongSpline = 0;
}

// Called when the game starts or when spawned
void ARobot::BeginPlay()
{
	Super::BeginPlay();
	
	Speed = BaseSpeed;

}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float CharacterZ = Character->GetRelativeLocation().Z + Spline->GetRelativeLocation().Z + GetActorLocation().Z;

	DistanceAlongSpline = fmod(DistanceAlongSpline + (DeltaTime * Speed), Spline->GetSplineLength());
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	
	

	FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	FVector Start = Location;
	Start.Z = CharacterZ + 200;

	FVector End = Location;
	End.Z = CharacterZ - 100;

	FHitResult OutHit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

	if (OutHit.bBlockingHit)
	{
		Location.Z = OutHit.Location.Z;
	}

	if (CharacterZ - Location.Z < 0.1f && CharacterZ - Location.Z > -0.1f)
	{
		Speed = BaseSpeed;
	}
	else if(CharacterZ - Location.Z  <= -0.1f)
	{
		Speed = BaseSpeed * (1.0 - SlopeSpeedModifier);
	}
	else
	{
		Speed = BaseSpeed * (1 + SlopeSpeedModifier);
	}

	Rotation.Yaw -= 90;

	Character->SetWorldLocation(Location);
	Character->SetWorldRotation(Rotation);
	CharacterZ = Location.Z;



	//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceAlongSpline);
}

