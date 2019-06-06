// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorChangePOC.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UColorChangePOC::UColorChangePOC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UColorChangePOC::BeginPlay()
{
	Super::BeginPlay();
	FString ObjectName = Owner->GetName();
	FString ObjectPos = Owner->GetTransform().GetLocation().ToString();
	UE_LOG(LogTemp, Warning, TEXT("%s is at %s"), *ObjectName, *ObjectPos);
	// ...
	
}

void UColorChangePOC::ChangeColor()
{
	// Eiffel requested a color change. Flip the sign and log in Unreal
	ColorState *= -1;
	FString StateString = FString::FromInt(ColorState);
	UE_LOG(LogTemp, Error, TEXT("Color Change requested. State is now %s"), *StateString);

	// some record keeping to track when to automatically change state after a time delay
	LastColorChangeTime = GetWorld()->GetTimeSeconds();
}


// Called every frame
void UColorChangePOC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Some testing code to automatically swap colors after a set period of time has elapsed.
	if (GetWorld()->GetTimeSeconds() - LastColorChangeTime >= ColorChangeDelay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Color change timeout reached..."));
		ChangeColor();
	}

	if (ColorState > 0)
	{
		// Broadcast serves as the trigger in Blueprint
		OnRed.Broadcast();
	}
	else
	{
		OnBlue.Broadcast();
	}
}

