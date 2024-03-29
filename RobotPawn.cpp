// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotPawn.h"
#include "Math/RotationMatrix.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARobotPawn::ARobotPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Character = CreateDefaultSubobject<USkeletalMeshComponent>("Character");
	Character->SetupAttachment(RootComponent);
	Character->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	//Camera
	UCameraComponent* OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("OurCamera"));
	OurCamera->SetupAttachment(Character);
	OurCamera->SetRelativeLocation(FVector(0.0f,-275.0f, 150.0f));
	OurCamera->SetRelativeRotation(FRotator(-15.0f, 90.0f, 0.0f));

	DistanceAlongSpline = 0;
	RotationAroundPoint = 0;
	CurrentInstructionIndex = 0;
}

// Called when the game starts or when spawned
void ARobotPawn::BeginPlay()
{
	Super::BeginPlay();
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
	Splines[Splines.Num() - 1]->AddSplineWorldPoint(Location);
	addPreview();
}

void ARobotPawn::addPreview()
{
	int previewDist = 10;
	int pointIndex = Splines[Splines.Num() - 1]->GetNumberOfSplinePoints() - 1;

	if (pointIndex % previewDist == 0 && pointIndex != 0) //change to dist or possibly velocity / acceleration
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		SplineMeshComponent->SetStaticMesh(RobotPreviewSplineMesh);
		SplineMeshComponent->SetMobility(EComponentMobility::Static);
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		//start and end points for our mesh
		int32 SplinePoint1 = pointIndex - previewDist;
		int32 SplinePoint2 = pointIndex;

		//location and tangents
		FVector StartPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(SplinePoint1, ESplineCoordinateSpace::Local);
		FVector StartTangent = Splines[Splines.Num() - 1]->GetTangentAtSplinePoint(SplinePoint1, ESplineCoordinateSpace::Local);
		FVector EndPoint = Splines[Splines.Num() - 1]->GetLocationAtSplinePoint(SplinePoint2, ESplineCoordinateSpace::Local);
		FVector EndTangent = Splines[Splines.Num() - 1]->GetTangentAtSplinePoint(SplinePoint2, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent);
		SplinePreviews.Add(SplineMeshComponent);

		if (SplinePreviews.Num() > 1 && SplinePreviews[SplinePreviews.Num() - 2]->GetEndPosition() == StartPoint)
		{
			SplinePreviews[SplinePreviews.Num() - 2]->SetEndTangent(StartTangent);
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
}

void ARobotPawn::ProcessMovement(float DeltaTime)
{
	

	ComputeAccelerations(DeltaTime);

		

	Instruction CurrentInstruction = Instruction::null;
	if(!(Instructions.Num() == 0))
		CurrentInstruction = Instructions[Instructions.Num() - 1];

	if (!canProceed())
		CurrentVelocity *= -0.75;

	if (falling)
	{
		FVector newLocation = GetActorLocation() + (GravityDirection * DeltaTime * 300);
		SetActorLocation(newLocation);
		setRotAndPosNormalToGround();
	}

	if (CurrentVelocity != 0)
	{
		

		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime * GetActorForwardVector());
		FRotator NewRotation = GetActorRotation() + FRotator(0.0f, CurrentRotationVelocity * DeltaTime, 0.0f); //rotate forward vector around up vector
		SetActorLocation(NewLocation);
		SetActorRotation(NewRotation);
		setRotAndPosNormalToGround();

		
		//SetActorLocation(NewLocation);

		

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
	else if (CurrentRotationVelocity != 0)
	{
		float RotateAmount = CurrentRotationVelocity * DeltaTime;
		FRotator NewRotation =  GetActorRotation() + FRotator(0.0f, RotateAmount, 0.0f);
		//Character->SetWorldRotation(NewRotation);
		SetActorRotation(NewRotation);
		setRotAndPosNormalToGround();
		Instruction CurrentAction;
		RotateAmount < 0 ? CurrentAction = Instruction::RotateNegative : CurrentAction = Instruction::Rotate;

		if (CurrentInstruction != CurrentAction)
		{
			Instructions.Add(CurrentAction);
			Details.Add(RotateAmount);
		}
		else
		{
			Details[Details.Num() - 1] += RotateAmount;
		}

	}
}

bool ARobotPawn::canProceed()
{
	FVector Start = GetActorLocation() - GravityDirection * 25;
	//Start.Z += 25;

	FVector End = GetActorLocation() - GravityDirection * 25;
	int direction = ((CurrentVelocity >= 0) * 2) - 1;

	End += GetActorForwardVector() * 50 * direction;
	/*End.Z += 25;*/

	DrawDebugLine(GetWorld(), Start, End, FColor::Blue);

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);

	if (OutHit.bBlockingHit)
	{
		return false; //running into wall
	}

	return true;
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
		return; //or set to 0 for loop

	switch (Instructions[CurrentInstructionIndex])
	{
	case Instruction::ForwardMove:
		SplineIndex = Details[CurrentInstructionIndex];
		
		DistanceAlongSpline++;

		Location = Splines[SplineIndex]->GetLocationAtSplinePoint(DistanceAlongSpline, ESplineCoordinateSpace::World);
		Rotation = Splines[SplineIndex]->GetRotationAtSplinePoint(DistanceAlongSpline, ESplineCoordinateSpace::World);
		//Location.Z = CharacterLocation.Z; //todo deal with ground (needed?) only for later placed stuff

		SetActorLocation(Location);
		SetActorRotation(Rotation);
		setRotAndPosNormalToGround();

		if (DistanceAlongSpline > Splines[SplineIndex]->GetNumberOfSplinePoints())
		{
			CurrentInstructionIndex++;
			DistanceAlongSpline = 0;
		}

		break;
	case Instruction::BackwardMove:
		SplineIndex = Details[CurrentInstructionIndex];

		DistanceAlongSpline++;

		Location = Splines[SplineIndex]->GetLocationAtSplinePoint(DistanceAlongSpline, ESplineCoordinateSpace::World);
		Rotation = Splines[SplineIndex]->GetRotationAtSplinePoint(DistanceAlongSpline, ESplineCoordinateSpace::World);
		//Location.Z = CharacterLocation.Z; //deal with ground
		Rotation.Yaw += 180; //face backwards
		Rotation.Pitch *= -1; //if going up slope forward
		Rotation.Roll *= -1; // ensure we are going down slope backward
		
		SetActorLocation(Location);
		SetActorRotation(Rotation);
		setRotAndPosNormalToGround();

		if (DistanceAlongSpline > Splines[SplineIndex]->GetNumberOfSplinePoints())
		{
			CurrentInstructionIndex++;
			DistanceAlongSpline = 0; //possibly wait at end of instruction
		}

		break;
	case Instruction::Rotate:
		DistanceToRotate = Details[CurrentInstructionIndex] - RotationAroundPoint;
		DistanceToRotate = FMath::Min(DistanceToRotate, MaxRotationalSpeed * DeltaTime);

		Rotation = GetActorRotation();
		Rotation += FRotator(0.0f, DistanceToRotate, 0.0f);

		SetActorRotation(Rotation);

		RotationAroundPoint += DistanceToRotate;

		if (RotationAroundPoint == Details[CurrentInstructionIndex])
		{
			RotationAroundPoint = 0;
			CurrentInstructionIndex++;
		}
		
		break;
	case Instruction::RotateNegative:
		DistanceToRotate = Details[CurrentInstructionIndex] - RotationAroundPoint;
		DistanceToRotate = FMath::Max(DistanceToRotate, -(MaxRotationalSpeed * DeltaTime));

		Rotation = GetActorRotation();
		Rotation += FRotator(0.0f, DistanceToRotate, 0.0f);

		SetActorRotation(Rotation);
		setRotAndPosNormalToGround();

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



void ARobotPawn::unPause()
{
	bIsPaused = false;
}

void ARobotPawn::setRotAndPosNormalToGround()
{
	FVector location = GetActorLocation();

	FVector Start = location - GravityDirection * 25; //would be gravity up vector not actors
	//Start.Z += 25;

	FVector End = location + GravityDirection * 50; // same
	//End.Z -= 50 

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
	if (OutHit.bBlockingHit)
	{
		location = OutHit.Location;

		FRotator rotation = FRotationMatrix::MakeFromZX(OutHit.ImpactNormal, GetActorForwardVector()).Rotator(); 
		//set friction based on impactnormal angle away from upvector
		//todo figure out the matrix math
		SetActorRotation(rotation);
		SetActorLocation(location);
		falling = false;
	}
	else
	{
		falling = true;
		UE_LOG(LogTemp, Warning, TEXT("FALL From %f, To %f"), location.Z, (location + GravityDirection * 50).Z);
	}
}

