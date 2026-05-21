#include "JanggiPlayerController.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/LightComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "JanggiBoard.h"
#include "JanggiPieceBase.h"
#include "JanggiTile.h"

namespace
{
	constexpr float PreviousCameraDistanceMultiplier = 1.265f;
}

AJanggiPlayerController::AJanggiPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = false;
	bEnableTouchOverEvents = false;
	bAutoManageActiveCameraTarget = false;
	PrimaryActorTick.bCanEverTick = true;

	DefaultMouseCursor = EMouseCursor::Default;

	CameraHeight = 1200.0f;
	CameraDistance = 750.0f;
	CameraBlendTime = 0.0f;
	CameraYawOffsetDegrees = 0.0f;
	CameraFieldOfView = 50.0f;
	CameraPlanarDistanceScale = 0.792f;
	CameraHeightScale = 0.95f;
	CameraDistanceMultiplier = 1.45475f;
	DirectionalLightOffsetTilesX = -2.0f;
	CachedBoard = nullptr;
	SpawnedCamera = nullptr;
	DraggedPiece = nullptr;
	OriginalTile = nullptr;
	OriginalWorldLocation = FVector::ZeroVector;
	bIsDraggingPiece = false;
	bIsTouchDragging = false;
	ActiveTouchFinger = ETouchIndex::Touch1;
	LastTouchScreenPosition = FVector::ZeroVector;
}

void AJanggiPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	ReleaseExistingPawn();
	ApplyBoardOnlyInputMode();
	SetupFixedCamera();
	ApplyDirectionalLightOffset();
}

void AJanggiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AJanggiPlayerController::HandlePrimaryPressed);
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AJanggiPlayerController::HandlePrimaryReleased);
	InputComponent->BindTouch(IE_Pressed, this, &AJanggiPlayerController::HandleTouchPressed);
	InputComponent->BindTouch(IE_Repeat, this, &AJanggiPlayerController::HandleTouchMoved);
	InputComponent->BindTouch(IE_Released, this, &AJanggiPlayerController::HandleTouchReleased);
}

void AJanggiPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bIsDraggingPiece && !bIsTouchDragging)
	{
		UpdateMouseDragPreview();
	}
}

void AJanggiPlayerController::AddYawInput(float Val)
{
}

void AJanggiPlayerController::AddPitchInput(float Val)
{
}

void AJanggiPlayerController::AddRollInput(float Val)
{
}

void AJanggiPlayerController::HandlePrimaryPressed()
{
	AActor* HitActor = nullptr;
	if (!TraceActorUnderCursor(HitActor))
	{
		return;
	}

	BeginDragFromActor(HitActor, TEXT("Mouse"));
}

void AJanggiPlayerController::HandlePrimaryReleased()
{
	if (!bIsDraggingPiece)
	{
		return;
	}

	AActor* HitActor = nullptr;
	if (!TraceActorUnderCursor(HitActor))
	{
		CancelActiveDrag(TEXT("Mouse"), TEXT("NoHitOnRelease"));
		return;
	}

	FinishDragAtActor(HitActor, TEXT("Mouse"));
}

void AJanggiPlayerController::HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	AActor* HitActor = nullptr;
	if (!TraceActorAtScreenPosition(Location.X, Location.Y, HitActor))
	{
		return;
	}

	if (BeginDragFromActor(HitActor, TEXT("Touch")))
	{
		ActiveTouchFinger = FingerIndex;
		LastTouchScreenPosition = Location;
		bIsTouchDragging = true;
	}
}

void AJanggiPlayerController::HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!bIsTouchDragging || FingerIndex != ActiveTouchFinger)
	{
		return;
	}

	LastTouchScreenPosition = Location;
	UpdateDragPreviewAtScreenPosition(Location.X, Location.Y);
}

void AJanggiPlayerController::HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!bIsTouchDragging || FingerIndex != ActiveTouchFinger)
	{
		return;
	}

	bIsTouchDragging = false;
	LastTouchScreenPosition = Location;

	if (!bIsDraggingPiece)
	{
		return;
	}

	AActor* HitActor = nullptr;
	if (!TraceActorAtScreenPosition(Location.X, Location.Y, HitActor))
	{
		CancelActiveDrag(TEXT("Touch"), TEXT("NoHitOnRelease"));
		return;
	}

	FinishDragAtActor(HitActor, TEXT("Touch"));
}

