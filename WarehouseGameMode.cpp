// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "WarehouseGameMode.h"
#include "TaskHandler.h"

AWarehouseGameMode::AWarehouseGameMode() {
	PrimaryActorTick.bCanEverTick = true;

	numberOfTasks_ = 0;
	taskGeneratorDelay_ = 1;
	taskGiveOutDelay_ = 1;
	biddersWaitTime_ = 0.5f;
	timerCount_ = taskGiveOutDelay_;
	State_ = WaitingForNewTask;
}

void AWarehouseGameMode::BeginPlay() {
	Super::BeginPlay();

	Resource::initializeMap();

	NodeActorTypesMap_.Add(NodeActorType::Node, TArray<ANodeActor*>());
	NodeActorTypesMap_.Add(NodeActorType::Exit, TArray<ANodeActor*>());
	
	TArray<ResourceType> types;
	Resource::Resources_Map.GenerateKeyArray(types);
	for (ResourceType type : types) {
		ResourceNodesMap_.Add(type, TArray<ANodeActor*>());
	}
	
	for (TActorIterator<ANodeActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ANodeActor *Node = *ActorItr;
		NodeActorTypesMap_[Node->getNodeType()].Add(Node);
		for (ResourceType type : Node->getResourceTypes()) {
			ResourceNodesMap_[type].Add(Node);
		}
	}
	
	if (NodeActorTypesMap_[NodeActorType::Exit].Num() != 0) {
		GetWorldTimerManager().SetTimer(taskGeneratorTimer_, this, &AWarehouseGameMode::generateTask, taskGeneratorDelay_, false);
	}
	
	UE_LOG(LogClass, Log, TEXT("Size of Exit array: %d"), NodeActorTypesMap_[NodeActorType::Exit].Num());
}

void AWarehouseGameMode::enqueueForTask(UTaskHandler *task_executor) {
	if (!waitingForTask_.Contains(task_executor)) {
		waitingForTask_.Add(task_executor);
	}
}

Task AWarehouseGameMode::getTaskFromQueue() {
	Task task;
	if (tasksQueue_.Dequeue(task)) {
		return task;
	}
	else {
		return Task();
	}
}

bool AWarehouseGameMode::startAuction() {
	auctioners_.Empty();
	if (!tasksQueue_.IsEmpty() && waitingForTask_.Num() != 0) {
		Task t;
		tasksQueue_.Peek(t);
		for (UTaskHandler *task_executor : waitingForTask_) {
			task_executor->enterAuction(t);
		}
		UE_LOG(LogClass, Log, TEXT("Starting acution to pick up %d to %s"), (int)t.Type_, *(t.Exit_->GetName()));
		return true;
	}
	UE_LOG(LogClass, Log, TEXT("No one is waiting for new task"));
	return false;
}

void AWarehouseGameMode::acceptBid(UTaskHandler *ref, float bid) {
	auctioners_.HeapPush(Auctioneer(ref, bid), AuctioneerPredicate());
}

void AWarehouseGameMode::giveOutTask() {
	if (auctioners_.Num() != 0) {
		Task t;
		tasksQueue_.Dequeue(t);
		Auctioneer winner;
		auctioners_.HeapPop(winner, AuctioneerPredicate());
		winner.Executor_->grantTask();
		waitingForTask_.RemoveSingle(winner.Executor_);
		for (Auctioneer loser : auctioners_) {
			loser.Executor_->rejectTask();
		}
		UE_LOG(LogClass, Log, TEXT("winner of the task to pick up %d to %s is %s"), (int)t.Type_ ,*(t.Exit_->GetName()), *(winner.Executor_->GetOwner()->GetName()));
	}
}

void AWarehouseGameMode::generateTask() {
	if (numberOfTasks_ < 20) {
		if (NodeActorTypesMap_[NodeActorType::Exit].Num() > 0 && Resource::Resources_Map.Num() > 0) {
			int exitIndex = FMath::RoundToInt(FMath::FRand() * (NodeActorTypesMap_[NodeActorType::Exit].Num() - 1));
			int ResourceIndex = FMath::RoundToInt(FMath::FRand() * (Resource::Resources_Map.Num() - 1));
			tasksQueue_.Enqueue(Task(ResourceType(ResourceIndex), NodeActorTypesMap_[NodeActorType::Exit][exitIndex]));
			//UE_LOG(LogClass, Log, TEXT("added task with Node: %d"), nodeIndex);
		}
	}
	GetWorldTimerManager().SetTimer(taskGeneratorTimer_, this, &AWarehouseGameMode::generateTask, taskGeneratorDelay_, false);
}

TArray<ANodeActor*> AWarehouseGameMode::getNodesWithResource(ResourceType type) {
	return ResourceNodesMap_[type];
}

// Called every frame
void AWarehouseGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timerCount_ -= DeltaTime;
	switch (State_) {
	case WaitingForNewTask: {
		// Waiting to start new auction
		if (timerCount_ <= 0) {
			if (startAuction()) {
				State_ = WaitingForBidders;
				timerCount_ = biddersWaitTime_;
			}
			else {
				timerCount_ = taskGiveOutDelay_;
			}
		}
	}
	break;
	case WaitingForBidders: {
		// Waiting to get offers from all auctioneers
		if (timerCount_ <= 0) {
			giveOutTask();
			State_ = WaitingForNewTask;
			timerCount_ = taskGiveOutDelay_;
		}
	}
	break;
	}
}
