// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "NodeActor.h"
#include "Task.h"
#include "WarehouseGameMode.generated.h"

class UTaskHandler;

struct Auctioneer {
	explicit Auctioneer(UTaskHandler *executor = nullptr, float offer = -1.0f) : Executor_(executor), Offer(offer) {
	}

	UTaskHandler* Executor_;
	float Offer;
};

struct AuctioneerPredicate {
	bool operator()(const Auctioneer &A, const Auctioneer &B) const {
		return A.Offer < B.Offer;
	}
};

/**
 * 
 */
UCLASS()
class WAREHOUSE_API AWarehouseGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	enum States {
		WaitingForNewTask,
		BeginAuction,
		WaitingForBidders
	};

	AWarehouseGameMode();

	virtual void BeginPlay() override;

	void Tick(float DeltaTime);

	void enqueueForTask(UTaskHandler *executor);

	void generateTask();

	Task getTaskFromQueue();

	bool startAuction();

	void acceptBid(UTaskHandler *ref, float bid);

	void giveOutTask();

	// returns array of Nodes that contain specified Resource
	TArray<ANodeActor*> getNodesWithResource(ResourceType type);

private:
	States State_;

	int numberOfTasks_;
	FTimerHandle taskGeneratorTimer_;
	float taskGeneratorDelay_;
	TQueue<Task> tasksQueue_;

	TMap<NodeActorType, TArray<ANodeActor*>> NodeActorTypesMap_;
	TMap<ResourceType, TArray<ANodeActor*>> ResourceNodesMap_;

	TArray<UTaskHandler*> waitingForTask_;
	TArray<Auctioneer> auctioners_;

	float timerCount_;
	float taskGiveOutDelay_;
	float biddersWaitTime_;
};
