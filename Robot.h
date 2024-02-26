// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Robot.generated.h"


UCLASS()
class ROBIT_API ARobot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARobot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot")
	USkeletalMeshComponent* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Robot")
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float BaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float SlopeSpeedModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	bool Debug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float CharacterStepHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	float CharacterCheckDistance;



	float Speed;

	float DistanceAlongSpline;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	void MoveRobot(float DeltaTime);
	bool canProceed(float DeltaTime);
	bool bHittingWall = false;
	bool bIsFalling = false;
	bool isFalling(float DeltaTime);
	

};
