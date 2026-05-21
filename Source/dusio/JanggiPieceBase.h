#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JanggiPieceBase.generated.h"

class UMaterialInterface;

UENUM(BlueprintType)
enum class EJanggiTeam : uint8
{
	Red UMETA(DisplayName = "Red"),
	Blue UMETA(DisplayName = "Blue")
};

UENUM(BlueprintType)
enum class EJanggiPieceType : uint8
{
	King UMETA(DisplayName = "King"),
	Guard UMETA(DisplayName = "Guard"),
	Chariot UMETA(DisplayName = "Chariot"),
	Cannon UMETA(DisplayName = "Cannon"),
	Horse UMETA(DisplayName = "Horse"),
	Elephant UMETA(DisplayName = "Elephant"),
	Soldier UMETA(DisplayName = "Soldier")
};

UCLASS()
class DUSIO_API AJanggiPieceBase : public AActor
{
	GENERATED_BODY()

public:
	AJanggiPieceBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Components")
	TObjectPtr<UStaticMeshComponent> PieceMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Piece")
	EJanggiTeam Team;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Piece")
	EJanggiPieceType PieceType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Board")
	int32 BoardX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Board")
	int32 BoardY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Janggi|Piece")
	bool bIsAlive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor RedTeamColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FLinearColor BlueTeamColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Janggi|Visual")
	FVector PieceScale;

	UFUNCTION(BlueprintCallable, Category = "Janggi|Piece")
	void InitPiece(EJanggiTeam InTeam, EJanggiPieceType InPieceType, int32 InX, int32 InY);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Piece")
	void SetBoardPosition(int32 InX, int32 InY);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Piece")
	void MoveToWorldLocation(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Janggi|Piece")
	void Capture();

	UFUNCTION(BlueprintCallable, Category = "Janggi|Visual")
	void ApplyVisualByTypeAndTeam();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SphereMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> ConeMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseVisualMaterial;

	FVector GetScaleForPieceType() const;
	UStaticMesh* GetMeshForPieceType() const;
	FLinearColor GetColorForTeam() const;
	const TCHAR* GetPieceTypeText() const;
	void ApplyMeshFloorOffset();
};
