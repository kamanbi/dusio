#include "JanggiBoard.h"

#include "Engine/World.h"
#include "JanggiPieceBase.h"
#include "JanggiTile.h"

namespace
{
	const TCHAR* ToJanggiPieceTypeText(EJanggiPieceType PieceType)
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
}

AJanggiBoard::AJanggiBoard()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BoardWidth = 9;
	BoardHeight = 10;
	TileSize = 100.0f;
	bCenterBoardOnActor = true;

	TileClass = AJanggiTile::StaticClass();
	PieceClass = AJanggiPieceBase::StaticClass();
	RedFormation = EJanggiOpeningFormation::MaSangMaSang;
	BlueFormation = EJanggiOpeningFormation::MaSangMaSang;
	CurrentTurn = EJanggiTeam::Red;
	bVsAI = false;
	SelectedPiece = nullptr;
	DraggedPiece = nullptr;
	DragSourceTile = nullptr;
	bIsDraggingPiece = false;
}

void AJanggiBoard::BeginPlay()
{
	Super::BeginPlay();

	GenerateBoard();
	SpawnInitialPieces();

	UE_LOG(LogTemp, Log, TEXT("Janggi turn initialized: CurrentTurn=%s bVsAI=%s"),
		GetTeamText(CurrentTurn),
		bVsAI ? TEXT("true") : TEXT("false"));
}

void AJanggiBoard::GenerateBoard()
{
	if (!GetWorld() || !TileClass)
	{
		return;
	}

	ClearGeneratedActors();
	SelectedPiece = nullptr;
	DraggedPiece = nullptr;
	DragSourceTile = nullptr;
	bIsDraggingPiece = false;
	CurrentTurn = EJanggiTeam::Red;
	Tiles.Reserve(BoardWidth * BoardHeight);

	for (int32 Y = 0; Y < BoardHeight; ++Y)
	{
		for (int32 X = 0; X < BoardWidth; ++X)
		{
			const FVector TileLocation = BoardToWorld(X, Y);
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;

			AJanggiTile* Tile = GetWorld()->SpawnActor<AJanggiTile>(TileClass, TileLocation, GetActorRotation(), SpawnParameters);
			if (!Tile)
			{
				continue;
			}

			Tile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			Tile->InitTile(X, Y);
			Tiles.Add(Tile);
		}
	}
}

AJanggiTile* AJanggiBoard::GetTileAt(int32 X, int32 Y) const
{
	if (!IsInsideBoard(X, Y))
	{
		return nullptr;
	}

	const int32 TileIndex = Y * BoardWidth + X;
	if (!Tiles.IsValidIndex(TileIndex))
	{
		return nullptr;
	}

	return Tiles[TileIndex].Get();
}

FVector AJanggiBoard::BoardToWorld(int32 X, int32 Y) const
{
	const FVector BoardOffset = bCenterBoardOnActor
		? FVector((BoardWidth - 1) * TileSize * -0.5f, (BoardHeight - 1) * TileSize * -0.5f, 0.0f)
		: FVector::ZeroVector;

	return GetActorLocation() + BoardOffset + FVector(X * TileSize, Y * TileSize, 0.0f);
}

bool AJanggiBoard::IsInsideBoard(int32 X, int32 Y) const
{
	return X >= 0 && X < BoardWidth && Y >= 0 && Y < BoardHeight;
}

void AJanggiBoard::ClearHighlights()
{
	for (AJanggiTile* Tile : Tiles)
	{
		if (!Tile)
		{
			continue;
		}

		Tile->SetHighlight(false);
		Tile->SetSelectedHighlight(false);
	}
}

void AJanggiBoard::SpawnTestPieces()
{
	SpawnInitialPieces();
}

void AJanggiBoard::SpawnInitialPieces()
{
	if (!GetWorld() || !PieceClass)
	{
		return;
	}

	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Chariot, 0, 0);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Chariot, 8, 0);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Guard, 3, 0);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Guard, 5, 0);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::King, 4, 1);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Cannon, 1, 2);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Cannon, 7, 2);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Soldier, 0, 3);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Soldier, 2, 3);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Soldier, 4, 3);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Soldier, 6, 3);
	SpawnPiece(EJanggiTeam::Red, EJanggiPieceType::Soldier, 8, 3);
	SpawnFormationPieces(EJanggiTeam::Red, RedFormation, 0);

	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Chariot, 0, 9);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Chariot, 8, 9);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Guard, 3, 9);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Guard, 5, 9);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::King, 4, 8);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Cannon, 1, 7);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Cannon, 7, 7);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Soldier, 0, 6);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Soldier, 2, 6);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Soldier, 4, 6);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Soldier, 6, 6);
	SpawnPiece(EJanggiTeam::Blue, EJanggiPieceType::Soldier, 8, 6);
	SpawnFormationPieces(EJanggiTeam::Blue, BlueFormation, 9);

	UE_LOG(LogTemp, Log, TEXT("Janggi initial pieces spawned: RedFormation=%d BlueFormation=%d Total=%d"),
		static_cast<int32>(RedFormation),
		static_cast<int32>(BlueFormation),
		Pieces.Num());
}

