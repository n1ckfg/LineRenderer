// Copyright 2016, Sigurdur Gunnarsson. All Rights Reserved. 
// Example cylinder strip mesh

#include "LineRendererPrivatePCH.h"
#include "CylinderStripActor.h"

ACylinderStripActor::ACylinderStripActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMesh->AttachTo(RootComponent);
	// Make sure the PMC doesnt save any mesh data with the map
	ProcMesh->SetFlags(EObjectFlags::RF_Transient);
}

#if WITH_EDITOR  
void ACylinderStripActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	PreCacheCrossSection();
	GenerateMesh();
}
#endif // WITH_EDITOR

void ACylinderStripActor::BeginPlay()
{
	Super::BeginPlay();
	PreCacheCrossSection();
	GenerateMesh();

	// Fix for PCM location/rotation/scale since the whole component is Transient
	ProcMesh->SetWorldLocation(this->GetActorLocation());
	ProcMesh->SetWorldRotation(this->GetActorRotation());
	ProcMesh->SetWorldScale3D(this->GetActorScale3D());
}

void ACylinderStripActor::GenerateMesh()
{
	if (LinePoints.Num() < 2)
	{
		return;
	}

	// Calculate and pre-allocate buffers
	int32 TotalNumberOfVerticesPerSection = RadialSegmentCount * 4; // 4 verts per face 
	int32 TotalNumberOfTrianglesPerSection = TotalNumberOfVerticesPerSection + 2 * RadialSegmentCount;
	int32 NumberOfSections = LinePoints.Num() - 1;

	FProceduralMeshData MeshData = FProceduralMeshData();
	MeshData.Vertices.AddUninitialized(TotalNumberOfVerticesPerSection * NumberOfSections);
	MeshData.Triangles.AddUninitialized(TotalNumberOfTrianglesPerSection * NumberOfSections);
	MeshData.Normals.AddUninitialized(TotalNumberOfVerticesPerSection * NumberOfSections);
	MeshData.UVs.AddUninitialized(TotalNumberOfVerticesPerSection * NumberOfSections);
	MeshData.Tangents.AddUninitialized(TotalNumberOfVerticesPerSection * NumberOfSections);

	// Now lets loop through all the defined points and create a cylinder between each two defined points
	int32 CurrentIndexStart = 0;

	for (int32 i = 0; i < NumberOfSections; i++)
	{
		CurrentIndexStart = i * TotalNumberOfVerticesPerSection;
		GenerateCylinder(MeshData, LinePoints[i], LinePoints[i + 1], Radius, RadialSegmentCount, CurrentIndexStart, bSmoothNormals);
	}

	ProcMesh->ClearAllMeshSections();
	ProcMesh->CreateMeshSection(0, MeshData.Vertices, MeshData.Triangles, MeshData.Normals, MeshData.UVs, MeshData.VertexColors, MeshData.Tangents, false);
	ProcMesh->SetMaterial(0, Material);
}

FVector ACylinderStripActor::RotatePointAroundPivot(FVector InPoint, FVector InPivot, FVector InAngles)
{
	FVector direction = InPoint - InPivot; // get point direction relative to pivot
	direction = FQuat::MakeFromEuler(InAngles) * direction; // rotate it
	return direction + InPivot; // calculate rotated point
}

void ACylinderStripActor::PreCacheCrossSection()
{
	if (LastCachedCrossSectionCount == RadialSegmentCount)
	{
		return;
	}

	// Generate a cross-section for use in cylinder generation
	const float AngleBetweenQuads = (2.0f / (float)(RadialSegmentCount)) * PI;
	CachedCrossSectionPoints.Empty();

	// Pre-calculate cross section points of a circle, two more than needed
	for (int32 PointIndex = 0; PointIndex < (RadialSegmentCount + 2); PointIndex++)
	{
		float Angle = (float)PointIndex * AngleBetweenQuads;
		CachedCrossSectionPoints.Add(FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0));
	}

	LastCachedCrossSectionCount = RadialSegmentCount;
}

