// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotPawn.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARobotPawn::ARobotPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Character
	Character = CreateDefaultSubobject<USkeletalMeshComponent>("Character");
	Character->SetupAttachment(RootComponent);

	//Camera
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	OurCamera->SetupAttachment(Character);
	OurCamera->SetRelativeLocation(FVector(0.0f, -275.0f, 150.0f));
	OurCamera->SetRelativeRotation(FRotator(-15.0f, 90.0f, 0.0f));
}

// Called when the game starts or when spawned
void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();

	Splines[0] = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
	Splines[0]->ClearSplinePoints();
	Splines[0]->AddSplineWorldPoint(Character->GetRelativeLocation() + GetActorLocation());
	SplineProperties.Add(1);
	
}

// Called every frame
void ARobotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentRotationAcceleration != 0)
	{
		CurrentRotationVelocity = FMath::Clamp(CurrentRotationVelocity + CurrentRotationAcceleration * DeltaTime, -MaxRotationalSpeed, MaxRotationalSpeed);
	}
	else
	{
		if (CurrentRotationVelocity > 0)
		{
			CurrentRotationVelocity = FMath::Clamp(CurrentRotationVelocity -= RotationSpeed * DeltaTime * Friction, 0, MaxRotationalSpeed);
		}
		else if (CurrentRotationVelocity < 0)
		{
			CurrentRotationVelocity = FMath::Clamp(CurrentRotationVelocity += RotationSpeed * DeltaTime * Friction, -MaxRotationalSpeed, 0);
		}
	}

	if (CurrentRotationVelocity != 0)
	{
		
		FRotator NewRotation = Character->GetRelativeRotation() + FRotator(0.0f, CurrentRotationVelocity * DeltaTime, 0.0f);
		Character->SetRelativeRotation(NewRotation);
	}

	if (CurrentAcceleration != 0)
	{
		CurrentVelocity = FMath::Clamp(CurrentVelocity + CurrentAcceleration * DeltaTime, -MaxSpeed, MaxSpeed);
	}
	else
	{
		if (CurrentVelocity > 0)
		{
			CurrentVelocity = FMath::Clamp(CurrentVelocity -= Speed * DeltaTime * Friction, 0, MaxSpeed);
		}
		else if (CurrentVelocity < 0)
		{
			CurrentVelocity = FMath::Clamp(CurrentVelocity += Speed * DeltaTime * Friction, -MaxSpeed, 0);
		}
		
	}

	if (CurrentVelocity != 0)
	{
		FVector LastPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		FVector NewLocation = Character->GetRelativeLocation() + GetActorLocation() + (CurrentVelocity * DeltaTime * Character->GetRightVector());
		Character->SetWorldLocation(NewLocation);

		if (FVector::Distance(NewLocation, LastPoint) > 50.0f)
		{
			int NewProperty = 0;
			//forward or backward
			CurrentVelocity < 0 ? NewProperty = -1 : NewProperty = 1;
			//different direction?
			if (!(SplineProperties[SplineProperties.Num() - 1] == NewProperty))
			{
				AddNewSpline(NewProperty); 
			} 
			AddSplinePoint(NewLocation);
		}
	}
}

// Called to bind functionality to input
void ARobotPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &ARobotPawn::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &ARobotPawn::Rotate_YAxis);

	InputComponent->BindAction("TogglePreview", IE_Pressed, this, &ARobotPawn::TogglePreview);
}

void ARobotPawn::Move_XAxis(float AxisValue)
{
	// Move at 100 units per second forward or backward
	CurrentAcceleration = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.0f * Speed;
}

void ARobotPawn::Rotate_YAxis(float AxisValue)
{
	CurrentRotationAcceleration = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 90.0f * RotationSpeed;
}

void ARobotPawn::ShowPreview()
{
	for (int i = 0; i < SplinePreviews.Num(); i++)
	{
		SplinePreviews[i]->SetHiddenInGame(false);
	}
	bPreviewShowing = true;
}

void ARobotPawn::HidePreview()
{
	for (int i = 0; i < SplinePreviews.Num(); i++)
	{
		SplinePreviews[i]->SetHiddenInGame(true);
	}
	bPreviewShowing = false;
}

void ARobotPawn::TogglePreview()
{
	if (bPreviewShowing)
	{
		HidePreview();
	}
	else
	{
		ShowPreview();
	}
}

void ARobotPawn::AddNewSpline(int Property)
{
	//Make the spline
	USplineComponent* SplineToAdd = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
	SplineToAdd->ClearSplinePoints();

	//add the values to memory
	Splines.Add(SplineToAdd);
	SplineProperties.Add(Property);
}

void ARobotPawn::AddSplinePoint(FVector Location)
{
	//add the point
	Splines[Splines.Num() - 1]->AddSplineWorldPoint(Location);

	//make the mesh
	USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
	SplineMeshComponent->SetStaticMesh(RobotPreviewSplineMesh);
	SplineMeshComponent->SetMobility(EComponentMobility::Movable);
	SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
	SplineMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SplinePreviews.Add(SplineMeshComponent);

	//start and end points for our mesh
	int32 SplinePoint1 = Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 2;
	int32 SplinePoint2 = Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 1;

	//location and tangents
	FVector StartPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(SplinePoint1, ESplineCoordinateSpace::World) - GetActorLocation();
	FVector StartTangent = Splines[Splines.Num() - 1]->GetTangentAtSplinePoint(SplinePoint1, ESplineCoordinateSpace::Local);
	FVector EndPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(SplinePoint2, ESplineCoordinateSpace::World) - GetActorLocation();
	FVector EndTangent = Splines[Splines.Num() - 1]->GetTangentAtSplinePoint(SplinePoint2, ESplineCoordinateSpace::Local);

	SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
		
	//now that weve added a new mesh we have to fix our last tangent
	if (SplinePreviews.Num() > 0)
	{
		SplinePreviews[SplinePreviews.Num() - 1]->SetEndTangent(StartTangent);
	}
	
	//set material based on property
	if (RobotPreviewSplineMaterial && SplineProperties[SplineProperties.Num() - 1] == 1)
	{
		SplineMeshComponent->SetMaterial(0, RobotPreviewSplineMaterial);
	}
	else if (RobotPreviewSplineMaterialBackward && SplineProperties[SplineProperties.Num() - 1] == -1)
	{
		SplineMeshComponent->SetMaterial(0, RobotPreviewSplineMaterialBackward);
	}

	if (!bPreviewShowing)
		SplineMeshComponent->SetHiddenInGame(true);
}