bool AJanggiPlayerController::BeginDragFromActor(AActor* HitActor, const TCHAR* InputType)
{
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi drag start ignored: Input=%s Reason=NoHitActor."), InputType);
		return false;
	}

	AJanggiBoard* Board = FindJanggiBoard();
	if (!Board)
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi drag start ignored: Input=%s Reason=NoBoard."), InputType);
		return false;
	}

	if (bIsDraggingPiece)
	{
		CancelActiveDrag(InputType, TEXT("RestartDrag"));
	}

	if (AJanggiPieceBase* Piece = Cast<AJanggiPieceBase>(HitActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Janggi drag start target: Input=%s Actor=%s Type=Piece Team=%s Coord=(%d,%d)."),
			InputType,
			*GetNameSafe(Piece),
			Piece->Team == EJanggiTeam::Red ? TEXT("Red") : TEXT("Blue"),
			Piece->BoardX,
			Piece->BoardY);

		if (!Board->BeginDragPiece(Piece))
		{
			ClearLocalDragState();
			UE_LOG(LogTemp, Warning, TEXT("Janggi drag start failed: Input=%s Piece=%s Reason=BoardRejected."),
				InputType,
				*GetNameSafe(Piece));
			return false;
		}

		bIsDraggingPiece = true;
		DraggedPiece = Piece;
		OriginalTile = Board->GetTileAt(Piece->BoardX, Piece->BoardY);
		OriginalWorldLocation = Piece->GetActorLocation();

		UE_LOG(LogTemp, Log, TEXT("Janggi drag started in controller: Input=%s Piece=%s OriginalTile=(%d,%d) OriginalWorld=%s."),
			InputType,
			*GetNameSafe(Piece),
			OriginalTile ? OriginalTile->TileX : -1,
			OriginalTile ? OriginalTile->TileY : -1,
			*OriginalWorldLocation.ToCompactString());
		return true;
	}

	if (AJanggiTile* Tile = Cast<AJanggiTile>(HitActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Janggi drag start ignored: Input=%s Actor=%s Type=Tile Tile=(%d,%d) Reason=DragRequiresPiecePress."),
			InputType,
			*GetNameSafe(Tile),
			Tile->TileX,
			Tile->TileY);
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Janggi drag start ignored: Input=%s Actor=%s Type=Other Reason=UnsupportedActor."),
		InputType,
		*GetNameSafe(HitActor));
	return false;
}

