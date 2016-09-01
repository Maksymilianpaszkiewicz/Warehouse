// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NodeActor.h"
/**
 * 
 */
class WAREHOUSE_API Task
{
public:
	Task(ResourceType type, ANodeActor *Exit = nullptr, TArray<ANodeActor*> *Resource = nullptr) : Exit_(Exit), Resource_(Resource), Type_(type) {
	}
	Task(ANodeActor *Exit = nullptr, TArray<ANodeActor*> *Resource = nullptr) : Exit_(Exit), Resource_(Resource) {
	}
	Task(const Task &other) : Exit_(other.Exit_), Resource_(other.Resource_), Type_(other.Type_) {
	}

	Task &operator=(const Task &other) {
		Exit_ = other.Exit_;
		Resource_ = other.Resource_;
		Type_ = other.Type_;
		return *this;
	}

	~Task();
	ANodeActor *Exit_;
	TArray<ANodeActor*> *Resource_;
	ResourceType Type_;
};
