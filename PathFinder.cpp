// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "WarehouseGameMode.h"
#include "PathFinder.h"

// Sets default values for this component's properties
UPathFinder::UPathFinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	State_ = DeliveringResource;
	Speed_ = 500.0f;
	Owner_ = GetOwner();
	CurrentNode_ = nullptr;
	Activate();
}

// Called when the game starts
void UPathFinder::BeginPlay()
{
	Super::BeginPlay();
	OverMind_ = Cast<AWarehouseGameMode>(GetWorld()->GetAuthGameMode());
	// ...
}

void UPathFinder::moving(float DeltaTime) {
	FVector my_pos = Owner_->GetActorLocation();
	FVector target = CurrentTarget_->GetActorLocation();
	target.Z = my_pos.Z;
	FVector direction = (target - my_pos).GetSafeNormal();
	FRotator rot = direction.Rotation();
	Owner_->SetActorRotation(FMath::Lerp(Owner_->GetActorRotation(), rot, 0.05f));
	float distance = (my_pos - target).Size();
	float movement = DeltaTime * Speed_;
	/* Check whether we are close enough to the target
	(distance is smaller than a distance to move in one tick)*/
	if (distance < movement) {
		CurrentNode_ = CurrentTarget_;
		chooseNextTarget();
	}
	else {
		Owner_->SetActorLocation(my_pos + (direction * movement));
	}
}

// Called every frame
void UPathFinder::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	switch (State_) {
	case Idle: {
		// Waiting for overmind to start auction for task
	}
	break;
	case WaitForOverMind: {
		// Waiting for overmind answer to your auction bid
	}
	break;
	case FinishedTask: {
		// After finishing current task, tell overmind that you are ready for next task
		OverMind_->enqueueForTask(this);
		State_ = Idle;
	}
	break;
	case Bidding: {
		// Auction was started, sending my bid
		sendBid();
	}
	break;
	case PickingResource: {
		moving(DeltaTime);
	}
	break;
	case DeliveringResource: {
		moving(DeltaTime);
	}
	break;
	}
	// ...
}

void UPathFinder::enterAuction(const Task &task) {
	CurrentTask_ = Task(task.Type_, task.Exit_, task.Resource_);
	State_ = Bidding;
}

void UPathFinder::sendBid() {
	float bestbid = -1;
	float bid, temp;
	/// Calculating best bid
	TArray<ANodeActor*> toResource;
	TArray<ANodeActor*> toFinnish;
	ANodeActor *closestResource = nullptr;
	for (ANodeActor *resource : *(CurrentTask_.Resource_)) {
		bid = 0;
		toResource = choosePath(CurrentNode_, resource, &temp);
		bid += temp;
		toFinnish = choosePath(resource, CurrentTask_.Exit_, &temp);
		bid += temp;
		if (bestbid != -1) {
			if (bestbid > bid) {
				bestbid = bid;
				closestResource = resource;
			}
		}
		else {
			bestbid = bid;
			closestResource = resource;
		}
	}
	if (bestbid != -1) {
		toResource_ = choosePath(CurrentNode_, closestResource);
		toFinnish_ = choosePath(closestResource, CurrentTask_.Exit_);
		OverMind_->acceptBid(this, bestbid);
		State_ = WaitForOverMind;
	}
	else {
	/// cant get to the resource 
		UE_LOG(LogClass, Log, TEXT("Can't get to resource %d"), (int)CurrentTask_.Type_);
		State_ = Idle;
	}
}

void UPathFinder::grantTask() {
	State_ = PickingResource;
	Destination_ = nullptr;
	chooseNextTarget();
}

void UPathFinder::rejectTask() {
	Destination_ = nullptr;
	State_ = Idle;
}

