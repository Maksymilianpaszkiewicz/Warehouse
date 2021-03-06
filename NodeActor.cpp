// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "NodeActor.h"

// Sets default values
ANodeActor::ANodeActor() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Position"));
	RootComponent = Mesh;

}

// Called when the game starts or when spawned
void ANodeActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANodeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

