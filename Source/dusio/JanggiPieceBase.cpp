#include "JanggiPieceBase.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr float BasicShapeHalfHeight = 50.0f;
}

AJanggiPieceBase::AJanggiPieceBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PieceMesh"));
	PieceMesh->SetupAttachment(SceneRoot);
	PieceMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PieceMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PieceMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	if (CubeMeshAsset.Succeeded())
	{
		CubeMesh = CubeMeshAsset.Object;
	}

	if (SphereMeshAsset.Succeeded())
	{
		SphereMesh = SphereMeshAsset.Object;
	}

	if (CylinderMeshAsset.Succeeded())
	{
		CylinderMesh = CylinderMeshAsset.Object;
	}

	if (ConeMeshAsset.Succeeded())
	{
		ConeMesh = ConeMeshAsset.Object;
	}

	if (BaseMaterialAsset.Succeeded())
	{
		BaseVisualMaterial = BaseMaterialAsset.Object;
	}

	if (SphereMesh)
	{
		PieceMesh->SetStaticMesh(SphereMesh);
	}

	Team = EJanggiTeam::Red;
	PieceType = EJanggiPieceType::Soldier;
	BoardX = 0;
	BoardY = 0;
	bIsAlive = true;

	RedTeamColor = FLinearColor(0.85f, 0.05f, 0.03f, 1.0f);
	BlueTeamColor = FLinearColor(0.05f, 0.20f, 0.85f, 1.0f);
	PieceScale = FVector(0.6f, 0.6f, 0.35f);

	SetActorScale3D(PieceScale);
}

void AJanggiPieceBase::BeginPlay()
{
	Super::BeginPlay();

	ApplyVisualByTypeAndTeam();
}

void AJanggiPieceBase::InitPiece(EJanggiTeam InTeam, EJanggiPieceType InPieceType, int32 InX, int32 InY)
{
	Team = InTeam;
	PieceType = InPieceType;
	bIsAlive = true;

	SetBoardPosition(InX, InY);
	ApplyVisualByTypeAndTeam();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AJanggiPieceBase::SetBoardPosition(int32 InX, int32 InY)
{
	BoardX = InX;
	BoardY = InY;
}

void AJanggiPieceBase::MoveToWorldLocation(const FVector& TargetLocation)
{
	SetActorLocation(TargetLocation);
}

void AJanggiPieceBase::Capture()
{
	bIsAlive = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AJanggiPieceBase::ApplyVisualByTypeAndTeam()
{
	if (!PieceMesh)
	{
		return;
	}

	if (UStaticMesh* TypeMesh = GetMeshForPieceType())
	{
		PieceMesh->SetStaticMesh(TypeMesh);
	}

	PieceScale = GetScaleForPieceType();
	SetActorScale3D(PieceScale);
	ApplyMeshFloorOffset();

	UMaterialInterface* MaterialSource = BaseVisualMaterial ? BaseVisualMaterial.Get() : PieceMesh->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = MaterialSource
		? UMaterialInstanceDynamic::Create(MaterialSource, this)
		: PieceMesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynamicMaterial)
	{
		return;
	}

	const FLinearColor TeamColor = GetColorForTeam();
	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), TeamColor);
	DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), TeamColor);
	PieceMesh->SetMaterial(0, DynamicMaterial);

	UE_LOG(LogTemp, Log, TEXT("Janggi visual applied: Type=%s Team=%s Coord=(%d,%d) Scale=%s ActorZ=%.2f MeshRelativeZ=%.2f Color=%s"),
		GetPieceTypeText(),
		Team == EJanggiTeam::Red ? TEXT("Red") : TEXT("Blue"),
		BoardX,
		BoardY,
		*PieceScale.ToCompactString(),
		GetActorLocation().Z,
		PieceMesh->GetRelativeLocation().Z,
		*TeamColor.ToString());
}

FVector AJanggiPieceBase::GetScaleForPieceType() const
{
	switch (PieceType)
	{
	case EJanggiPieceType::King:
		return FVector(0.75f, 0.75f, 0.75f);
	case EJanggiPieceType::Guard:
		return FVector(0.45f, 0.45f, 0.35f);
	case EJanggiPieceType::Chariot:
		return FVector(0.55f, 0.35f, 0.25f);
	case EJanggiPieceType::Cannon:
		return FVector(0.45f, 0.45f, 0.30f);
	case EJanggiPieceType::Horse:
		return FVector(0.55f, 0.55f, 0.65f);
	case EJanggiPieceType::Elephant:
		return FVector(0.50f, 0.45f, 0.35f);
	case EJanggiPieceType::Soldier:
		return FVector(0.42f, 0.42f, 0.42f);
	default:
		return FVector(0.5f, 0.5f, 0.5f);
	}
}

UStaticMesh* AJanggiPieceBase::GetMeshForPieceType() const
{
	switch (PieceType)
	{
	case EJanggiPieceType::King:
		return SphereMesh ? SphereMesh.Get() : CubeMesh.Get();
	case EJanggiPieceType::Guard:
		return CubeMesh.Get();
	case EJanggiPieceType::Chariot:
		return CubeMesh.Get();
	case EJanggiPieceType::Cannon:
		return CylinderMesh ? CylinderMesh.Get() : CubeMesh.Get();
	case EJanggiPieceType::Horse:
		return ConeMesh ? ConeMesh.Get() : CubeMesh.Get();
	case EJanggiPieceType::Elephant:
		return CubeMesh.Get();
	case EJanggiPieceType::Soldier:
		return SphereMesh ? SphereMesh.Get() : CubeMesh.Get();
	default:
		return SphereMesh ? SphereMesh.Get() : CubeMesh.Get();
	}
}

FLinearColor AJanggiPieceBase::GetColorForTeam() const
{
	return Team == EJanggiTeam::Red
		? FLinearColor(0.85f, 0.05f, 0.03f, 1.0f)
		: FLinearColor(0.05f, 0.20f, 0.85f, 1.0f);
}

const TCHAR* AJanggiPieceBase::GetPieceTypeText() const
{
	switch (PieceType)
	{
	case EJanggiPieceType::King:
		return TEXT("King");
	case EJanggiPieceType::Guard:
		return TEXT("Guard");
	case EJanggiPieceType::Chariot:
		return TEXT("Chariot");
	case EJanggiPieceType::Cannon:
		return TEXT("Cannon");
	case EJanggiPieceType::Horse:
		return TEXT("Horse");
	case EJanggiPieceType::Elephant:
		return TEXT("Elephant");
	case EJanggiPieceType::Soldier:
		return TEXT("Soldier");
	default:
		return TEXT("Unknown");
	}
}

void AJanggiPieceBase::ApplyMeshFloorOffset()
{
	if (!PieceMesh)
	{
		return;
	}

	PieceMesh->SetRelativeLocation(FVector(0.0f, 0.0f, BasicShapeHalfHeight));
}