void AJanggiPlayerController::FinishDragAtActor(AActor* HitActor, const TCHAR* InputType)
{
	AJanggiBoard* Board = FindJanggiBoard();
	if (!Board || !bIsDraggingPiece || !Board->IsDraggingPiece())
	{
		CancelActiveDrag(InputType, TEXT("NoActiveDrag"));
		return;
	}

	AJanggiTile* TargetTile = nullptr;
	const TCHAR* TargetType = TEXT("Other");
	if (AJanggiPieceBase* Piece = Cast<AJanggiPieceBase>(HitActor))
	{
		TargetTile = Board->GetTileAt(Piece->BoardX, Piece->BoardY);
		TargetType = TEXT("Piece");
	}
	else if (AJanggiTile* Tile = Cast<AJanggiTile>(HitActor))
	{
		TargetTile = Tile;
		TargetType = TEXT("Tile");
	}

	if (!TargetTile)
	{
		CancelActiveDrag(InputType, TEXT("NoTileFound"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi drag release: Input=%s HitActor=%s TargetType=%s TargetTile=(%d,%d) DraggedPiece=%s OriginalTile=(%d,%d)."),
		InputType,
		*GetNameSafe(HitActor),
		TargetType,
		TargetTile->TileX,
		TargetTile->TileY,
		*GetNameSafe(DraggedPiece.Get()),
		OriginalTile ? OriginalTile->TileX : -1,
		OriginalTile ? OriginalTile->TileY : -1);

	const bool bMoved = Board->FinishDragOnTile(TargetTile);
	if (!bMoved && DraggedPiece)
	{
		DraggedPiece->MoveToWorldLocation(OriginalWorldLocation);
		UE_LOG(LogTemp, Log, TEXT("Janggi drag restored original location: Input=%s Piece=%s World=%s."),
			InputType,
			*GetNameSafe(DraggedPiece.Get()),
			*OriginalWorldLocation.ToCompactString());
	}

	ClearLocalDragState();
}

void AJanggiPlayerController::CancelActiveDrag(const TCHAR* InputType, const TCHAR* Reason)
{
	AJanggiBoard* Board = FindJanggiBoard();
	if (Board)
	{
		Board->CancelDrag();
	}

	if (DraggedPiece)
	{
		DraggedPiece->MoveToWorldLocation(OriginalWorldLocation);
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi drag canceled in controller: Input=%s Reason=%s Piece=%s OriginalTile=(%d,%d) OriginalWorld=%s."),
		InputType,
		Reason,
		*GetNameSafe(DraggedPiece.Get()),
		OriginalTile ? OriginalTile->TileX : -1,
		OriginalTile ? OriginalTile->TileY : -1,
		*OriginalWorldLocation.ToCompactString());

	ClearLocalDragState();
}

void AJanggiPlayerController::ClearLocalDragState()
{
	bIsDraggingPiece = false;
	bIsTouchDragging = false;
	DraggedPiece = nullptr;
	OriginalTile = nullptr;
	OriginalWorldLocation = FVector::ZeroVector;
}

void AJanggiPlayerController::ApplyBoardOnlyInputMode()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = false;
	bEnableTouchOverEvents = false;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AJanggiPlayerController::ReleaseExistingPawn()
{
	APawn* ExistingPawn = GetPawn();
	if (!ExistingPawn)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Janggi controller released existing pawn: %s."), *GetNameSafe(ExistingPawn));
	UnPossess();
	ExistingPawn->Destroy();
}

void AJanggiPlayerController::SetupFixedCamera()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AJanggiBoard* Board = FindJanggiBoard();
	const FVector BoardCenter = Board ? Board->GetBoardCenterWorldLocation() : FVector::ZeroVector;

	float PlanarDistance = CameraDistance;
	float Height = CameraHeight;
	if (Board)
	{
		const float BoardWorldWidth = Board->BoardWidth * Board->TileSize;
		const float BoardWorldHeight = Board->BoardHeight * Board->TileSize;
		const float BoardLongSide = FMath::Max(BoardWorldWidth, BoardWorldHeight);
		PlanarDistance = BoardLongSide * CameraPlanarDistanceScale;
		Height = BoardLongSide * CameraHeightScale;
	}

	const FVector OriginalCameraOffset(0.0f, -PlanarDistance, Height);
	const float PreviousCameraDistance = (OriginalCameraOffset * PreviousCameraDistanceMultiplier).Size();
	const FVector AdjustedCameraOffset = OriginalCameraOffset * CameraDistanceMultiplier;
	const float AdjustedCameraDistance = AdjustedCameraOffset.Size();
	const FVector CameraLocation = BoardCenter + AdjustedCameraOffset;
	FRotator CameraRotation = (BoardCenter - CameraLocation).Rotation();
	CameraRotation.Yaw += CameraYawOffsetDegrees;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnedCamera = World->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), CameraLocation, CameraRotation, SpawnParameters);
	if (!SpawnedCamera)
	{
		return;
	}

	if (UCameraComponent* CameraComponent = SpawnedCamera->GetCameraComponent())
	{
		CameraComponent->SetFieldOfView(CameraFieldOfView);
	}

	bAutoManageActiveCameraTarget = false;
	SetViewTargetWithBlend(SpawnedCamera, CameraBlendTime);
	UE_LOG(LogTemp, Log, TEXT("Janggi fixed camera set: Location=%s Rotation=%s FOV=%.1f PreviousDistance=%.1f AdjustedDistance=%.1f PreviousMultiplier=%.2f Multiplier=%.3f PlanarDistance=%.1f Height=%.1f."),
		*CameraLocation.ToCompactString(),
		*CameraRotation.ToCompactString(),
		CameraFieldOfView,
		PreviousCameraDistance,
		AdjustedCameraDistance,
		PreviousCameraDistanceMultiplier,
		CameraDistanceMultiplier,
		PlanarDistance,
		Height);
}