TArray<FIntPoint> AJanggiBoard::GetLegalMoves(AJanggiPieceBase* Piece) const
{
	TArray<FIntPoint> LegalMoves;

	if (!Piece || !Piece->bIsAlive)
	{
		return LegalMoves;
	}

	switch (Piece->PieceType)
	{
	case EJanggiPieceType::King:
		AddPalaceStepMoves(LegalMoves, Piece);
		break;
	case EJanggiPieceType::Guard:
		AddPalaceStepMoves(LegalMoves, Piece);
		break;
	case EJanggiPieceType::Chariot:
		AddStraightMoves(LegalMoves, Piece);
		break;
	case EJanggiPieceType::Cannon:
		AddCannonMoves(LegalMoves, Piece);
		break;
	case EJanggiPieceType::Horse:
		AddHorseMoves(LegalMoves, Piece);
		break;
	case EJanggiPieceType::Elephant:
		AddElephantMoves(LegalMoves, Piece);
		break;
	case EJanggiPieceType::Soldier:
		AddSoldierMoves(LegalMoves, Piece);
		break;
	default:
		return LegalMoves;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi legal moves: Type=%s Team=%s Piece=%s At=(%d,%d) Count=%d"),
		ToJanggiPieceTypeText(Piece->PieceType),
		Piece->Team == EJanggiTeam::Red ? TEXT("Red") : TEXT("Blue"),
		*GetNameSafe(Piece),
		Piece->BoardX,
		Piece->BoardY,
		LegalMoves.Num());

	if (LegalMoves.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi legal moves empty: Type=%s Team=%s At=(%d,%d). Check blockers, own pieces, palace bounds, or board bounds."),
			ToJanggiPieceTypeText(Piece->PieceType),
			Piece->Team == EJanggiTeam::Red ? TEXT("Red") : TEXT("Blue"),
			Piece->BoardX,
			Piece->BoardY);
	}

	return LegalMoves;
}

TArray<FString> AJanggiBoard::GetLegalMoveDropdownOptions(AJanggiPieceBase* Piece) const
{
	TArray<FString> Options;
	UE_LOG(LogTemp, Log, TEXT("Janggi dropdown options disabled: Reason=DragAndDropOnly Piece=%s."), *GetNameSafe(Piece));
	return Options;
}

void AJanggiBoard::OnTileClicked(AJanggiTile* Tile)
{
	if (!Tile)
	{
		return;
	}

	if (SelectedPiece && SelectedPiece->bIsAlive)
	{
		UE_LOG(LogTemp, Log, TEXT("Janggi tile click ignored for movement: Tile=(%d,%d) SelectedPiece=%s CurrentTurn=%s Reason=DragAndDropOnly."),
			Tile->TileX,
			Tile->TileY,
			*GetNameSafe(SelectedPiece.Get()),
			GetTeamText(CurrentTurn));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi tile selected without piece: (%d, %d) CurrentTurn=%s"), Tile->TileX, Tile->TileY, GetTeamText(CurrentTurn));
}

void AJanggiBoard::OnPieceClicked(AJanggiPieceBase* Piece)
{
	if (!Piece || !Piece->bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi piece click ignored: Reason=InvalidPiece CurrentTurn=%s"), GetTeamText(CurrentTurn));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi piece clicked: Team=%s Type=%s Coord=(%d,%d) CurrentTurn=%s"),
		GetTeamText(Piece->Team),
		GetPieceTypeText(Piece->PieceType),
		Piece->BoardX,
		Piece->BoardY,
		GetTeamText(CurrentTurn));

	SelectPiece(Piece);
}

