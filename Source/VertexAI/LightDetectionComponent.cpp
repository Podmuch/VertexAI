// Fill out your copyright notice in the Description page of Project Settings.

#include "VertexAI.h"
#include "LightDetectionComponent.h"


// Sets default values for this component's properties
ULightDetectionComponent::ULightDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    SetComponentTickEnabled(true);

	// ...
}


// Called when the game starts
void ULightDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
    this->timeOfNextLightComponentsUpdate = this->GetWorld()->TimeSeconds + 1.0f;
    this->refreshLightComponentsList();
	// ...
	
}

float ULightDetectionComponent::getLightIntensity()
{    
    if (this->GetWorld()->TimeSeconds > timeOfNextUpdate)
    {
        this->timeOfNextUpdate = this->GetWorld()->TimeSeconds + this->timeBetweenIntensityRefreshes;
        this->result = 0;

        for (auto light : this->lightComponents)
        {            
            if (auto asSkyLight = Cast<USkyLightComponent>(light))
            {
                this->result += this->getLightIntensityOfSkyLight(asSkyLight);
            }            
            else if (auto asSpotLight = Cast<USpotLightComponent>(light))
            {
                this->result += this->getLightIntensityOfSpotLight(asSpotLight);
            }                        
            else if (auto asPointLight = Cast<UPointLightComponent>(light))
            {
                this->result += this->getLightIntensityOfPointLight(asPointLight);
            }            
            else if (auto asDirectionalLight = Cast<UDirectionalLightComponent>(light))
            {
                this->result += this->getLightIntensityOfDirectionalLight(asDirectionalLight);
            }
        }
    }    
    return this->result;
}

float ULightDetectionComponent::getLightIntensityOfSkyLight(USkyLightComponent * skyLight)
{    
    return skyLight->Intensity * 1000;
}

float ULightDetectionComponent::getLightIntensityOfSpotLight(USpotLightComponent * spotLight)
{
    auto lightDirection = spotLight->GetForwardVector();
    auto locationDifference = this->GetOwner()->GetActorLocation() - spotLight->GetComponentLocation();
    auto toPlayerDirection = FRotationMatrix::MakeFromX(locationDifference).Rotator().Vector();
    auto dotProduct = FVector::DotProduct(toPlayerDirection, lightDirection);
    auto angle = acos(dotProduct) * 180 / PI;
    if (angle < spotLight->OuterConeAngle)
    {
        return this->getLightIntensityOfPointLight(spotLight) * this->getSimpleLightlikeInterpolatedValue(angle, spotLight->OuterConeAngle);
    }
    else
    {
        return 0;
    }
}

float ULightDetectionComponent::getLightIntensityOfPointLight(UPointLightComponent * pointLight)
{
    auto distance = this->GetOwner()->GetDistanceTo(pointLight->GetOwner());
    auto radius = pointLight->AttenuationRadius;
    if (distance < radius)
    {
        FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this->GetOwner());
        RV_TraceParams.bTraceComplex = false;
        RV_TraceParams.bTraceAsyncScene = true;
        RV_TraceParams.bReturnPhysicalMaterial = false;
        FHitResult hitResult(ForceInit);

        this->GetWorld()->LineTraceSingleByChannel(
            hitResult,        
            this->GetOwner()->GetActorLocation(),    
            pointLight->GetComponentLocation(), 
            ECC_Visibility,
            RV_TraceParams
        );
        if (hitResult.bBlockingHit == false)
        {
            return pointLight->Intensity * this->getSimpleLightlikeInterpolatedValue(distance, radius);
        }       
    }
    return 0.0f;
}

float ULightDetectionComponent::getLightIntensityOfDirectionalLight(UDirectionalLightComponent * directionalLight)
{
    FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this->GetOwner());
    RV_TraceParams.bTraceComplex = false;
    RV_TraceParams.bTraceAsyncScene = true;
    RV_TraceParams.bReturnPhysicalMaterial = false;
    FHitResult hitResult(ForceInit);

    auto traceStartLocation = this->GetOwner()->GetActorLocation() - directionalLight->GetForwardVector() * 100000;

    this->GetWorld()->LineTraceSingleByChannel(
        hitResult,
        traceStartLocation,
        this->GetOwner()->GetActorLocation(),
        ECC_Visibility,
        RV_TraceParams
    );

    if (hitResult.bBlockingHit)
    {
        return 0;
    }
    else
    {
        return directionalLight->Intensity * 1000;
    }
}

float ULightDetectionComponent::getSimpleLightlikeInterpolatedValue(float distance, float radius) const
{
    auto x = distance / radius * 3.14 / 2;
    if (x < 0.2)
        return 1;
    else
        return pow(cos(x - 0.2), 4);
}

void ULightDetectionComponent::refreshLightComponentsList()
{
    this->lightComponents.Empty();
    for (auto actor : TActorRange<AActor>(this->GetWorld()))
    {
        auto components = actor->GetComponents();

        for (auto component : components)
        {
            auto asLightComponent = Cast<ULightComponentBase>(component);
            if (asLightComponent != nullptr)
            {
                this->lightComponents.Add(asLightComponent);
            }
        }
    }
}


// Called every frame
void ULightDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (this->GetWorld()->TimeSeconds > timeOfNextLightComponentsUpdate || this->GetWorld()->TimeSeconds < 2)
    {
        timeOfNextLightComponentsUpdate = this->GetWorld()->TimeSeconds + this->timeBetweenLightComponentsRefreshes;
        this->refreshLightComponentsList();
    }
}

