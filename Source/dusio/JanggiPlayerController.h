#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "JanggiPlayerController.generated.h"

class AJanggiBoard;
class ACameraActor;
class AJanggiPieceBase;
class AJanggiTile;

UCLASS()
class DUSIO_API AJanggiPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AJanggiPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera")
	float CameraHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera")
	float CameraDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera")
	float CameraBlendTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera")
	float CameraYawOffsetDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera", meta = (ClampMin = "5.0", ClampMax = "170.0"))
	float CameraFieldOfView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera", meta = (ClampMin = "0.1"))
	float CameraPlanarDistanceScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera", meta = (ClampMin = "0.1"))
	float CameraHeightScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Camera", meta = (ClampMin = "1.0"))
	float CameraDistanceMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Lighting")
	float DirectionalLightOffsetTilesX;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void AddYawInput(float Val) override;
	virtual void AddPitchInput(float Val) override;
	virtual void AddRollInput(float Val) override;

private:
	UPROPERTY()
	TObjectPtr<AJanggiBoard> CachedBoard;

	UPROPERTY()
	TObjectPtr<ACameraActor> SpawnedCamera;

	UPROPERTY()
	TObjectPtr<AJanggiPieceBase> DraggedPiece;

	UPROPERTY()
	TObjectPtr<AJanggiTile> OriginalTile;

	FVector OriginalWorldLocation;
	bool bIsDraggingPiece;
	bool bIsTouchDragging;
	ETouchIndex::Type ActiveTouchFinger;
	FVector LastTouchScreenPosition;

	void HandlePrimaryPressed();
	void HandlePrimaryReleased();
	void HandleTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
	bool BeginDragFromActor(AActor* HitActor, const TCHAR* InputType);
	void FinishDragAtActor(AActor* HitActor, const TCHAR* InputType);
	void CancelActiveDrag(const TCHAR* InputType, const TCHAR* Reason);
	void ClearLocalDragState();
	void ApplyBoardOnlyInputMode();
	void ReleaseExistingPawn();
	void SetupFixedCamera();
	void ApplyDirectionalLightOffset();
	void UpdateMouseDragPreview();
	void UpdateDragPreviewAtScreenPosition(float ScreenX, float ScreenY);
	bool TraceActorUnderCursor(AActor*& HitActor);
	bool TraceActorAtScreenPosition(float ScreenX, float ScreenY, AActor*& HitActor);
	AActor* SelectTraceTarget(const TArray<FHitResult>& HitResults, AJanggiBoard* Board) const;
	const TCHAR* GetTraceActorType(AActor* Actor) const;
	AJanggiBoard* FindJanggiBoard();
};
