#include "JanggiTile.h"

#include "Components/StaticMeshComponent.h"
#include "JanggiPieceBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AJanggiTile::AJanggiTile()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	TileMesh->SetupAttachment(SceneRoot);
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (CubeMeshAsset.Succeeded())
	{
		TileMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	if (BaseMaterialAsset.Succeeded())
	{
		BaseVisualMaterial = BaseMaterialAsset.Object;
	}

	TileX = 0;
	TileY = 0;
	bIsOccupied = false;
	CurrentPiece = nullptr;

	NormalColor = FLinearColor(0.72f, 0.72f, 0.72f, 1.0f);
	LightTileColor = FLinearColor(0.78f, 0.78f, 0.78f, 1.0f);
	DarkTileColor = FLinearColor(0.42f, 0.42f, 0.42f, 1.0f);
	HighlightColor = FLinearColor(0.1f, 0.9f, 0.25f, 1.0f);
	SelectedHighlightColor = FLinearColor(1.0f, 0.85f, 0.05f, 1.0f);
	TileScale = FVector(0.95f, 0.95f, 0.05f);

	SetActorScale3D(TileScale);
}

void AJanggiTile::BeginPlay()
{
	Super::BeginPlay();

	ApplyTileColor(GetBaseTileColor());
}

void AJanggiTile::InitTile(int32 InX, int32 InY)
{
	TileX = InX;
	TileY = InY;
	ClearCurrentPiece();
	NormalColor = GetBaseTileColor();
	ApplyTileColor(NormalColor);
}

void AJanggiTile::SetCurrentPiece(AJanggiPieceBase* Piece)
{
	CurrentPiece = Piece;
	bIsOccupied = IsValid(CurrentPiece);
}

void AJanggiTile::ClearCurrentPiece()
{
	CurrentPiece = nullptr;
	bIsOccupied = false;
}

bool AJanggiTile::IsOccupied() const
{
	return bIsOccupied && IsValid(CurrentPiece);
}

void AJanggiTile::SetHighlight(bool bEnable)
{
	ApplyTileColor(bEnable ? HighlightColor : GetBaseTileColor());
}

void AJanggiTile::SetSelectedHighlight(bool bEnable)
{
	ApplyTileColor(bEnable ? SelectedHighlightColor : GetBaseTileColor());
}

FLinearColor AJanggiTile::GetBaseTileColor() const
{
	return ((TileX + TileY) % 2 == 0) ? LightTileColor : DarkTileColor;
}

void AJanggiTile::ApplyTileColor(const FLinearColor& InColor)
{
	if (!TileMesh)
	{
		return;
	}

	UMaterialInterface* MaterialSource = BaseVisualMaterial ? BaseVisualMaterial.Get() : TileMesh->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = MaterialSource
		? UMaterialInstanceDynamic::Create(MaterialSource, this)
		: TileMesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynamicMaterial)
	{
		return;
	}

	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), InColor);
	DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), InColor);
	TileMesh->SetMaterial(0, DynamicMaterial);
}
