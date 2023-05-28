#include "ProceduralGeneration.h"
#include "ProceduralMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/SecureHash.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "PhysicsEngine/BodySetup.h"
#include "EditorFramework/AssetImportData.h"
#include "ProceduralMeshConversion.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMeshSocket.h"
#include "CollisionQueryParams.h"
#include "Materials/Material.h"
#include "KismetProceduralMeshLibrary.h"
#include COMPILED_PLATFORM_HEADER(PlatformTime.h)
#include <CanvasItem.h>


float coefs[11];

AProceduralGeneration::AProceduralGeneration()
{
	PrimaryActorTick.bCanEverTick = false;
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
	MyActor = this;

	srand(time(NULL));
	for (int i = 0; i < 11; i++) {
		coefs[i] = rand() * 1000;
	}
}

float Interpolate(float a, float b, float x);
float Fade(float t);
float dotGradient(int X, float x, int Y = 0, float y = 0, int Z = 0, float z = 0, int W = 0, float w = 0);
FVector4 Random(int X, int Y, int Z, int W);

void AProceduralGeneration::BeginPlay()
{
	Super::BeginPlay();

	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();

	GenerateLand(false);
}

void AProceduralGeneration::GenerateLand(bool useSimpleAlgo)
{
	double StartTime = FPlatformTime::Seconds();

	Vertices.Empty();
	Triangles.Empty();
	UV0.Empty();

	srand(time(NULL));
	for (int i = 0; i < 11; i++) {
		coefs[i] = rand() * 1000;
	}
	CreateVertices(useSimpleAlgo);
	CreateTriangles();

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProceduralMesh->SetMaterial(0, Material);
	
	double EndTime = FPlatformTime::Seconds();

	double ExecutionTime = (EndTime - StartTime) * 1000.0;

	UE_LOG(LogTemp, Warning, TEXT("Час виконання функції: %.2f мс"), ExecutionTime);
}

void AProceduralGeneration::CreateVertices(bool useSimpleAlgo)
{
	const float NoiseOffset = FMath::RandRange(0.f, 10000.f);
	for (int X = 0; X <= XSize; ++X)
	{
		for (int Y = 0; Y <= YSize; ++Y)
		{
			float Z;
			if (useSimpleAlgo) Z = PerlinNoise2D(X, Y, NoiseScale, NoiseOffset) * ZMultiplier; else Z = TwoD_Perlin_Noise(X, Y, NoiseScale, NoiseOffset) * ZMultiplier;
			Vertices.Add(FVector(X * Scale2, Y * Scale2, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

void AProceduralGeneration::CreateTriangles()
{
	int Vertex = 0;

	for (int X = 0; X < XSize; ++X)
	{
		for (int Y = 0; Y < YSize; ++Y)
		{
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);

			++Vertex;
		}
		++Vertex;
	}
}

float AProceduralGeneration::PerlinNoise2D(float X, float Y, float Scale, float NoiseOffset)
{
	int32 X0 = FMath::FloorToInt(X / Scale);
	int32 X1 = X0 + 1;
	int32 Y0 = FMath::FloorToInt(Y / Scale);
	int32 Y1 = Y0 + 1;

	float Sx = X / Scale - X0;
	float Sy = Y / Scale - Y0;

	float Dot00 = FVector2D::DotProduct(Gradient2D(X0, Y0, NoiseOffset), FVector2D(Sx, Sy));
	float Dot10 = FVector2D::DotProduct(Gradient2D(X1, Y0, NoiseOffset), FVector2D(Sx - 1, Sy));
	float Dot01 = FVector2D::DotProduct(Gradient2D(X0, Y1, NoiseOffset), FVector2D(Sx, Sy - 1));
	float Dot11 = FVector2D::DotProduct(Gradient2D(X1, Y1, NoiseOffset), FVector2D(Sx - 1, Sy - 1));

	float InterpolatedX0 = FMath::Lerp(Dot00, Dot10, SmoothStep(Sx));
	float InterpolatedX1 = FMath::Lerp(Dot01, Dot11, SmoothStep(Sx));
	float InterpolatedY = FMath::Lerp(InterpolatedX0, InterpolatedX1, SmoothStep(Sy));

	return InterpolatedY;
}

FVector2D AProceduralGeneration::Gradient2D(int32 X, int32 Y, float NoiseOffset)
{
	float Angle = FMath::Atan2(FMath::Sin(X * 12.9898f + Y * 78.233f + NoiseOffset), FMath::Cos(X * 12.9898f + Y * 78.233f + NoiseOffset));
	return FVector2D(FMath::Cos(Angle), FMath::Sin(Angle));
}

float AProceduralGeneration::SmoothStep(float T)
{
	return T * T * (3 - 2 * T);
}

float AProceduralGeneration::TwoD_Perlin_Noise(float x, float y, float scale, float amplitude) {
	scale = scale <= 0 ? 1 : scale;
	x /= scale;
	y /= scale;
	int xL = floor(x);
	int xU = xL + 1;
	int yL = floor(y);
	int yU = yL + 1;

	float dx = Fade(x - xL);
	float dy = Fade(y - yL);

	return Interpolate(Interpolate(dotGradient(xL, x, yL, y), dotGradient(xU, x, yL, y), dx),
		Interpolate(dotGradient(xL, x, yU, y), dotGradient(xU, x, yU, y), dx), dy) * amplitude;
}
float Interpolate(float a, float b, float x) {
	float t = x * PI;
	float f = (1 - cos(t)) * 0.5;
	return a * (1 - f) + b * f;
}
float dotGradient(int X, float x, int Y, float y, int Z, float z, int W, float w) {
	FVector4 random_vector = Random(X, Y, Z, W);
	float dx = x - X;
	float dy = y - Y;
	float dz = z - Z;
	float dw = w - W;
	return dx * random_vector.X + dy * random_vector.Y + dz * random_vector.Z + dw * random_vector.W;
}
float Fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}
FVector4 Random(int X, int Y, int Z, int W) {
	float seed = coefs[0] * sin(coefs[1] * X + coefs[2] * Y + coefs[3] * Z + coefs[4] * W + coefs[5]) * cos(coefs[6] * X + coefs[7] * Y + coefs[8] * Z + coefs[9] * W + coefs[10]);
	srand(seed);
	float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0)) - 1.0;
	float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0)) - 1.0;
	float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0)) - 1.0;
	float w = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.0)) - 1.0;
	FVector4 random_vector = FVector4(x, y, z, w);
	return random_vector / random_vector.Size();
}

void AProceduralGeneration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