void AJanggiPlayerController::ApplyDirectionalLightOffset()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AJanggiBoard* Board = FindJanggiBoard();
	if (!Board)
	{
		return;
	}

	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		ADirectionalLight* DirectionalLight = *It;
		if (!DirectionalLight)
		{
			continue;
		}

		if (USceneComponent* RootSceneComponent = DirectionalLight->GetRootComponent())
		{
			RootSceneComponent->SetMobility(EComponentMobility::Movable);
		}

		if (ULightComponent* LightComponent = DirectionalLight->GetLightComponent())
		{
			LightComponent->SetMobility(EComponentMobility::Movable);
		}

		const FVector PreviousLocation = DirectionalLight->GetActorLocation();
		const FVector Offset(Board->TileSize * DirectionalLightOffsetTilesX, 0.0f, 0.0f);
		DirectionalLight->SetActorLocation(PreviousLocation + Offset);
		UE_LOG(LogTemp, Log, TEXT("Janggi directional light moved from %s to %s."),
			*PreviousLocation.ToCompactString(),
			*DirectionalLight->GetActorLocation().ToCompactString());
		return;
	}
}

void AJanggiPlayerController::UpdateMouseDragPreview()
{
	AJanggiBoard* Board = FindJanggiBoard();
	if (!Board || !Board->IsDraggingPiece())
	{
		return;
	}

	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!GetMousePosition(MouseX, MouseY))
	{
		return;
	}

	UpdateDragPreviewAtScreenPosition(MouseX, MouseY);
}

void AJanggiPlayerController::UpdateDragPreviewAtScreenPosition(float ScreenX, float ScreenY)
{
	AJanggiBoard* Board = FindJanggiBoard();
	if (!Board || !Board->IsDraggingPiece())
	{
		return;
	}

	FVector RayOrigin;
	FVector RayDirection;
	if (!DeprojectScreenPositionToWorld(ScreenX, ScreenY, RayOrigin, RayDirection))
	{
		return;
	}

	if (FMath::IsNearlyZero(RayDirection.Z))
	{
		return;
	}

	const float TargetZ = Board->GetBoardCenterWorldLocation().Z;
	const float Distance = (TargetZ - RayOrigin.Z) / RayDirection.Z;
	if (Distance < 0.0f)
	{
		return;
	}

	Board->PreviewDraggedPieceAtWorld(RayOrigin + RayDirection * Distance);
}

bool AJanggiPlayerController::TraceActorUnderCursor(AActor*& HitActor)
{
	float MouseX = 0.0f;
	float MouseY = 0.0f;
	if (!GetMousePosition(MouseX, MouseY))
	{
		HitActor = nullptr;
		return false;
	}

	return TraceActorAtScreenPosition(MouseX, MouseY, HitActor);
}

bool AJanggiPlayerController::TraceActorAtScreenPosition(float ScreenX, float ScreenY, AActor*& HitActor)
{
	HitActor = nullptr;

	FVector RayOrigin;
	FVector RayDirection;
	if (!DeprojectScreenPositionToWorld(ScreenX, ScreenY, RayOrigin, RayDirection))
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	AJanggiBoard* Board = FindJanggiBoard();

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(JanggiPointerTrace), true);
	if (CachedBoard && CachedBoard->GetDraggedPiece())
	{
		QueryParams.AddIgnoredActor(CachedBoard->GetDraggedPiece());
	}

	TArray<FHitResult> HitResults;
	const FVector TraceEnd = RayOrigin + RayDirection * 100000.0f;
	if (!World->LineTraceMultiByChannel(HitResults, RayOrigin, TraceEnd, ECC_Visibility, QueryParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("Janggi trace missed: Screen=(%.1f,%.1f) Selected=%s Reason=NoHit."),
			ScreenX,
			ScreenY,
			Board && Board->SelectedPiece ? TEXT("true") : TEXT("false"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Janggi trace multi: Screen=(%.1f,%.1f) HitCount=%d Selected=%s SelectedPiece=%s."),
		ScreenX,
		ScreenY,
		HitResults.Num(),
		Board && Board->SelectedPiece ? TEXT("true") : TEXT("false"),
		Board && Board->SelectedPiece ? *GetNameSafe(Board->SelectedPiece.Get()) : TEXT("None"));

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* Actor = HitResult.GetActor();
		UE_LOG(LogTemp, Log, TEXT("Janggi trace hit: Actor=%s Type=%s Distance=%.1f Blocking=%s."),
			*GetNameSafe(Actor),
			GetTraceActorType(Actor),
			HitResult.Distance,
			HitResult.bBlockingHit ? TEXT("true") : TEXT("false"));
	}

	HitActor = SelectTraceTarget(HitResults, Board);
	UE_LOG(LogTemp, Log, TEXT("Janggi trace selected target: Actor=%s Type=%s."),
		*GetNameSafe(HitActor),
		GetTraceActorType(HitActor));

	return HitActor != nullptr;
}

