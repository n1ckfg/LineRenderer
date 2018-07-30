// Copyright 2016, Sigurdur Gunnarsson. All Rights Reserved. 
// Simple struct to keep all the mesh data

#pragma once

#include "ProceduralMeshesPrivatePCH.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshData.generated.h"

USTRUCT()
struct PROCEDURALMESHES_API FProceduralMeshData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<int32> Triangles;

	UPROPERTY()
	TArray<FVector> Normals;

	UPROPERTY()
	TArray<FVector2D> UVs;

	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;

	UPROPERTY()
	TArray<FColor> VertexColors;
};