void AJanggiBoard::SelectPiece(AJanggiPieceBase* Piece)
{
	if (!Piece || !Piece->bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi selection failed: Reason=InvalidPiece CurrentTurn=%s"), GetTeamText(CurrentTurn));
		return;
	}

	if (!CanSelectPiece(Piece))
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi selection failed: Reason=Not current turn piece PieceTeam=%s CurrentTurn=%s Type=%s Coord=(%d,%d)"),
			GetTeamText(Piece->Team),
			GetTeamText(CurrentTurn),
			GetPieceTypeText(Piece->PieceType),
			Piece->BoardX,
			Piece->BoardY);
		return;
	}

	SelectedPiece = Piece;
	ClearHighlights();

	if (AJanggiTile* CurrentTile = GetTileForPiece(Piece))
	{
		CurrentTile->SetSelectedHighlight(true);
	}

	const TArray<FIntPoint> LegalMoves = GetLegalMoves(Piece);
	UE_LOG(LogTemp, Log, TEXT("Janggi selection succeeded: Piece=%s Team=%s Type=%s Coord=(%d,%d) CurrentTurn=%s LegalMoveCount=%d"),
		*GetNameSafe(Piece),
		GetTeamText(Piece->Team),
		ToJanggiPieceTypeText(Piece->PieceType),
		Piece->BoardX,
		Piece->BoardY,
		GetTeamText(CurrentTurn),
		LegalMoves.Num());

	LogLegalMoveOptions(Piece, LegalMoves);

	for (const FIntPoint& Move : LegalMoves)
	{
		if (AJanggiTile* Tile = GetTileAt(Move.X, Move.Y))
		{
			Tile->SetHighlight(true);
		}
	}
}

bool AJanggiBoard::MoveSelectedPieceToTile(AJanggiTile* TargetTile)
{
	if (!SelectedPiece || !SelectedPiece->bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi move failed: Reason=NoSelectedPiece."));
		return false;
	}

	if (!bIsDraggingPiece || !DraggedPiece)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi move failed: Reason=DragAndDropOnly. BeginDragPiece must be active."));
		return false;
	}

	if (!TargetTile)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi move failed: Reason=NoTileFound."));
		return false;
	}

	if (!IsInsideBoard(TargetTile->TileX, TargetTile->TileY))
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi move failed: Reason=OutsideBoard Target=(%d,%d)."), TargetTile->TileX, TargetTile->TileY);
		return false;
	}

	AJanggiPieceBase* MovingPiece = SelectedPiece;
	if (MovingPiece->Team != CurrentTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi move failed: Reason=NotCurrentTurnPiece PieceTeam=%s CurrentTurn=%s Piece=%s."),
			GetTeamText(MovingPiece->Team),
			GetTeamText(CurrentTurn),
			*GetNameSafe(MovingPiece));
		return false;
	}

	if (!IsLegalMoveForPiece(MovingPiece, TargetTile->TileX, TargetTile->TileY))
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi move failed: Reason=NotLegalMove Target=(%d,%d) Piece=%s From=(%d,%d)."),
			TargetTile->TileX,
			TargetTile->TileY,
			*GetNameSafe(MovingPiece),
			MovingPiece->BoardX,
			MovingPiece->BoardY);
		return false;
	}

	AJanggiTile* SourceTile = GetTileForPiece(MovingPiece);
	if (SourceTile == TargetTile)
	{
		ClearHighlights();
		TargetTile->SetSelectedHighlight(true);
		UE_LOG(LogTemp, Log, TEXT("Janggi move failed: Reason=SameTile Piece=%s Target=(%d,%d)."), *GetNameSafe(MovingPiece), TargetTile->TileX, TargetTile->TileY);
		return false;
	}

	AJanggiPieceBase* CapturedPiece = TargetTile->CurrentPiece.Get();
	if (CapturedPiece && CapturedPiece->Team == MovingPiece->Team)
	{
		SelectPiece(CapturedPiece);
		UE_LOG(LogTemp, Log, TEXT("Janggi move failed: Reason=SameTeamPiece Piece=%s TargetPiece=%s Target=(%d,%d). Selection changed."),
			*GetNameSafe(MovingPiece),
			*GetNameSafe(CapturedPiece),
			CapturedPiece->BoardX,
			CapturedPiece->BoardY);
		return false;
	}

	const int32 PreviousX = MovingPiece->BoardX;
	const int32 PreviousY = MovingPiece->BoardY;
	bool bCaptured = false;

	if (CapturedPiece)
	{
		bCaptured = true;
		CapturedPiece->Capture();
		Pieces.Remove(CapturedPiece);
		TargetTile->ClearCurrentPiece();
	}

	if (SourceTile)
	{
		SourceTile->ClearCurrentPiece();
	}

	MovingPiece->SetBoardPosition(TargetTile->TileX, TargetTile->TileY);
	MovingPiece->MoveToWorldLocation(BoardToWorld(TargetTile->TileX, TargetTile->TileY) + FVector(0.0f, 0.0f, PieceWorldZOffset));
	TargetTile->SetCurrentPiece(MovingPiece);

	ClearHighlights();
	TargetTile->SetSelectedHighlight(true);

	UE_LOG(LogTemp, Log, TEXT("Janggi move result: Piece=%s From=(%d,%d) To=(%d,%d) Captured=%s CapturedPiece=%s"),
		*GetNameSafe(MovingPiece),
		PreviousX,
		PreviousY,
		MovingPiece->BoardX,
		MovingPiece->BoardY,
		bCaptured ? TEXT("true") : TEXT("false"),
		*GetNameSafe(CapturedPiece));

	SwitchTurnAfterMove();
	return true;
}

