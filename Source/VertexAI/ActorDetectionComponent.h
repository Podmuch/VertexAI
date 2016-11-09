// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "ActorDetectionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTEXAI_API UActorDetectionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        float sightRadius = 45;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        float detectionRange = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
        FName tagToDetect = "AIdetectable";

	// Sets default values for this component's properties
	UActorDetectionComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    UFUNCTION(BlueprintCallable, Category = "AI")
        TArray<AActor*> getTaggedActorsInSight() const;
	
    UFUNCTION(BlueprintCallable, Category = "AI")
        bool isActorInSight(AActor* actor) const;

};
