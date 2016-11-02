// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "LightDetectionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VAULTRUNNER_API ULightDetectionComponent : public UActorComponent
{
    GENERATED_BODY()
        UPROPERTY()
        TArray<ULightComponentBase*> lightComponents;

    UPROPERTY()
        float result = 0.0f;
    UPROPERTY()
        float timeOfNextUpdate = 0;
    UPROPERTY()
        float timeOfNextLightComponentsUpdate = 0;

public:	
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Light Detection")
        float timeBetweenLightComponentsRefreshes = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Light Detection")
        float timeBetweenIntensityRefreshes = 0.5f;

	// Sets default values for this component's properties
	ULightDetectionComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
    UFUNCTION(BlueprintCallable, Category = "Light Detection")
        virtual float getLightIntensity();

    virtual float getLightIntensityOfSkyLight(USkyLightComponent* skyLight);

    virtual float getLightIntensityOfSpotLight(USpotLightComponent* spotLight);

    virtual float getLightIntensityOfPointLight(UPointLightComponent* pointLight);

    virtual float getLightIntensityOfDirectionalLight(UDirectionalLightComponent* directionalLight);

    virtual float getSimpleLightlikeInterpolatedValue(float distance, float radius) const;

    virtual void refreshLightComponentsList();

	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

		
	
};
