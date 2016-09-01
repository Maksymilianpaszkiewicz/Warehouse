// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM()
enum class ResourceType : uint8
{
	A	UMETA(DisplayName = "A"),
	B	UMETA(DisplayName = "B"),
	C	UMETA(DisplayName = "C")
};

/**
 * 
 */
class WAREHOUSE_API Resource
{
public:
	Resource(ResourceType type) : Type_(type) {
	}
	~Resource();

	ResourceType Type_;
	static TMap<ResourceType, Resource> Resources_Map;

	static void initializeMap() {
		Resources_Map.Add(ResourceType::A, Resource(ResourceType::A));
		Resources_Map.Add(ResourceType::B, Resource(ResourceType::B));
		Resources_Map.Add(ResourceType::C, Resource(ResourceType::C));
	}
};
