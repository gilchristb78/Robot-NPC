// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotPawn.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARobotPawn::ARobotPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a dummy root component we can attach things to.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	// Create a camera and a visible object
	Character = CreateDefaultSubobject<USkeletalMeshComponent>("Character");
	Character->SetupAttachment(RootComponent);


	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// Attach our camera and visible object to our root component. Offset and rotate the camera.
	OurCamera->SetupAttachment(Character);
	OurCamera->SetRelativeLocation(FVector(0.0f, -275.0f, 150.0f));
	OurCamera->SetRelativeRotation(FRotator(-15.0f, 90.0f, 0.0f));
	OurVisibleComponent->SetupAttachment(RootComponent);

	

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(RootComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();

	Spline->ClearSplinePoints();
	lastNumPoints = 0;

	
}

// Called every frame
void ARobotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentScale = OurVisibleComponent->GetComponentScale().X;
	if (bGrowing)
	{
		// Grow to double size over the course of one second
		CurrentScale += DeltaTime;
	}
	else
	{
		// Shrink half as fast as we grow
		CurrentScale -= (DeltaTime * 0.5f);
	}
	// Make sure we never drop below our starting size, or increase past double size.
	CurrentScale = FMath::Clamp(CurrentScale, 1.0f, 2.0f);
	OurVisibleComponent->SetWorldScale3D(FVector(CurrentScale));

	if (CurrentRotationAmount != 0)
	{
		FRotator NewRotation = Character->GetRelativeRotation() + FRotator(0.0f, CurrentRotationAmount * DeltaTime, 0.0f);
		Character->SetRelativeRotation(NewRotation);
	}

// Handle movement based on our "MoveX" and "MoveY" axes
	if (CurrentVelocity != 0)
	{
		FVector NewLocation = Character->GetRelativeLocation() + GetActorLocation() + (CurrentVelocity * DeltaTime * Character->GetRightVector());
		Character->SetWorldLocation(NewLocation);
	}

	int NumPoints = Spline->GetNumberOfSplinePoints();

	if (NumPoints != lastNumPoints)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spline Points: %d"), NumPoints);
		
		lastNumPoints = NumPoints;
	}

	for (int i = 0; i < NumPoints; i++)
	{
		FVector PointLocation = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);

		DrawDebugSphere(GetWorld(), PointLocation, 30, 20, FColor::Red);
	}
	

}

// Called to bind functionality to input
void ARobotPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Respond when our "Grow" key is pressed or released.
	InputComponent->BindAction("Grow", IE_Pressed, this, &ARobotPawn::StartGrowing);
	InputComponent->BindAction("Grow", IE_Released, this, &ARobotPawn::StopGrowing);

	// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveX", this, &ARobotPawn::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &ARobotPawn::Move_YAxis);

	InputComponent->BindAction("MakePoint", IE_Pressed, this, &ARobotPawn::MakePoint);

}

void ARobotPawn::Move_XAxis(float AxisValue)
{
	// Move at 100 units per second forward or backward
	CurrentVelocity = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f * Speed;
}

void ARobotPawn::Move_YAxis(float AxisValue)
{
	CurrentRotationAmount = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 90.0f * RotationSpeed;
}

void ARobotPawn::StartGrowing()
{
	bGrowing = true;
}

void ARobotPawn::StopGrowing()
{
	bGrowing = false;
}

void ARobotPawn::MakePoint()
{
	FVector PointLoc = Character->GetRelativeLocation() + GetActorLocation();
	Spline->AddSplineWorldPoint(PointLoc);
	UE_LOG(LogTemp, Warning, TEXT("Point Made @ %f, %f, %f"), PointLoc.X, PointLoc.Y, PointLoc.Z);
}

