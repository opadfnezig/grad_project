#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../../../UE_5.1/Engine/Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem/Public/NavMesh/NavMeshBoundsVolume.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralGeneration.generated.h"

class ProceduralMeshComponent;
class UMaterialInterface;

UCLASS(BlueprintType)
class A3681_API AProceduralGeneration : public AActor
{
	GENERATED_BODY()

public:
	AProceduralGeneration();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0), Category = "Generation Settings")
		int XSize = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0), Category = "Generation Settings")
		int YSize = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0), Category = "Generation Settings")
		float ZMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0), Category = "Generation Settings")
		float NoiseScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.000001), Category = "Generation Settings")
		float Scale2 = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Settings")
		UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.000001), Category = "Generation Settings")
		float UVScale = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> Vertices;


protected:
	virtual void BeginPlay() override;

	

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
		void GenerateLand(bool useSimpleAlgo);
	UPROPERTY(BlueprintReadWrite)
		UProceduralMeshComponent* ProceduralMesh;

private:
	AActor* MyActor;
	FVector WorldLocation;
	UMeshComponent* MeshComponent;

	ANavMeshBoundsVolume* NavMeshBoundsVolume = nullptr;

	TArray<int> Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;
	TArray<struct FProcMeshTangent> Tangents;

	float PerlinNoise2D(float x, float y, float scale, float noiseOffset);
	float TwoD_Perlin_Noise(float x, float y, float scale, float amplitude);
	FVector2D Gradient2D(int32 X, int32 Y, float NoiseOffset);
	float SmoothStep(float T);

	void CreateVertices(bool useSimpleAlgo);
	void CreateTriangles();
};