bool AJanggiBoard::MoveSelectedPieceToBoardPosition(int32 TargetX, int32 TargetY)
{
	UE_LOG(LogTemp, Warning, TEXT("Janggi dropdown move disabled: Reason=DragAndDropOnly Target=(%d,%d) SelectedPiece=%s CurrentTurn=%s"),
		TargetX,
		TargetY,
		*GetNameSafe(SelectedPiece.Get()),
		GetTeamText(CurrentTurn));

	return false;
}

bool AJanggiBoard::BeginDragPiece(AJanggiPieceBase* Piece)
{
	if (!Piece || !Piece->bIsAlive)
	{
		return false;
	}

	SelectPiece(Piece);
	if (SelectedPiece != Piece)
	{
		DraggedPiece = nullptr;
		DragSourceTile = nullptr;
		bIsDraggingPiece = false;
		UE_LOG(LogTemp, Warning, TEXT("Janggi drag ignored: selection failed for %s."), *GetNameSafe(Piece));
		return false;
	}

	DraggedPiece = Piece;
	DragSourceTile = GetTileForPiece(Piece);
	bIsDraggingPiece = DragSourceTile != nullptr;

	if (!bIsDraggingPiece)
	{
		DraggedPiece = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Janggi drag ignored: source tile not found for %s."), *GetNameSafe(Piece));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi drag started: %s Team=%s From=(%d,%d) CurrentTurn=%s."),
		*GetNameSafe(Piece),
		GetTeamText(Piece->Team),
		Piece->BoardX,
		Piece->BoardY,
		GetTeamText(CurrentTurn));
	return true;
}

void AJanggiBoard::PreviewDraggedPieceAtWorld(const FVector& WorldLocation)
{
	if (!bIsDraggingPiece || !DraggedPiece)
	{
		return;
	}

	DraggedPiece->MoveToWorldLocation(FVector(WorldLocation.X, WorldLocation.Y, GetBoardCenterWorldLocation().Z + PieceWorldZOffset));
}

bool AJanggiBoard::FinishDragOnTile(AJanggiTile* TargetTile)
{
	if (!bIsDraggingPiece || !DraggedPiece)
	{
		return false;
	}

	if (!TargetTile)
	{
		CancelDrag();
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi drag finish: Piece=%s Target=(%d,%d)."),
		*GetNameSafe(DraggedPiece.Get()),
		TargetTile->TileX,
		TargetTile->TileY);

	const bool bMoved = MoveSelectedPieceToTile(TargetTile);
	if (!bMoved && DragSourceTile && DraggedPiece)
	{
		DraggedPiece->MoveToWorldLocation(BoardToWorld(DraggedPiece->BoardX, DraggedPiece->BoardY) + FVector(0.0f, 0.0f, PieceWorldZOffset));
		bIsDraggingPiece = false;
		DraggedPiece = nullptr;
		DragSourceTile = nullptr;
	}

	return bMoved;
}

void AJanggiBoard::CancelDrag()
{
	if (!bIsDraggingPiece || !DraggedPiece)
	{
		return;
	}

	DraggedPiece->MoveToWorldLocation(BoardToWorld(DraggedPiece->BoardX, DraggedPiece->BoardY) + FVector(0.0f, 0.0f, PieceWorldZOffset));
	UE_LOG(LogTemp, Log, TEXT("Janggi drag canceled: %s returned to (%d, %d)."), *GetNameSafe(DraggedPiece), DraggedPiece->BoardX, DraggedPiece->BoardY);

	bIsDraggingPiece = false;
	DraggedPiece = nullptr;
	DragSourceTile = nullptr;
}

bool AJanggiBoard::IsDraggingPiece() const
{
	return bIsDraggingPiece && IsValid(DraggedPiece);
}

AJanggiPieceBase* AJanggiBoard::GetDraggedPiece() const
{
	return IsDraggingPiece() ? DraggedPiece.Get() : nullptr;
}

