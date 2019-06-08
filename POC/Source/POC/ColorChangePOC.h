// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sockets.h"					// For using sockets
#include "Engine/World.h"				// Recent UE4 versions are include what you use
#include "GameFramework/Actor.h"		// Recent UE4 versions are include what you use
#include "ColorChangePOC.generated.h"	// NOTE! *.generated.h must be the last #include

// This UE MACRO makes it possible to hook a value to a Blueprint
// It makes a class hookable by BluePrints called what is passed as an argument
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FColorEvent);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POC_API UColorChangePOC : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UColorChangePOC();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// This is the feature we hook from Eiffel
	void ChangeColor();

	// This creates a feature that a Blueprint can hook onto
	// Since the class FColorEvent was created above using the 
	// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FColorEvent) macro...
	// we can now instantiate an instance of that class. The actual 
	// transition between colors is handled in Blueprint.
	UPROPERTY(BlueprintAssignable)
	FColorEvent OnRed;

	UPROPERTY(BlueprintAssignable)
	FColorEvent OnBlue;

private:
	// This is a convenience so we don't have to repeatedly call 
	// GetOwner() in ColorChangePOC.cpp
	AActor* Owner = GetOwner();

	// Could be a boolean, but lets use an integer to demonstrate
	// type conversion in ColorChangePOC.cpp.
	int8 ColorState = 1;

	UPROPERTY(EditAnywhere)
	float ColorChangeDelay = 6.0f;

	float LastColorChangeTime;

	FSocket* Socket;

	bool connected;
		
};
