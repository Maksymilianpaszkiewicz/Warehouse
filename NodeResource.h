// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NodeActor.h"
#include "Resource.h"
#include "NodeResource.generated.h"

/**
 * 
 */
UCLASS()
class WAREHOUSE_API ANodeResource : public ANodeActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	ResourceType getResourceType() {
		return ResourceType_;
	}

	void renewResource();

	void collect();
protected:
	UPROPERTY(EditAnywhere, Category = "PathFinding")
	ResourceType ResourceType_;

	void printInfo();

	int Amount_;
	int PredictedAmount_;
	
	FTimerHandle resourceRenewTimer_;
	float resourceRenewDelay_;
};
