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
	
	MoveRobot(DeltaTime);

}

void ARobot::MoveRobot(float DeltaTime)
{
	if (!canProceed(DeltaTime)) //are we running into a wall?
		return;


	FVector CharacterLocation = Character->GetRelativeLocation() + GetActorLocation();	//where is our skeletalmesh?

	DistanceAlongSpline = fmod(DistanceAlongSpline + (DeltaTime * Speed), Spline->GetSplineLength()); //advance
	
	//get next spot to move to
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World); 
	FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FVector Start = Location;
	Start.Z = CharacterLocation.Z + CharacterStepHeight;

	FVector End = Location;
	End.Z = CharacterLocation.Z - (CharacterStepHeight * 2);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);	//find the ground

	if (Debug)
		DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	if (OutHit.bBlockingHit)
	{
		Location.Z = OutHit.Location.Z; //set the ground
	}

	if (CharacterLocation.Z - Location.Z < 0.1f && CharacterLocation.Z - Location.Z > -0.1f)
	{
		Speed = BaseSpeed;
	}
	else if (CharacterLocation.Z - Location.Z <= -0.1f)
	{
		Speed = BaseSpeed * (1.0 - SlopeSpeedModifier);	//Slope Up
	}
	else
	{
		Speed = BaseSpeed * (1 + SlopeSpeedModifier); //Slope Down
	}

	Rotation.Yaw -= 90; //look toward "forward" of spline

	Character->SetWorldLocation(Location);
	Character->SetWorldRotation(Rotation);

}

bool ARobot::canProceed(float DeltaTime)
{
	if(bHittingWall)
		return false;

	FVector CharacterLocation = Character->GetRelativeLocation() + GetActorLocation();
	//could get weird with slow deltatime (running into a wall miles away)
	//TODO change to go from next location (not multiplied) forward 1 then get that vector and multiply by the character depth
	float TempDistanceAlongSplineChecker = fmod(DistanceAlongSpline + (DeltaTime * Speed * CharacterCheckDistance), Spline->GetSplineLength());
	FVector LocationCheck = Spline->GetLocationAtDistanceAlongSpline(TempDistanceAlongSplineChecker, ESplineCoordinateSpace::World);
	FVector Start = CharacterLocation;
	Start.Z += CharacterStepHeight;

	FVector End = LocationCheck;
	End.Z = CharacterLocation.Z + CharacterStepHeight;

	if(Debug)
		DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
	if (OutHit.bBlockingHit)
	{
		bHittingWall = true;
		return false;
	}

	return true;
}

