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
	
	FVector CharacterLocation = Character->GetRelativeLocation() + GetActorLocation();

	//UE_LOG(LogTemp, Warning, TEXT("Location: %f + %f + %f = %f"), Character->GetRelativeLocation().X, Spline->GetRelativeLocation().X, GetActorLocation().X, CharacterLocation.X)

	float TempDistanceAlongSpline = fmod(DistanceAlongSpline + (DeltaTime * Speed), Spline->GetSplineLength());
	float TempDistanceAlongSplineChecker = fmod(DistanceAlongSpline + (DeltaTime * Speed * 15), Spline->GetSplineLength());

	FVector Location = Spline->GetLocationAtDistanceAlongSpline(TempDistanceAlongSpline, ESplineCoordinateSpace::World);
	FVector LocationCheck = Spline->GetLocationAtDistanceAlongSpline(TempDistanceAlongSplineChecker, ESplineCoordinateSpace::World);
	FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(TempDistanceAlongSpline, ESplineCoordinateSpace::World);

	FVector Start = CharacterLocation;
	Start.Z += 50;

	FVector End = LocationCheck;
	End.Z = CharacterLocation.Z + 50;
	
	DrawDebugSphere(GetWorld(), CharacterLocation, 30, 15, FColor::Red);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

	if (OutHit.bBlockingHit)
	{

	}
	else
	{
		DistanceAlongSpline = TempDistanceAlongSpline;
		Start = Location;
		Start.Z = CharacterLocation.Z + 50;

		End = Location; 
		End.Z = CharacterLocation.Z - 100;
		GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

		if (OutHit.bBlockingHit)
		{
			Location.Z = OutHit.Location.Z;
		}

		if (CharacterLocation.Z - Location.Z < 0.1f && CharacterLocation.Z - Location.Z > -0.1f)
		{
			Speed = BaseSpeed;
		}
		else if (CharacterLocation.Z - Location.Z <= -0.1f)
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
	}

	



	//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceAlongSpline);
}

