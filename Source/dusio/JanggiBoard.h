#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JanggiPieceBase.h"
#include "JanggiBoard.generated.h"

class AJanggiTile;

UENUM(BlueprintType)
enum class EJanggiOpeningFormation : uint8
{
	MaSangMaSang UMETA(DisplayName = "Ma Sang Ma Sang"),
	MaSangSangMa UMETA(DisplayName = "Ma Sang Sang Ma"),
	SangMaMaSang UMETA(DisplayName = "Sang Ma Ma Sang"),
	SangMaSangMa UMETA(DisplayName = "Sang Ma Sang Ma")
};

UCLASS()
class DUSIO_API AJanggiBoard : public AActor
{
	GENERATED_BODY()

public:
	AJanggiBoard();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Board", meta = (ClampMin = "1"))
	int32 BoardWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Board", meta = (ClampMin = "1"))
	int32 BoardHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Board", meta = (ClampMin = "1.0"))
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Board")
	bool bCenterBoardOnActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Classes")
	TSubclassOf<AJanggiTile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Classes")
	TSubclassOf<AJanggiPieceBase> PieceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Setup")
	EJanggiOpeningFormation RedFormation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Setup")
	EJanggiOpeningFormation BlueFormation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Board")
	TArray<TObjectPtr<AJanggiTile>> Tiles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Pieces")
	TArray<TObjectPtr<AJanggiPieceBase>> Pieces;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Selection")
	TObjectPtr<AJanggiPieceBase> SelectedPiece;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Turn")
	EJanggiTeam CurrentTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Turn")
	bool bVsAI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Drag")
	TObjectPtr<AJanggiPieceBase> DraggedPiece;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Drag")
	TObjectPtr<AJanggiTile> DragSourceTile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Drag")
	bool bIsDraggingPiece;

	UFUNCTION(BlueprintCallable, Category = "Janggi|Board")
	void GenerateBoard();

	UFUNCTION(BlueprintPure, Category = "Janggi|Board")
	AJanggiTile* GetTileAt(int32 X, int32 Y) const;

	UFUNCTION(BlueprintPure, Category = "Janggi|Board")
	FVector BoardToWorld(int32 X, int32 Y) const;

	UFUNCTION(BlueprintPure, Category = "Janggi|Board")
	bool IsInsideBoard(int32 X, int32 Y) const;

	UFUNCTION(BlueprintCallable, Category = "Janggi|Board")
	void ClearHighlights();

	UFUNCTION(BlueprintCallable, Category = "Janggi|Pieces")
	void SpawnTestPieces();

	UFUNCTION(BlueprintCallable, Category = "Janggi|Pieces")
	void SpawnInitialPieces();

	UFUNCTION(BlueprintPure, Category = "Janggi|Rules")
	TArray<FIntPoint> GetLegalMoves(AJanggiPieceBase* Piece) const;

	UFUNCTION(BlueprintPure, Category = "Janggi|Rules")
	TArray<FString> GetLegalMoveDropdownOptions(AJanggiPieceBase* Piece) const;

	UFUNCTION(BlueprintCallable, Category = "Janggi|Input")
	void OnTileClicked(AJanggiTile* Tile);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Input")
	void OnPieceClicked(AJanggiPieceBase* Piece);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Selection")
	void SelectPiece(AJanggiPieceBase* Piece);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Movement")
	bool MoveSelectedPieceToTile(AJanggiTile* TargetTile);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Movement")
	bool MoveSelectedPieceToBoardPosition(int32 TargetX, int32 TargetY);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Drag")
	bool BeginDragPiece(AJanggiPieceBase* Piece);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Drag")
	void PreviewDraggedPieceAtWorld(const FVector& WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Drag")
	bool FinishDragOnTile(AJanggiTile* TargetTile);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Drag")
	void CancelDrag();

	UFUNCTION(BlueprintPure, Category = "Janggi|Drag")
	bool IsDraggingPiece() const;

	UFUNCTION(BlueprintPure, Category = "Janggi|Drag")
	AJanggiPieceBase* GetDraggedPiece() const;

	UFUNCTION(BlueprintPure, Category = "Janggi|Board")
	FVector GetBoardCenterWorldLocation() const;

protected:
	virtual void BeginPlay() override;

private:
	static constexpr float PieceWorldZOffset = 2.5f;

	void ClearGeneratedActors();
	void SpawnFormationPieces(EJanggiTeam Team, EJanggiOpeningFormation Formation, int32 BackRankY);
	EJanggiPieceType GetFormationPieceType(EJanggiOpeningFormation Formation, int32 SlotIndex) const;
	void AddMoveIfValid(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece, int32 TargetX, int32 TargetY) const;
	void AddPalaceStepMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const;
	void AddStraightMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const;
	void AddCannonMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const;
	void AddHorseMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const;
	void AddElephantMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const;
	void AddSoldierMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const;
	bool CanPieceMoveToTile(const AJanggiPieceBase* Piece, int32 TargetX, int32 TargetY) const;
	bool IsInsideTeamPalace(EJanggiTeam Team, int32 X, int32 Y) const;
	bool IsInsideAnyPalace(int32 X, int32 Y) const;
	bool IsPalaceDiagonalStep(int32 FromX, int32 FromY, int32 ToX, int32 ToY) const;
	bool IsLegalMoveForPiece(AJanggiPieceBase* Piece, int32 TargetX, int32 TargetY) const;
	AJanggiTile* GetTileForPiece(const AJanggiPieceBase* Piece) const;
	AJanggiPieceBase* SpawnPiece(EJanggiTeam Team, EJanggiPieceType PieceType, int32 X, int32 Y);
	const TCHAR* GetTeamText(EJanggiTeam Team) const;
	const TCHAR* GetPieceTypeText(EJanggiPieceType PieceType) const;
	void LogLegalMoveOptions(AJanggiPieceBase* Piece, const TArray<FIntPoint>& LegalMoves) const;
	bool CanSelectPiece(const AJanggiPieceBase* Piece) const;
	void SwitchTurnAfterMove();
};
