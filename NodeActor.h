// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

UENUM()
enum class NodeActorType : uint8 
{
	Node	UMETA(DisplayName = "Xroad"),
	Exit	UMETA(DisplayName = "Exit")
};

UCLASS()
class WAREHOUSE_API ANodeActor : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ANodeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	TArray<ANodeActor*> &getNeighbours() {
		return Neighbours;
	}

	NodeActorType getNodeType() const {
		return Type_;
	}

protected:

	UPROPERTY(EditAnywhere, Category = "PathFinding")
	NodeActorType Type_;

	UPROPERTY(EditAnywhere, Category = "PathFinding")
	TArray<ANodeActor*> Neighbours;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PathFinding", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *Mesh;


};

