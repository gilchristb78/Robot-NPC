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
	
}

// Called every frame
void ARobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DistanceAlongSpline = fmod(DistanceAlongSpline + (DeltaTime * Speed), Spline->GetSplineLength());
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	FVector Start = Location;
	Start.Z += 100;

	FVector End = Location;
	End.Z -= 100;

	FHitResult OutHit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

	if (OutHit.bBlockingHit)
	{
		Location.Z = OutHit.Location.Z;
	}

	Rotation.Yaw -= 90;

	Character->SetWorldLocation(Location);
	Character->SetWorldRotation(Rotation);

	//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceAlongSpline);
}