FVector AJanggiBoard::GetBoardCenterWorldLocation() const
{
	const float CenterX = (BoardWidth - 1) * 0.5f;
	const float CenterY = (BoardHeight - 1) * 0.5f;

	const FVector BoardOffset = bCenterBoardOnActor
		? FVector((BoardWidth - 1) * TileSize * -0.5f, (BoardHeight - 1) * TileSize * -0.5f, 0.0f)
		: FVector::ZeroVector;

	return GetActorLocation() + BoardOffset + FVector(CenterX * TileSize, CenterY * TileSize, 0.0f);
}

void AJanggiBoard::ClearGeneratedActors()
{
	for (AJanggiPieceBase* Piece : Pieces)
	{
		if (Piece)
		{
			Piece->Destroy();
		}
	}
	Pieces.Reset();
	SelectedPiece = nullptr;
	DraggedPiece = nullptr;
	DragSourceTile = nullptr;
	bIsDraggingPiece = false;
	CurrentTurn = EJanggiTeam::Red;

	for (AJanggiTile* Tile : Tiles)
	{
		if (Tile)
		{
			Tile->Destroy();
		}
	}
	Tiles.Reset();
}

void AJanggiBoard::SpawnFormationPieces(EJanggiTeam Team, EJanggiOpeningFormation Formation, int32 BackRankY)
{
	constexpr int32 FormationSlotCount = 4;
	const int32 SlotX[FormationSlotCount] = { 1, 2, 6, 7 };

	for (int32 SlotIndex = 0; SlotIndex < FormationSlotCount; ++SlotIndex)
	{
		SpawnPiece(Team, GetFormationPieceType(Formation, SlotIndex), SlotX[SlotIndex], BackRankY);
	}
}

EJanggiPieceType AJanggiBoard::GetFormationPieceType(EJanggiOpeningFormation Formation, int32 SlotIndex) const
{
	switch (Formation)
	{
	case EJanggiOpeningFormation::MaSangSangMa:
		return (SlotIndex == 0 || SlotIndex == 3) ? EJanggiPieceType::Horse : EJanggiPieceType::Elephant;
	case EJanggiOpeningFormation::SangMaMaSang:
		return (SlotIndex == 1 || SlotIndex == 2) ? EJanggiPieceType::Horse : EJanggiPieceType::Elephant;
	case EJanggiOpeningFormation::SangMaSangMa:
		return (SlotIndex == 1 || SlotIndex == 3) ? EJanggiPieceType::Horse : EJanggiPieceType::Elephant;
	case EJanggiOpeningFormation::MaSangMaSang:
	default:
		return (SlotIndex == 0 || SlotIndex == 2) ? EJanggiPieceType::Horse : EJanggiPieceType::Elephant;
	}
}

void AJanggiBoard::AddMoveIfValid(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece, int32 TargetX, int32 TargetY) const
{
	if (!CanPieceMoveToTile(Piece, TargetX, TargetY))
	{
		return;
	}

	LegalMoves.AddUnique(FIntPoint(TargetX, TargetY));
}

void AJanggiBoard::AddPalaceStepMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return;
	}

	if (!IsInsideTeamPalace(Piece->Team, Piece->BoardX, Piece->BoardY))
	{
		return;
	}

	constexpr int32 DirectionCount = 8;
	const FIntPoint Directions[DirectionCount] =
	{
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1),
		FIntPoint(1, 1),
		FIntPoint(1, -1),
		FIntPoint(-1, 1),
		FIntPoint(-1, -1)
	};

	for (const FIntPoint& Direction : Directions)
	{
		const int32 TargetX = Piece->BoardX + Direction.X;
		const int32 TargetY = Piece->BoardY + Direction.Y;
		if (!IsInsideTeamPalace(Piece->Team, TargetX, TargetY))
		{
			continue;
		}

		const bool bIsDiagonal = FMath::Abs(Direction.X) == 1 && FMath::Abs(Direction.Y) == 1;
		if (bIsDiagonal && !IsPalaceDiagonalStep(Piece->BoardX, Piece->BoardY, TargetX, TargetY))
		{
			continue;
		}

		AddMoveIfValid(LegalMoves, Piece, TargetX, TargetY);
	}
}