void ACylinderStripActor::GenerateCylinder(FProceduralMeshData& MeshData, FVector StartPoint, FVector EndPoint, float InWidth, int32 InCrossSectionCount, int32 InVertexIndexStart, bool bInSmoothNormals/* = true*/)
{
	// Basic setup
	int32 VertexIndex = InVertexIndexStart;
	int32 NumVerts = InCrossSectionCount * 4; // 4 verts per face

											  // Make a cylinder section
	const float AngleBetweenQuads = (2.0f / (float)(InCrossSectionCount)) * PI;
	const float UMapPerQuad = 1.0f / (float)InCrossSectionCount;

	FVector StartOffset = StartPoint - FVector(0, 0, 0);
	FVector Offset = EndPoint - StartPoint;

	// Find angle between vectors
	FVector LineDirection = (StartPoint - EndPoint);
	LineDirection.Normalize();
	FVector RotationAngle = LineDirection.Rotation().Add(90.f, 0.f, 0.f).Euler();

	// Start by building up vertices that make up the cylinder sides
	for (int32 QuadIndex = 0; QuadIndex < InCrossSectionCount; QuadIndex++)
	{
		float Angle = (float)QuadIndex * AngleBetweenQuads;
		float NextAngle = (float)(QuadIndex + 1) * AngleBetweenQuads;

		// Set up the vertices
		FVector p0 = (CachedCrossSectionPoints[QuadIndex] * InWidth) + StartOffset;
		p0 = RotatePointAroundPivot(p0, StartPoint, RotationAngle);
		FVector p1 = CachedCrossSectionPoints[QuadIndex + 1] * InWidth + StartOffset;
		p1 = RotatePointAroundPivot(p1, StartPoint, RotationAngle);
		FVector p2 = p1 + Offset;
		FVector p3 = p0 + Offset;

		// Set up the quad triangles
		int32 VertIndex1 = VertexIndex++;
		int32 VertIndex2 = VertexIndex++;
		int32 VertIndex3 = VertexIndex++;
		int32 VertIndex4 = VertexIndex++;

		MeshData.Vertices[VertIndex1] = p0;
		MeshData.Vertices[VertIndex2] = p1;
		MeshData.Vertices[VertIndex3] = p2;
		MeshData.Vertices[VertIndex4] = p3;

		// Now create two triangles from those four vertices
		// The order of these (clockwise/counter-clockwise) dictates which way the normal will face. 
		MeshData.Triangles.Add(VertIndex4);
		MeshData.Triangles.Add(VertIndex3);
		MeshData.Triangles.Add(VertIndex1);

		MeshData.Triangles.Add(VertIndex3);
		MeshData.Triangles.Add(VertIndex2);
		MeshData.Triangles.Add(VertIndex1);

		// UVs.  Note that Unreal UV origin (0,0) is top left
		MeshData.UVs[VertIndex1] = FVector2D(1.0f - (UMapPerQuad * QuadIndex), 1.0f);
		MeshData.UVs[VertIndex2] = FVector2D(1.0f - (UMapPerQuad * (QuadIndex + 1)), 1.0f);
		MeshData.UVs[VertIndex3] = FVector2D(1.0f - (UMapPerQuad * (QuadIndex + 1)), 0.0f);
		MeshData.UVs[VertIndex4] = FVector2D(1.0f - (UMapPerQuad * QuadIndex), 0.0f);

		// Normals
		FVector NormalCurrent = FVector::CrossProduct(MeshData.Vertices[VertIndex1] - MeshData.Vertices[VertIndex3], MeshData.Vertices[VertIndex2] - MeshData.Vertices[VertIndex3]).GetSafeNormal();

		if (bInSmoothNormals)
		{
			// To smooth normals we give the vertices different values than the polygon they belong to.
			// GPUs know how to interpolate between those.
			// I do this here as an average between normals of two adjacent polygons
			float NextNextAngle = (float)(QuadIndex + 2) * AngleBetweenQuads;
			FVector p4 = (CachedCrossSectionPoints[QuadIndex + 2] * InWidth) + StartOffset;
			p4 = RotatePointAroundPivot(p4, StartPoint, RotationAngle);

			// p1 to p4 to p2
			FVector NormalNext = FVector::CrossProduct(p1 - p2, p4 - p2).GetSafeNormal();
			FVector AverageNormalRight = (NormalCurrent + NormalNext) / 2;
			AverageNormalRight = AverageNormalRight.GetSafeNormal();

			float PreviousAngle = (float)(QuadIndex - 1) * AngleBetweenQuads;
			FVector pMinus1 = FVector(FMath::Cos(PreviousAngle) * InWidth, FMath::Sin(PreviousAngle) * InWidth, 0.f) + StartOffset;
			pMinus1 = RotatePointAroundPivot(pMinus1, StartPoint, RotationAngle);

			// p0 to p3 to pMinus1
			FVector NormalPrevious = FVector::CrossProduct(p0 - pMinus1, p3 - pMinus1).GetSafeNormal();
			FVector AverageNormalLeft = (NormalCurrent + NormalPrevious) / 2;
			AverageNormalLeft = AverageNormalLeft.GetSafeNormal();

			MeshData.Normals[VertIndex1] = AverageNormalLeft;
			MeshData.Normals[VertIndex2] = AverageNormalRight;
			MeshData.Normals[VertIndex3] = AverageNormalRight;
			MeshData.Normals[VertIndex4] = AverageNormalLeft;
		}
		else
		{
			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			MeshData.Normals[VertIndex1] = MeshData.Normals[VertIndex2] = MeshData.Normals[VertIndex3] = MeshData.Normals[VertIndex4] = NormalCurrent;
		}

		// Tangents (perpendicular to the surface)
		FVector SurfaceTangent = p0 - p1;
		SurfaceTangent = SurfaceTangent.GetSafeNormal();
		MeshData.Tangents[VertIndex1] = MeshData.Tangents[VertIndex2] = MeshData.Tangents[VertIndex3] = MeshData.Tangents[VertIndex4] = FProcMeshTangent(SurfaceTangent, true);
	}
}