AActor* AJanggiPlayerController::SelectTraceTarget(const TArray<FHitResult>& HitResults, AJanggiBoard* Board) const
{
	AJanggiPieceBase* FirstPiece = nullptr;
	AJanggiPieceBase* FirstOwnPiece = nullptr;
	AJanggiPieceBase* FirstEnemyPiece = nullptr;
	AJanggiTile* FirstTile = nullptr;
	AJanggiTile* FirstLegalTile = nullptr;

	const bool bHasSelectedPiece = Board && Board->SelectedPiece && Board->SelectedPiece->bIsAlive;
	TArray<FIntPoint> LegalMoves;
	if (bHasSelectedPiece)
	{
		LegalMoves = Board->GetLegalMoves(Board->SelectedPiece.Get());
	}

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* Actor = HitResult.GetActor();
		if (!Actor)
		{
			continue;
		}

		if (AJanggiPieceBase* Piece = Cast<AJanggiPieceBase>(Actor))
		{
			if (!FirstPiece)
			{
				FirstPiece = Piece;
			}

			if (bHasSelectedPiece)
			{
				if (Piece->Team == Board->SelectedPiece->Team)
				{
					if (!FirstOwnPiece)
					{
						FirstOwnPiece = Piece;
					}
				}
				else if (!FirstEnemyPiece)
				{
					FirstEnemyPiece = Piece;
				}

				if (AJanggiTile* PieceTile = Board->GetTileAt(Piece->BoardX, Piece->BoardY))
				{
					const FIntPoint PieceTilePoint(PieceTile->TileX, PieceTile->TileY);
					if (!FirstLegalTile && LegalMoves.Contains(PieceTilePoint))
					{
						FirstLegalTile = PieceTile;
					}
				}
			}

			continue;
		}

		if (AJanggiTile* Tile = Cast<AJanggiTile>(Actor))
		{
			if (!FirstTile)
			{
				FirstTile = Tile;
			}

			const FIntPoint TilePoint(Tile->TileX, Tile->TileY);
			if (bHasSelectedPiece && !FirstLegalTile && LegalMoves.Contains(TilePoint))
			{
				FirstLegalTile = Tile;
			}
		}
	}

	if (bHasSelectedPiece)
	{
		if (FirstOwnPiece)
		{
			return FirstOwnPiece;
		}

		if (FirstLegalTile)
		{
			return FirstLegalTile;
		}

		if (FirstEnemyPiece)
		{
			return FirstEnemyPiece;
		}

		if (FirstTile)
		{
			return FirstTile;
		}
	}

	if (FirstPiece)
	{
		return FirstPiece;
	}

	return FirstTile;
}

const TCHAR* AJanggiPlayerController::GetTraceActorType(AActor* Actor) const
{
	if (Cast<AJanggiPieceBase>(Actor))
	{
		return TEXT("Piece");
	}

	if (Cast<AJanggiTile>(Actor))
	{
		return TEXT("Tile");
	}

	return TEXT("Other");
}

AJanggiBoard* AJanggiPlayerController::FindJanggiBoard()
{
	if (CachedBoard)
	{
		return CachedBoard;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<AJanggiBoard> It(World); It; ++It)
	{
		CachedBoard = *It;
		return CachedBoard;
	}

	return nullptr;
}