void AJanggiBoard::AddStraightMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return;
	}

	constexpr int32 DirectionCount = 4;
	const FIntPoint Directions[DirectionCount] =
	{
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1)
	};

	for (const FIntPoint& Direction : Directions)
	{
		int32 TargetX = Piece->BoardX + Direction.X;
		int32 TargetY = Piece->BoardY + Direction.Y;

		while (IsInsideBoard(TargetX, TargetY))
		{
			AJanggiTile* TargetTile = GetTileAt(TargetX, TargetY);
			if (!TargetTile)
			{
				break;
			}

			AJanggiPieceBase* BlockingPiece = TargetTile->CurrentPiece.Get();
			if (!BlockingPiece)
			{
				LegalMoves.Add(FIntPoint(TargetX, TargetY));
			}
			else
			{
				if (BlockingPiece->Team != Piece->Team)
				{
					LegalMoves.Add(FIntPoint(TargetX, TargetY));
				}
				break;
			}

			TargetX += Direction.X;
			TargetY += Direction.Y;
		}
	}
}

void AJanggiBoard::AddCannonMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return;
	}

	constexpr int32 DirectionCount = 4;
	const FIntPoint Directions[DirectionCount] =
	{
		FIntPoint(1, 0),
		FIntPoint(-1, 0),
		FIntPoint(0, 1),
		FIntPoint(0, -1)
	};

	for (const FIntPoint& Direction : Directions)
	{
		bool bJumpedScreen = false;
		int32 TargetX = Piece->BoardX + Direction.X;
		int32 TargetY = Piece->BoardY + Direction.Y;

		while (IsInsideBoard(TargetX, TargetY))
		{
			AJanggiTile* TargetTile = GetTileAt(TargetX, TargetY);
			if (!TargetTile)
			{
				break;
			}

			AJanggiPieceBase* TilePiece = TargetTile->CurrentPiece.Get();
			if (!bJumpedScreen)
			{
				if (TilePiece)
				{
					if (TilePiece->PieceType == EJanggiPieceType::Cannon)
					{
						break;
					}

					bJumpedScreen = true;
				}

				TargetX += Direction.X;
				TargetY += Direction.Y;
				continue;
			}

			if (!TilePiece)
			{
				LegalMoves.Add(FIntPoint(TargetX, TargetY));
				TargetX += Direction.X;
				TargetY += Direction.Y;
				continue;
			}

			if (TilePiece->Team != Piece->Team && TilePiece->PieceType != EJanggiPieceType::Cannon)
			{
				LegalMoves.Add(FIntPoint(TargetX, TargetY));
			}
			break;
		}
	}
}

void AJanggiBoard::AddHorseMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return;
	}

	struct FHorseMoveRule
	{
		FIntPoint Leg;
		FIntPoint Targets[2];
	};

	const FHorseMoveRule Rules[4] =
	{
		{ FIntPoint(0, 1), { FIntPoint(-1, 2), FIntPoint(1, 2) } },
		{ FIntPoint(0, -1), { FIntPoint(-1, -2), FIntPoint(1, -2) } },
		{ FIntPoint(1, 0), { FIntPoint(2, 1), FIntPoint(2, -1) } },
		{ FIntPoint(-1, 0), { FIntPoint(-2, 1), FIntPoint(-2, -1) } }
	};

	for (const FHorseMoveRule& Rule : Rules)
	{
		AJanggiTile* LegTile = GetTileAt(Piece->BoardX + Rule.Leg.X, Piece->BoardY + Rule.Leg.Y);
		if (!LegTile || LegTile->IsOccupied())
		{
			continue;
		}

		for (const FIntPoint& TargetOffset : Rule.Targets)
		{
			AddMoveIfValid(LegalMoves, Piece, Piece->BoardX + TargetOffset.X, Piece->BoardY + TargetOffset.Y);
		}
	}
}

void AJanggiBoard::AddElephantMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return;
	}

	struct FElephantMoveRule
	{
		FIntPoint FirstBlocker;
		FIntPoint SecondBlocker;
		FIntPoint Target;
	};

	const FElephantMoveRule Rules[8] =
	{
		{ FIntPoint(0, 1), FIntPoint(-1, 2), FIntPoint(-2, 3) },
		{ FIntPoint(0, 1), FIntPoint(1, 2), FIntPoint(2, 3) },
		{ FIntPoint(0, -1), FIntPoint(-1, -2), FIntPoint(-2, -3) },
		{ FIntPoint(0, -1), FIntPoint(1, -2), FIntPoint(2, -3) },
		{ FIntPoint(1, 0), FIntPoint(2, 1), FIntPoint(3, 2) },
		{ FIntPoint(1, 0), FIntPoint(2, -1), FIntPoint(3, -2) },
		{ FIntPoint(-1, 0), FIntPoint(-2, 1), FIntPoint(-3, 2) },
		{ FIntPoint(-1, 0), FIntPoint(-2, -1), FIntPoint(-3, -2) }
	};

	for (const FElephantMoveRule& Rule : Rules)
	{
		AJanggiTile* FirstBlockerTile = GetTileAt(Piece->BoardX + Rule.FirstBlocker.X, Piece->BoardY + Rule.FirstBlocker.Y);
		if (!FirstBlockerTile || FirstBlockerTile->IsOccupied())
		{
			continue;
		}

		AJanggiTile* SecondBlockerTile = GetTileAt(Piece->BoardX + Rule.SecondBlocker.X, Piece->BoardY + Rule.SecondBlocker.Y);
		if (!SecondBlockerTile || SecondBlockerTile->IsOccupied())
		{
			continue;
		}

		AddMoveIfValid(LegalMoves, Piece, Piece->BoardX + Rule.Target.X, Piece->BoardY + Rule.Target.Y);
	}
}

