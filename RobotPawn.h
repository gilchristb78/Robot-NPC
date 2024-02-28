// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SplineComponent.h"
#include "RobotPawn.generated.h"


UCLASS()
class ROBIT_API ARobotPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARobotPawn();

	UPROPERTY(EditAnywhere)
	USceneComponent* OurVisibleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot")
	USkeletalMeshComponent* Character;

	UPROPERTY(EditAnywhere, Category = "Robot")
	TArray<USplineComponent*> Splines;

	UPROPERTY(EditAnywhere, Category = "Robot")
	TArray<int> SplineProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, Category = "Robot")
	class UMaterialInterface* RobotPreviewSplineMaterial;

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
	void Move_YAxis(float AxisValue);
	void StartGrowing();
	void StopGrowing();

	//Input variables
	float CurrentVelocity;
	float CurrentRotationAmount;
	bool bGrowing;
	int lastNumPoints;

};
