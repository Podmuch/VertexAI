// Fill out your copyright notice in the Description page of Project Settings.

#include "VertexAI.h"
#include "ActorDetectionComponent.h"


// Sets default values for this component's properties
UActorDetectionComponent::UActorDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActorDetectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UActorDetectionComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

TArray<AActor*> UActorDetectionComponent::getTaggedActorsInSight() const
{
    TArray<AActor*> result;
    for (auto actor : TActorRange<AActor>(this->GetWorld()))
    {
        if (actor->ActorHasTag(this->tagToDetect) && this->isActorInSight(actor))
        {
            result.Add(actor);
        }
    }
    return result;
}

bool UActorDetectionComponent::isActorInSight(AActor* actor) const
{
    if (FVector::Dist(this->GetComponentLocation(), actor->GetActorLocation()) > this->detectionRange)
    {
        return false;
    }

    auto locationDifference = actor->GetActorLocation() - this->GetComponentLocation();
    auto eyeDirection = this->GetForwardVector();
    auto toActorDirection = FRotationMatrix::MakeFromX(locationDifference).Rotator().Vector();
    auto dotProduct = FVector::DotProduct(toActorDirection, eyeDirection);
    auto angle = acos(dotProduct) * 180 / PI;

    if (angle > this->sightRadius)
    {
        return false;
    }

    FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this->GetOwner());
    RV_TraceParams.bTraceComplex = true;
    RV_TraceParams.bTraceAsyncScene = true;
    RV_TraceParams.bReturnPhysicalMaterial = false;
    FHitResult hitResult(ForceInit);

    this->GetWorld()->LineTraceSingleByChannel(
        hitResult,
        actor->GetActorLocation(),
        this->GetComponentLocation(),
        ECC_Visibility,
        RV_TraceParams
        );
    if (hitResult.bBlockingHit == true)
    {
        return false;
    }

    return true;
}