void AJanggiBoard::AddSoldierMoves(TArray<FIntPoint>& LegalMoves, const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return;
	}

	const int32 ForwardY = Piece->Team == EJanggiTeam::Red ? 1 : -1;
	AddMoveIfValid(LegalMoves, Piece, Piece->BoardX, Piece->BoardY + ForwardY);
	AddMoveIfValid(LegalMoves, Piece, Piece->BoardX - 1, Piece->BoardY);
	AddMoveIfValid(LegalMoves, Piece, Piece->BoardX + 1, Piece->BoardY);

	const int32 DiagonalY = Piece->BoardY + ForwardY;
	const int32 DiagonalLeftX = Piece->BoardX - 1;
	const int32 DiagonalRightX = Piece->BoardX + 1;

	if (IsInsideAnyPalace(Piece->BoardX, Piece->BoardY))
	{
		if (IsPalaceDiagonalStep(Piece->BoardX, Piece->BoardY, DiagonalLeftX, DiagonalY))
		{
			AddMoveIfValid(LegalMoves, Piece, DiagonalLeftX, DiagonalY);
		}

		if (IsPalaceDiagonalStep(Piece->BoardX, Piece->BoardY, DiagonalRightX, DiagonalY))
		{
			AddMoveIfValid(LegalMoves, Piece, DiagonalRightX, DiagonalY);
		}
	}
}

bool AJanggiBoard::CanPieceMoveToTile(const AJanggiPieceBase* Piece, int32 TargetX, int32 TargetY) const
{
	if (!Piece || !Piece->bIsAlive)
	{
		return false;
	}

	AJanggiTile* TargetTile = GetTileAt(TargetX, TargetY);
	if (!TargetTile)
	{
		return false;
	}

	AJanggiPieceBase* TargetPiece = TargetTile->CurrentPiece.Get();
	return !TargetPiece || TargetPiece->Team != Piece->Team;
}

bool AJanggiBoard::IsInsideTeamPalace(EJanggiTeam Team, int32 X, int32 Y) const
{
	const bool bInsidePalaceFile = X >= 3 && X <= 5;
	if (!bInsidePalaceFile)
	{
		return false;
	}

	if (Team == EJanggiTeam::Red)
	{
		return Y >= 0 && Y <= 2;
	}

	return Y >= 7 && Y <= 9;
}

bool AJanggiBoard::IsInsideAnyPalace(int32 X, int32 Y) const
{
	const bool bInsidePalaceFile = X >= 3 && X <= 5;
	if (!bInsidePalaceFile)
	{
		return false;
	}

	return (Y >= 0 && Y <= 2) || (Y >= 7 && Y <= 9);
}

bool AJanggiBoard::IsPalaceDiagonalStep(int32 FromX, int32 FromY, int32 ToX, int32 ToY) const
{
	if (!IsInsideAnyPalace(FromX, FromY) || !IsInsideAnyPalace(ToX, ToY))
	{
		return false;
	}

	const bool bBothInRedPalace = FromY >= 0 && FromY <= 2 && ToY >= 0 && ToY <= 2;
	const bool bBothInBluePalace = FromY >= 7 && FromY <= 9 && ToY >= 7 && ToY <= 9;
	if (!bBothInRedPalace && !bBothInBluePalace)
	{
		return false;
	}

	if (FMath::Abs(ToX - FromX) != 1 || FMath::Abs(ToY - FromY) != 1)
	{
		return false;
	}

	const int32 CenterY = bBothInRedPalace ? 1 : 8;
	const bool bFromCenter = FromX == 4 && FromY == CenterY;
	const bool bToCenter = ToX == 4 && ToY == CenterY;
	return bFromCenter || bToCenter;
}