void UPathFinder::chooseNextTarget() {
	CurrentTarget_ = nullptr;
	
	if (Destination_ == nullptr) {
		
		if (State_ == PickingResource) {
			CurrentPath_ = toResource_;
		}
		else if (State_ == DeliveringResource) {
			CurrentPath_ = toFinnish_;
		}

		if (CurrentPath_.Num() != 0) {
			Destination_ = CurrentPath_[0];
			choseNewTarget();
		}
		else {
			if (State_ == PickingResource) {
				State_ = DeliveringResource;
			}
			else {
				State_ = FinishedTask;
			}
			return;
		}
	}
	if (Destination_ != CurrentNode_) {
		if (CurrentPath_.Num() != 0) {
			CurrentTarget_ = CurrentPath_.Last();
			CurrentPath_.RemoveAt(CurrentPath_.Num() - 1);
		}
		else {
			//Destination can't be reached from CurrentNode_
			UE_LOG(LogClass, Log, TEXT("Error, can't find path to a chosen Destination"));
			State_ = FinishedTask;
		}
	}
	else {
		Destination_ = nullptr;
		if (State_ == PickingResource) {
			State_ = DeliveringResource;
			chooseNextTarget();
		}
		else {
			State_ = FinishedTask;
		}
	}
	
}

void UPathFinder::choseNewTarget() {
	UActorComponent *actor = Owner_->GetComponentByClass(UTextRenderComponent::StaticClass());
	UTextRenderComponent *text = Cast<UTextRenderComponent>(actor);
	if (text) {
		text->SetText(Destination_->GetName() + "\n" + (int)CurrentTask_.Type_);
	}
}

float UPathFinder::straight_distance(ANodeActor *node, ANodeActor *destination) {
	return (node->GetActorLocation() - destination->GetActorLocation()).Size();
}


struct NodeWrapper {
	NodeWrapper(ANodeActor *node = nullptr) : Node(node) {
	}
	bool operator==(const NodeWrapper &B) const {
		return Node == B.Node;
	}
	bool operator==(const ANodeActor *B) const {
		return Node == B;
	}
	ANodeActor* Node;
};

struct NodePredicate {
	NodePredicate(TMap<ANodeActor*, float> *fScore) : fScore_(fScore) {
	}
	TMap<ANodeActor*, float> *fScore_;
	bool operator()(const NodeWrapper &A, const NodeWrapper &B) const {
		return (*fScore_)[A.Node] < (*fScore_)[B.Node];
	}
};


TArray<ANodeActor*> UPathFinder::choosePath(ANodeActor *start, ANodeActor *finnish, float *distance) {

	if (start == nullptr || finnish == nullptr) {
		return TArray<ANodeActor*>();
	}

	TArray<NodeWrapper> openArray;
	TArray<ANodeActor*> closedArray;
	TMap<ANodeActor*, ANodeActor*> cameFrom;
	TMap<ANodeActor*, float> gScore;
	TMap<ANodeActor*, float> fScore;
	
	NodePredicate predicate(&fScore);

	ANodeActor *currentNode = start;
	openArray.HeapPush(NodeWrapper(currentNode), predicate);
	gScore.Add(currentNode, 0);
	fScore.Add(currentNode, straight_distance(currentNode, finnish));
	
	NodeWrapper node;
	while (openArray.Num() != 0) {
		openArray.HeapPop(node, predicate);
		currentNode = node.Node;

		if (currentNode == finnish) {
			//We found our target, return the path
			TArray<ANodeActor*> path;
			if (distance != nullptr) {
				*distance = gScore[finnish];
			}
			while (currentNode != start) {
				path.Add(currentNode);
				currentNode = cameFrom[currentNode];
			}
			return path;
		}
		closedArray.Add(currentNode);
		for (ANodeActor *neighbour : currentNode->getNeighbours()) {
			if (closedArray.Contains(neighbour)) {
				continue;
			}
			float tentative_gScore = gScore[currentNode] + straight_distance(currentNode, neighbour);
			if (!openArray.Contains(neighbour) || (tentative_gScore < gScore[neighbour])) {
				cameFrom.Add(neighbour, currentNode);
				gScore.Add(neighbour, tentative_gScore);
				fScore.Add(neighbour, gScore[neighbour] + straight_distance(neighbour, finnish));

				openArray.HeapPush(NodeWrapper(neighbour), predicate);
				//openArray.Add(NodeWrapper(neighbour));
			}
		}
	}
	return TArray<ANodeActor*>();
}
