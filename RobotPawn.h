// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RobotPawn.generated.h"


UCLASS()
class ROBIT_API ARobotPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARobotPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot")
	USkeletalMeshComponent* Character;

	UPROPERTY(EditAnywhere, Category = "Robot")
	TArray<USplineComponent*> Splines;

	TArray<USplineMeshComponent*> SplinePreviews;

	UPROPERTY(EditAnywhere, Category = "Robot")
	TArray<int> SplineProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float Speed = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float MaxSpeed = 400;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float MaxRotationalSpeed = 120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float Friction = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float RotationSpeed = 1.4;

	UPROPERTY(EditAnywhere, Category = "Robot")
	class UMaterialInterface* RobotPreviewSplineMaterial;

	UPROPERTY(EditAnywhere, Category = "Robot")
	class UMaterialInterface* RobotPreviewSplineMaterialBackward;

	UPROPERTY(EditAnywhere, Category = "Robot")
	UStaticMesh* RobotPreviewSplineMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Input functions
	void Move_XAxis(float AxisValue);
	void Rotate_YAxis(float AxisValue);
	void ShowPreview();
	void HidePreview();
	void TogglePreview();

	bool bPreviewShowing = true;

	//Input variables
	float CurrentVelocity = 0;
	float CurrentAcceleration;
	float CurrentRotationVelocity = 0;
	float CurrentRotationAcceleration;
	void AddNewSpline(int Property);
	void AddSplinePoint(FVector Location);

};
