// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "WarehouseGameMode.h"
#include "PathFinder.h"
#include "NodeResource.h"
#include "TaskHandler.h"


// Sets default values for this component's properties
UTaskHandler::UTaskHandler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	ReadyForNewStep_ = true;
	EnqueuedForTask_ = false;
	Owner_ = GetOwner();
	Activate();
	// ...
}


// Called when the game starts
void UTaskHandler::BeginPlay()
{
	Super::BeginPlay();
	OverMind_ = Cast<AWarehouseGameMode>(GetWorld()->GetAuthGameMode());
	PathFinder_ = Cast<UPathFinder>(Owner_->GetComponentByClass(UPathFinder::StaticClass()));
	// ...
}

// Called every frame
void UTaskHandler::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (ReadyForNewStep_) {
		if (!CurrentTaskSteps_.IsEmpty()) {
			ReadyForNewStep_ = false;
			TaskStep current_step;
			CurrentTaskSteps_.Dequeue(current_step);
			switch (current_step.getStepType()) {
			case TaskStep::MoveTo: {
				PathFinder_->setDestination(current_step.getDestination());
			} break;
			case TaskStep::Collect: {
				//// Collecting
				ANodeResource *resource_node = Cast<ANodeResource>(current_step.getDestination());
				if (resource_node) {
					resource_node->collect();
				}
				ReadyForNewStep_ = true;
			} break;
			}
		}
		else {
			if (!EnqueuedForTask_) {
				OverMind_->enqueueForTask(this);
				EnqueuedForTask_ = true;
			}
		}
	}
	// ...
}

void UTaskHandler::stepFinished() {
	ReadyForNewStep_ = true;
}

void UTaskHandler::enterAuction(const Task &task) {
	CurrentTask_ = Task(task.Type_, task.Exit_);
	calculateBestCost();
}

void UTaskHandler::calculateBestCost() {
	TaskCost_ = 0.0f;
	bool foundPath = false;
	float cost, temp;
	/// Calculating best bid
	ANodeActor *closestResource = nullptr;

	TArray<ANodeResource*> nodesWithResource = OverMind_->getNodesWithResource(CurrentTask_.Type_);
	ANodeActor *currentNode = PathFinder_->getCurrentNode();
	for (ANodeResource *resource : nodesWithResource) {
		cost = 0;
		if (currentNode == resource) {
			temp = 0;
		}
		else {
			UPathFinder::choosePath(currentNode, resource, &temp);
		}
		if (temp < 0) { // negative value indicates that node can't be reached
			continue;
		}
		cost += temp;
		UPathFinder::choosePath(resource, CurrentTask_.Exit_, &temp);
		if (temp < 0) { // negative value indicates that node can't be reached
			continue;
		}
		cost += temp;
		if (!foundPath) {
			foundPath = true;
			TaskCost_ = cost;
			closestResource = resource;
		}
		else {
			if (TaskCost_ > cost) {
				TaskCost_ = cost;
				closestResource = resource;
			}
		}
	}
	if (foundPath) {
		PotentialNewSteps_.Enqueue(TaskStep(TaskStep::MoveTo, closestResource));
		PotentialNewSteps_.Enqueue(TaskStep(TaskStep::Collect, closestResource));
		PotentialNewSteps_.Enqueue(TaskStep(TaskStep::MoveTo, CurrentTask_.Exit_));
		OverMind_->acceptBid(this, TaskCost_);
	}
	else {
		/// cant get to the resource 
		UE_LOG(LogClass, Log, TEXT("Can't get to resource %d"), (int)CurrentTask_.Type_);
	}
}

void UTaskHandler::grantTask() {
	EnqueuedForTask_ = false;
	while (!PotentialNewSteps_.IsEmpty()) {
		TaskStep step;
		PotentialNewSteps_.Dequeue(step);
		CurrentTaskSteps_.Enqueue(step);
	}
	ReadyForNewStep_ = true;
}

void UTaskHandler::rejectTask() {
	PotentialNewSteps_.Empty();
}

