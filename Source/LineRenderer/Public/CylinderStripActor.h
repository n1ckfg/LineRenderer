// Copyright 2016, Sigurdur Gunnarsson. All Rights Reserved. 
// Example cylinder strip mesh

#pragma once

#include "LineRendererPrivatePCH.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshData.h"
#include "CylinderStripActor.generated.h"

UCLASS()
class LineRenderer_API ACylinderStripActor : public AActor
{
	GENERATED_BODY()

public:
	ACylinderStripActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Parameters")
		TArray<FVector> LinePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Parameters")
		float Radius = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Parameters")
		int32 RadialSegmentCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Parameters")
		bool bSmoothNormals = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Parameters")
		UMaterialInterface* Material;

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif   // WITH_EDITOR

protected:
	UPROPERTY(Transient, DuplicateTransient)
		UProceduralMeshComponent* ProcMesh;

private:
	void GenerateMesh();

	void GenerateCylinder(FProceduralMeshData& MeshData, FVector StartPoint, FVector EndPoint, float InWidth, int32 InCrossSectionCount, int32 InVertexIndexStart, bool bInSmoothNormals = true);

	FVector RotatePointAroundPivot(FVector InPoint, FVector InPivot, FVector InAngles);
	void PreCacheCrossSection();

	int32 LastCachedCrossSectionCount;
	UPROPERTY(Transient)
		TArray<FVector> CachedCrossSectionPoints;
};
