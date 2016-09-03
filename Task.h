// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NodeActor.h"
/**
 * 
 */

class TaskStep{
public:
	enum StepType {
		MoveTo,
		Collect
	};

	TaskStep(StepType step_type, ANodeActor *destination) : StepType_(step_type), Destination_(destination) {
	}
	TaskStep(const TaskStep &other) : StepType_(other.StepType_), Destination_(other.Destination_) {
	}
	TaskStep() {
	}

	StepType getStepType() {
		return StepType_;
	}

	ANodeActor *getDestination() {
		return Destination_;
	}

private:
	StepType StepType_;
	ANodeActor *Destination_;
};



class WAREHOUSE_API Task
{
public:
	Task(ResourceType type, ANodeActor *Exit = nullptr) : Exit_(Exit), Type_(type) {
	}
	Task(ANodeActor *Exit = nullptr) : Exit_(Exit) {
	}
	Task(const Task &other) : Exit_(other.Exit_), Type_(other.Type_) {
	}

	Task &operator=(const Task &other) {
		Exit_ = other.Exit_;
		Type_ = other.Type_;
		return *this;
	}

	~Task();
	ANodeActor *Exit_;
	ResourceType Type_;
};
