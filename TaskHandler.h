// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Task.h"
#include "TaskHandler.generated.h"

class AWarehouseGameMode;
class UPathFinder;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAREHOUSE_API UTaskHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTaskHandler();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void enterAuction(const Task &task);

	void grantTask();

	void rejectTask();

	void stepFinished();

private:

	void calculateBestCost();

	AActor *Owner_;
	AWarehouseGameMode *OverMind_;
	UPathFinder *PathFinder_;
	// Current task that is being handled
	Task CurrentTask_;
	float TaskCost_;
	// A queue of steps needed to acomplish task
	TQueue<TaskStep> CurrentTaskSteps_;
	TQueue<TaskStep> PotentialNewSteps_;

	bool ReadyForNewStep_;
	bool EnqueuedForTask_;
};
