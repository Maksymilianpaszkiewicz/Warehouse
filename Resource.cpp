// Fill out your copyright notice in the Description page of Project Settings.

#include "Warehouse.h"
#include "Resource.h"

TMap<ResourceType, Resource> Resource::Resources_Map;

Resource::~Resource()
{
}
