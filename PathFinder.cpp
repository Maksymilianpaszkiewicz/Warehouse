// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "WarehouseGameMode.h"
#include "TaskHandler.h"
#include "PathFinder.h"

// Sets default values for this component's properties
UPathFinder::UPathFinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	State_ = Moving;
	Speed_ = 700.0f;
	Owner_ = GetOwner();
	CurrentNode_ = nullptr;
	Activate();
}

// Called when the game starts
void UPathFinder::BeginPlay()
{
	Super::BeginPlay();
	OverMind_ = Cast<AWarehouseGameMode>(GetWorld()->GetAuthGameMode());
	TaskHandler_ = Cast<UTaskHandler>(Owner_->GetComponentByClass(UTaskHandler::StaticClass()));
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
		// Waiting
	}
	break;
	case Moving: {
		moving(DeltaTime);
	}
	break;
	}
	// ...
}

void UPathFinder::setDestination(ANodeActor *destination) {
	Destination_ = destination;
	chooseNextTarget();
	choseNewTarget();
}

void UPathFinder::chooseNextTarget() {
	CurrentTarget_ = nullptr;
	
	if (Destination_ != nullptr) {
		if (CurrentNode_ == Destination_) {
			State_ = Idle;
			TaskHandler_->stepFinished();
		}
		else {
			if (CurrentPath_.Num() == 0) {
				CurrentPath_ = choosePath(CurrentNode_, Destination_);
				if (CurrentPath_.Num() == 0) {
					UE_LOG(LogClass, Log, TEXT("Error, can't find path to a chosen Destination"));
					State_ = Idle;
				}
			}
			CurrentTarget_ = CurrentPath_.Last();
			CurrentPath_.RemoveAt(CurrentPath_.Num() - 1);
			State_ = Moving;
		}
	}
	else {
		State_ = Idle;
	}

	
}

void UPathFinder::choseNewTarget() {
	UActorComponent *actor = Owner_->GetComponentByClass(UTextRenderComponent::StaticClass());
	UTextRenderComponent *text = Cast<UTextRenderComponent>(actor);
	if (text) {
		FString stringText = FString::Printf(TEXT("%s"), *Destination_->GetName());
		text->SetText(FText::FromString(stringText));
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
	if (distance != nullptr) {
		*distance = -1.0f;
	}
	return TArray<ANodeActor*>();
}
