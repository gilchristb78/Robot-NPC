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
	Instructions.Add(Instruction::ForwardMove);
	Details.Add(0);
	
}

// Called every frame
void ARobotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPreviewShowing)
	{
		ProcessMovement(DeltaTime);
	}
	else
	{
		MoveIndependent(DeltaTime);
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
		//SplinePreviews[i]->SetHiddenInGame(true);
	}
	bPreviewShowing = false;
	CurrentInstructionIndex = 0;
	
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

void ARobotPawn::AddNewSpline(Instruction Property)
{
	//Make the spline
	USplineComponent* SplineToAdd = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
	SplineToAdd->ClearSplinePoints();

	//add the values to memory
	Splines.Add(SplineToAdd);
	Instructions.Add(Property);
	Details.Add(Splines.Num() - 1);
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
	if (RobotPreviewSplineMaterial && Instructions[Instructions.Num() - 1] == Instruction::ForwardMove)
	{
		SplineMeshComponent->SetMaterial(0, RobotPreviewSplineMaterial);
	}
	else if (RobotPreviewSplineMaterialBackward && Instructions[Instructions.Num() - 1] == Instruction::BackwardMove)
	{
		SplineMeshComponent->SetMaterial(0, RobotPreviewSplineMaterialBackward);
	}

	if (!bPreviewShowing)
		SplineMeshComponent->SetHiddenInGame(true);
}

void ARobotPawn::ProcessMovement(float DeltaTime)
{
	
	ComputeAccelerations(DeltaTime);

	Instruction CurrentInstruction = Instructions[Instructions.Num() - 1];

	if (CurrentVelocity != 0)
	{
		FRotator NewRotation = Character->GetRelativeRotation() + FRotator(0.0f, CurrentRotationVelocity * DeltaTime, 0.0f);
		Character->SetRelativeRotation(NewRotation);

		FVector LastPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		FVector NewLocation = Character->GetRelativeLocation() + GetActorLocation() + (CurrentVelocity * DeltaTime * Character->GetRightVector());
		Character->SetWorldLocation(NewLocation);

		if (FVector::Distance(NewLocation, LastPoint) > 50.0f)
		{
			if (CurrentVelocity < 0)
			{
				if (!(CurrentInstruction == Instruction::BackwardMove))
					AddNewSpline(Instruction::BackwardMove);
			}
			else
			{
				if (!(CurrentInstruction == Instruction::ForwardMove))
					AddNewSpline(Instruction::ForwardMove);
			}
			AddSplinePoint(NewLocation);
		}
	}
	else if (CurrentRotationVelocity != 0)
	{
		FRotator NewRotation = Character->GetRelativeRotation() + FRotator(0.0f, CurrentRotationVelocity * DeltaTime, 0.0f);
		Character->SetRelativeRotation(NewRotation);
		if (CurrentInstruction != Instruction::Rotate)
		{
			Instructions.Add(Instruction::Rotate);
			Details.Add(CurrentRotationVelocity * DeltaTime);
		}
		else
		{
			if ((Details[Details.Num() - 1] <= 0 && CurrentRotationVelocity * DeltaTime < 0) || (Details[Details.Num() - 1] >= 0 && CurrentRotationVelocity * DeltaTime > 0))
			{
				Details[Details.Num() - 1] += CurrentRotationVelocity * DeltaTime;
			}
			else
			{
				Instructions.Add(Instruction::Rotate);
				Details.Add(CurrentRotationVelocity * DeltaTime);
			}
			
		}
	}
}

void ARobotPawn::ComputeAccelerations(float DeltaTime)
{
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
}

void ARobotPawn::MoveIndependent(float DeltaTime)
{
	int SplineIndex;
	FVector Location;
	FRotator Rotation;
	float DistanceToRotate;

	if (CurrentInstructionIndex > Instructions.Num() - 1)
		return;

	switch (Instructions[CurrentInstructionIndex])
	{
	case Instruction::ForwardMove:

		SplineIndex = Details[CurrentInstructionIndex];
		
		DistanceAlongSpline += DeltaTime * MaxSpeed;
		
		Location = Splines[SplineIndex]->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		Rotation = Splines[SplineIndex]->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		//Location.Z = CharacterLocation.Z; //todo deal with ground (needed?) only for later placed stuff
		Rotation.Yaw -= 90;

		Character->SetWorldLocation(Location);
		Character->SetWorldRotation(Rotation);

		if (DistanceAlongSpline > Splines[SplineIndex]->GetSplineLength())
		{
			CurrentInstructionIndex++;
			DistanceAlongSpline = fmod(DistanceAlongSpline, Splines[SplineIndex]->GetSplineLength());
		}
			

		break;
	case Instruction::BackwardMove:

		SplineIndex = Details[CurrentInstructionIndex];

		DistanceAlongSpline += DeltaTime * MaxSpeed;

		Location = Splines[SplineIndex]->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		Rotation = Splines[SplineIndex]->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		//Location.Z = CharacterLocation.Z; //deal with ground
		Rotation.Yaw += 90;
		//Rotation = Rotation.GetInverse();
		
		
		Character->SetWorldLocation(Location);
		Character->SetWorldRotation(Rotation);

		if (DistanceAlongSpline > Splines[SplineIndex]->GetSplineLength())
		{
			CurrentInstructionIndex++;
			DistanceAlongSpline = fmod(DistanceAlongSpline, Splines[SplineIndex]->GetSplineLength());
		}
			

		break;
	case Instruction::Rotate:

		
		DistanceToRotate = Details[CurrentInstructionIndex] - RotationAroundPoint;

		if (DistanceToRotate < 0)
		{
			DistanceToRotate = FMath::Max(DistanceToRotate, -(MaxRotationalSpeed * DeltaTime));
		}
		else
		{
			DistanceToRotate = FMath::Min(DistanceToRotate, MaxRotationalSpeed * DeltaTime);
		}

		Rotation = Character->GetRelativeRotation() + FRotator(0.0f, DistanceToRotate, 0.0f);
		Character->SetRelativeRotation(Rotation);

		RotationAroundPoint += DistanceToRotate;

		if (RotationAroundPoint == Details[CurrentInstructionIndex])
		{
			RotationAroundPoint = 0;
			CurrentInstructionIndex++;
		}
		
		break;
	default:
		break;
	}


}