// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "Resource.h"
#include "NodeResource.h"


void ANodeResource::BeginPlay()
{
	Super::BeginPlay();
	Amount_ = 5;
	resourceRenewDelay_ = 5;
	printInfo();
}

void ANodeResource::renewResource() {
	int index = FMath::RoundToInt(FMath::FRand() * (Resource::Resources_Map.Num() - 1));
	ResourceType_ = ResourceType(index);
	Amount_ = 5;
	SetActorHiddenInGame(false);
	printInfo();
}

void ANodeResource::printInfo() {
	UActorComponent *actor = GetComponentByClass(UTextRenderComponent::StaticClass());
	UTextRenderComponent *text = Cast<UTextRenderComponent>(actor);
	if (text) {
		FString stringText = FString::Printf(TEXT("%d <br> %d"), (int)ResourceType_, Amount_);
		text->SetText(FText::FromString(stringText));
	}
}

void ANodeResource::collect() {
	--Amount_;
	if (Amount_ <= 0) {
		SetActorHiddenInGame(true);
		GetWorldTimerManager().SetTimer(resourceRenewTimer_, this, &ANodeResource::renewResource, resourceRenewDelay_, false);
	}
	printInfo();
}