#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JanggiTile.generated.h"

class AJanggiPieceBase;
class UMaterialInterface;

UCLASS()
class DUSIO_API AJanggiTile : public AActor
{
	GENERATED_BODY()

public:
	AJanggiTile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Components")
	TObjectPtr<UStaticMeshComponent> TileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Board")
	int32 TileX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Board")
	int32 TileY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Tile")
	bool bIsOccupied;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Tile")
	TObjectPtr<AJanggiPieceBase> CurrentPiece;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor NormalColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor LightTileColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor DarkTileColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor HighlightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor SelectedHighlightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FVector TileScale;

	UFUNCTION(BlueprintCallable, Category = "Janggi|Tile")
	void InitTile(int32 InX, int32 InY);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Tile")
	void SetCurrentPiece(AJanggiPieceBase* Piece);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Tile")
	void ClearCurrentPiece();

	UFUNCTION(BlueprintPure, Category = "Janggi|Tile")
	bool IsOccupied() const;

	UFUNCTION(BlueprintCallable, Category = "Janggi|Tile")
	void SetHighlight(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Tile")
	void SetSelectedHighlight(bool bEnable);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseVisualMaterial;

	FLinearColor GetBaseTileColor() const;
	void ApplyTileColor(const FLinearColor& InColor);
};
