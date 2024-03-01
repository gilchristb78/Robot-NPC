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

	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();

	Splines[0] = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
	Splines[0]->SetupAttachment(RootComponent);
	Splines[0]->ClearSplinePoints();
	SplineProperties.Add(1);
	Splines[0]->AddSplineWorldPoint(Character->GetRelativeLocation() + GetActorLocation());
	
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

		if (FVector::Distance(NewLocation, Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World)) > 50.0f)
		{
			if (!(CurrentVelocity > 0 && SplineProperties[SplineProperties.Num() - 1] == 1 || CurrentVelocity < 0 && SplineProperties[SplineProperties.Num() - 1] == -1))
			{
				if (CurrentVelocity > 0)
				{
					AddNewSpline(1);
				}
				else
				{
					AddNewSpline(-1);
				}
			} 
			AddSplinePoint(NewLocation);
			
		}
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

void ARobotPawn::AddNewSpline(int Property)
{
	Splines.Add(NewObject<USplineComponent>(this, USplineComponent::StaticClass())); //also draw the last deb for this (maybe find a way to make em connect?)
	Splines[Splines.Num() - 1]->SetupAttachment(RootComponent);
	Splines[Splines.Num() - 1]->ClearSplinePoints();
	SplineProperties.Add(Property);
	FVector LastPoint = Splines[Splines.Num() - 2]->GetLocationAtSplinePoint(
		Splines[Splines.Num() - 2]->GetNumberOfSplinePoints() - 1,
		ESplineCoordinateSpace::World
	);
	Splines[Splines.Num() - 1]->AddSplineWorldPoint(LastPoint);
}

void ARobotPawn::AddSplinePoint(FVector Location)
{
	Splines[Splines.Num() - 1]->AddSplineWorldPoint(Location);

	USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		
	SplineMeshComponent->SetStaticMesh(RobotPreviewSplineMesh);

	SplineMeshComponent->SetMobility(EComponentMobility::Movable);
	SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
	SplineMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	int32 SplinePoint1 = Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 2;
	int32 SplinePoint2 = Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 1;

	FVector StartPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(SplinePoint1, ESplineCoordinateSpace::World) - GetActorLocation();
	FVector StartTangent = Splines[Splines.Num() - 1]->GetTangentAtSplinePoint(SplinePoint1, ESplineCoordinateSpace::Local);
	FVector EndPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(SplinePoint2, ESplineCoordinateSpace::World) - GetActorLocation();
	FVector EndTangent = Splines[Splines.Num() - 1]->GetTangentAtSplinePoint(SplinePoint2, ESplineCoordinateSpace::Local);

	SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
		
	if (SplinePreviews.Num() > 0)
	{
		SplinePreviews[SplinePreviews.Num() - 1]->SetEndTangent(StartTangent);
	}

	SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (RobotPreviewSplineMaterial && SplineProperties[SplineProperties.Num() - 1] == 1)
	{
		SplineMeshComponent->SetMaterial(0, RobotPreviewSplineMaterial);
	}
	else if (RobotPreviewSplineMaterialBackward && SplineProperties[SplineProperties.Num() - 1] == -1)
	{
		SplineMeshComponent->SetMaterial(0, RobotPreviewSplineMaterialBackward);
	}
	SplinePreviews.Add(SplineMeshComponent);
}

//TODO: Draw the closest one aswell but overwrite it when the next one gets drawn.
//Probably need the array now