AJanggiTile* AJanggiBoard::GetTileForPiece(const AJanggiPieceBase* Piece) const
{
	if (!Piece)
	{
		return nullptr;
	}

	return GetTileAt(Piece->BoardX, Piece->BoardY);
}

bool AJanggiBoard::IsLegalMoveForPiece(AJanggiPieceBase* Piece, int32 TargetX, int32 TargetY) const
{
	if (!Piece || !Piece->bIsAlive)
	{
		return false;
	}

	const TArray<FIntPoint> LegalMoves = GetLegalMoves(Piece);
	const FIntPoint TargetPoint(TargetX, TargetY);
	return LegalMoves.Contains(TargetPoint);
}

AJanggiPieceBase* AJanggiBoard::SpawnPiece(EJanggiTeam Team, EJanggiPieceType PieceType, int32 X, int32 Y)
{
	if (!IsInsideBoard(X, Y))
	{
		return nullptr;
	}

	AJanggiTile* TargetTile = GetTileAt(X, Y);
	if (!TargetTile || TargetTile->IsOccupied())
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	const FVector PieceLocation = BoardToWorld(X, Y) + FVector(0.0f, 0.0f, PieceWorldZOffset);
	AJanggiPieceBase* Piece = GetWorld()->SpawnActor<AJanggiPieceBase>(PieceClass, PieceLocation, GetActorRotation(), SpawnParameters);
	if (!Piece)
	{
		return nullptr;
	}

	Piece->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	Piece->InitPiece(Team, PieceType, X, Y);
	TargetTile->SetCurrentPiece(Piece);
	Pieces.Add(Piece);

	UE_LOG(LogTemp, Log, TEXT("Janggi piece spawned: Team=%s Type=%s Coord=(%d,%d) Actor=%s"),
		Team == EJanggiTeam::Red ? TEXT("Red") : TEXT("Blue"),
		ToJanggiPieceTypeText(PieceType),
		X,
		Y,
		*GetNameSafe(Piece));

	return Piece;
}

const TCHAR* AJanggiBoard::GetTeamText(EJanggiTeam Team) const
{
	return Team == EJanggiTeam::Red ? TEXT("Red") : TEXT("Blue");
}

const TCHAR* AJanggiBoard::GetPieceTypeText(EJanggiPieceType PieceType) const
{
	return ToJanggiPieceTypeText(PieceType);
}

void AJanggiBoard::LogLegalMoveOptions(AJanggiPieceBase* Piece, const TArray<FIntPoint>& LegalMoves) const
{
	if (!Piece)
	{
		return;
	}

	FString OptionText;
	for (const FIntPoint& Move : LegalMoves)
	{
		if (!OptionText.IsEmpty())
		{
			OptionText += TEXT(", ");
		}

		OptionText += FString::Printf(TEXT("(%d,%d)"), Move.X, Move.Y);
	}

	if (OptionText.IsEmpty())
	{
		OptionText = TEXT("No legal move positions");
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi legal move options: Piece=%s Team=%s Type=%s Coord=(%d,%d) Count=%d Options=%s"),
		*GetNameSafe(Piece),
		GetTeamText(Piece->Team),
		GetPieceTypeText(Piece->PieceType),
		Piece->BoardX,
		Piece->BoardY,
		LegalMoves.Num(),
		*OptionText);
}

bool AJanggiBoard::CanSelectPiece(const AJanggiPieceBase* Piece) const
{
	if (!Piece || !Piece->bIsAlive)
	{
		return false;
	}

	return Piece->Team == CurrentTurn;
}

void AJanggiBoard::SwitchTurnAfterMove()
{
	const EJanggiTeam PreviousTurn = CurrentTurn;
	CurrentTurn = CurrentTurn == EJanggiTeam::Red ? EJanggiTeam::Blue : EJanggiTeam::Red;
	SelectedPiece = nullptr;
	DraggedPiece = nullptr;
	DragSourceTile = nullptr;
	bIsDraggingPiece = false;
	ClearHighlights();

	UE_LOG(LogTemp, Log, TEXT("Janggi turn changed: %s -> %s bVsAI=%s"),
		GetTeamText(PreviousTurn),
		GetTeamText(CurrentTurn),
		bVsAI ? TEXT("true") : TEXT("false"));

	if (bVsAI)
	{
		UE_LOG(LogTemp, Log, TEXT("Janggi AI turn pending: CurrentTurn=%s. AI execution is not implemented in this step."), GetTeamText(CurrentTurn));
	}
}
