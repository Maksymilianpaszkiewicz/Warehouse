// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "NodeActor.h"
#include "Task.h"
#include "WarehouseGameMode.generated.h"

class UPathFinder;

struct CarWrapper {
	explicit CarWrapper(UPathFinder *car = nullptr, float offer = -1.0f) : Car(car), Offer(offer) {
	}

	UPathFinder* Car;
	float Offer;
};

struct CarPredicate {
	bool operator()(const CarWrapper &A, const CarWrapper &B) const {
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

	void enqueueForTask(UPathFinder *car);

	void generateTask();

	Task getTaskFromQueue();

	bool startAuction();

	void acceptBid(UPathFinder *ref, float bid);

	void giveOutTask();

private:
	States State_;

	int numberOfTasks_;
	FTimerHandle taskGeneratorTimer_;
	float taskGeneratorDelay_;
	TQueue<Task> tasksQueue_;

	TMap<NodeActorType, TArray<ANodeActor*>> NodeActorTypesMap_;
	TMap<ResourceType, TArray<ANodeActor*>> resourceNodesMap_;

	TArray<UPathFinder*> waitingForTask_;
	TArray<CarWrapper> auctioners_;

	float timerCount_;
	float taskGiveOutDelay_;
	float biddersWaitTime_;
};
