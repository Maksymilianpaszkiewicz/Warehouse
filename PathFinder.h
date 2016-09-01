// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "NodeActor.h"
#include "Task.h"
#include "PathFinder.generated.h"

class AWarehouseGameMode;
/* NOTES
	PathFinder can currently only take orders from one OverMind

*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAREHOUSE_API UPathFinder : public UActorComponent
{
	GENERATED_BODY()

public:
	enum States {
		Idle,
		Moving,
		Bidding,
		WaitForOverMind,
		PickingResource,
		DeliveringResource,
		FinishedTask
	};

	// Sets default values for this component's properties
	UPathFinder();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void setTarget(ANodeActor *target) {
		CurrentTarget_ = target;
	}

	void enterAuction(const Task &task);

	void sendBid();

	void grantTask();

	void rejectTask();

	static float straight_distance(ANodeActor *node, ANodeActor *destination);
	/** Chooses a shortest path from start to finnish*/
	static TArray<ANodeActor*> choosePath(ANodeActor *start, ANodeActor *finnish, float *distance = nullptr);

private:
	void moving(float DeltaTime);

	/** Picks next node from CurrentPath_ and sets it as a CurrentTarget_
	If CurrentPath_ is empty and Destination_ is not reached,
	call choosePath() and set new CurrentTarget_*/
	void chooseNextTarget();

	/*Function called after a new target has been set*/
	void choseNewTarget();

	/* Represents the Node where this actor is heading right now */
	UPROPERTY(EditAnywhere, Category = "PathFinding", meta = (AllowPrivateAccess = "true"))
	ANodeActor *CurrentTarget_;
	/* Represents the Node that is the final destination of the actor */
	UPROPERTY(EditAnywhere, Category = "PathFinding", meta = (AllowPrivateAccess = "true"))
	ANodeActor *Destination_;
	/* Represents the last Node that this actor reached */
	ANodeActor *CurrentNode_;
	/* An array of Nodes that lead to the Destination_ (inverted order) */
	TArray<ANodeActor*> CurrentPath_;

	TArray<ANodeActor*> toResource_;
	TArray<ANodeActor*> toFinnish_;

	float Speed_;
	States State_;
	AActor *Owner_;

	Task CurrentTask_;

	AWarehouseGameMode *OverMind_;
};
