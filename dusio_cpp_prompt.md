# 작업 요청: Unreal Engine C++ 기반 조선 SD 장기 프로토타입 1단계 구현

## 현재 프로젝트 상태

Unreal Engine 5.7 기반 프로젝트를 생성했다.

프로젝트명은 `dusio`이며, 현재 TopDown 템플릿 기반으로 시작했다.

현재 진행된 내용은 다음과 같다.

1. 기본 TopDown 예제 맵에서 불필요한 파란 구조물과 장애물을 대부분 삭제했다.
2. 새 레벨을 `L_JanggiBoard` 이름으로 저장했다.
3. 현재 맵에는 기본 바닥 Floor, 조명, PlayerStart, 기본 TopDown 관련 요소가 남아 있다.
4. 콘텐츠 브라우저 기준으로 다음 폴더 구조를 만들기 시작했다.

```text
Content/
  TopDown/
    Blueprints/
      Board/
      Game/
      Pieces/
      UI/
```

5. 원래는 블루프린트로 `BP_Tile`을 직접 만들려고 했으나, 수작업보다 C++로 기본 구조를 구현하는 방향으로 전환한다.
6. 목표는 조선시대 SD 캐릭터 장기 게임이며, 1차 목표는 캐릭터/애니메이션이 아니라 장기판 좌표 시스템과 기물 이동 로직의 기반을 만드는 것이다.

---

## 전체 게임 방향

이 게임은 조선시대 배경의 3D SD 장기 게임이다.

기물 구성은 다음과 같다.

| 장기 기물 | 게임 캐릭터 |
|---|---|
| 왕 | 왕 캐릭터 |
| 사 | 내시 |
| 차 | 창을 든 병사 |
| 포 | 대포 |
| 말 | 기마병 |
| 졸 | 일반 병사 |

게임 방식은 실제 장기처럼 상대 기물을 잡으면 내 기물이 상대 기물의 자리로 이동하여 그 칸을 차지한다.

나중에 추가할 연출은 다음과 같다.

- 일반 기물은 달려가서 공격 후 상대 기물을 연기 FX로 사라지게 한다.
- 포는 포탄을 발사하고, 상대 기물이 연기로 사라진 뒤 포가 해당 위치로 이동한다.
- 마지막 왕이 제거될 때는 카메라 클로즈업, 슬로우 모션, 피격 애니메이션, 연기 FX, 승리 UI가 나오는 시네마틱 연출을 추가한다.

이번 작업 범위에서는 연출은 제외하고, C++ 기반 게임 로직 뼈대만 구현한다.

---

# 이번 작업 목표

Unreal Engine C++로 다음 클래스를 만든다.

## 1. AJanggiTile

장기판의 칸 하나를 담당하는 Actor 클래스.

### 역할

- 9 x 10 장기판의 각 칸을 표현한다.
- 좌표 정보를 가진다.
- 현재 칸에 기물이 있는지 확인할 수 있다.
- 선택 가능/이동 가능 상태를 시각적으로 표시할 수 있도록 기본 하이라이트 기능을 가진다.

### 필요 변수

```cpp
int32 TileX;
int32 TileY;

bool bIsOccupied;

AJanggiPieceBase* CurrentPiece;
```

### 필요 기능

```cpp
void InitTile(int32 InX, int32 InY);
void SetCurrentPiece(AJanggiPieceBase* Piece);
void ClearCurrentPiece();

bool IsOccupied() const;

void SetHighlight(bool bEnable);
void SetSelectedHighlight(bool bEnable);
```

### 컴포넌트

- RootComponent
- UStaticMeshComponent* TileMesh

임시로 Cube Mesh를 사용해도 된다.

Tile 크기는 100 x 100 정도를 기준으로 한다.

---

## 2. AJanggiPieceBase

모든 장기 기물의 부모 Actor 클래스.

### 역할

- 기물 공통 정보 관리
- 팀 정보 관리
- 현재 보드 좌표 관리
- 이동 처리
- 잡기 처리의 기본 뼈대 제공

### 필요 Enum

```cpp
UENUM(BlueprintType)
enum class EJanggiTeam : uint8
{
    Red,
    Blue
};

UENUM(BlueprintType)
enum class EJanggiPieceType : uint8
{
    King,
    Guard,
    Chariot,
    Cannon,
    Horse,
    Soldier
};
```

### 필요 변수

```cpp
EJanggiTeam Team;
EJanggiPieceType PieceType;

int32 BoardX;
int32 BoardY;

bool bIsAlive;
```

### 필요 기능

```cpp
void InitPiece(EJanggiTeam InTeam, EJanggiPieceType InPieceType, int32 InX, int32 InY);
void SetBoardPosition(int32 InX, int32 InY);
void MoveToWorldLocation(const FVector& TargetLocation);
void Capture();
```

이번 단계에서는 애니메이션 없이 즉시 이동해도 된다.

---

## 3. AJanggiBoard

장기판 전체를 관리하는 Actor 클래스.

### 역할

- 9 x 10 장기판 자동 생성
- Tile 배열 관리
- 좌표로 Tile 찾기
- 기물 배치 준비
- 향후 이동 가능 칸 계산의 중심 클래스

### 필요 변수

```cpp
int32 BoardWidth = 9;
int32 BoardHeight = 10;
float TileSize = 100.0f;

TSubclassOf<AJanggiTile> TileClass;
TSubclassOf<AJanggiPieceBase> PieceClass;

TArray<AJanggiTile*> Tiles;
TArray<AJanggiPieceBase*> Pieces;
```

### 필요 기능

```cpp
void GenerateBoard();
AJanggiTile* GetTileAt(int32 X, int32 Y) const;
FVector BoardToWorld(int32 X, int32 Y) const;
bool IsInsideBoard(int32 X, int32 Y) const;
void ClearHighlights();
```

### 생성 방식

BeginPlay에서 `GenerateBoard()`를 호출한다.

9 x 10 타일을 생성한다.

좌표 기준은 다음과 같이 한다.

```text
X = 0 ~ 8
Y = 0 ~ 9
```

각 Tile 위치는 다음 공식으로 배치한다.

```cpp
WorldX = X * TileSize;
WorldY = Y * TileSize;
WorldZ = 0;
```

보드 중심 정렬을 위해 전체 Offset을 적용해도 된다.

---

## 4. 기본 기물 배치 기능

이번 작업에서 최소한 테스트용 기물 배치를 구현한다.

`AJanggiBoard`에 함수 추가:

```cpp
void SpawnTestPieces();
```

기본 테스트로 다음만 배치한다.

```text
Red King  : (4, 0)
Blue King : (4, 9)
Red Soldier : (4, 3)
Blue Soldier: (4, 6)
```

기물은 임시 Cube 또는 Sphere Mesh로 표현해도 된다.

팀 구분은 임시 Material 또는 색상으로 구분한다.

---

# 장기 룰은 이번 단계에서 전부 구현하지 않는다

이번 단계에서는 룰 전체를 구현하지 말고, 아래 구조만 준비한다.

`AJanggiPieceBase` 또는 `AJanggiBoard`에 다음 함수의 기본 뼈대를 만든다.

```cpp
TArray<FIntPoint> GetLegalMoves(AJanggiPieceBase* Piece) const;
```

현재는 테스트용으로 주변 1칸 또는 빈 배열을 반환해도 된다.

이후 기물별 룰을 구현할 수 있도록 switch 구조를 만든다.

```cpp
switch (Piece->PieceType)
{
    case EJanggiPieceType::King:
        break;
    case EJanggiPieceType::Guard:
        break;
    case EJanggiPieceType::Chariot:
        break;
    case EJanggiPieceType::Cannon:
        break;
    case EJanggiPieceType::Horse:
        break;
    case EJanggiPieceType::Soldier:
        break;
}
```

---

# 향후 구현할 장기 룰 기준

이 함수 구조는 나중에 다음 규칙을 넣을 수 있어야 한다.

## 왕

- 궁성 안에서만 이동
- 한 칸 이동
- 직선/대각선 궁성 라인 기준 이동

## 사

- 궁성 안에서 이동
- 대각 이동 중심

## 차

- 직선 이동
- 중간에 기물이 있으면 그 뒤로 이동 불가
- 상대 기물은 잡고 그 칸 차지

## 포

- 반드시 하나의 기물을 넘어야 이동 또는 공격 가능
- 포는 포를 넘을 수 없음
- 포끼리는 잡을 수 없음

## 말

- 장기 말 이동
- 첫 직선 경로가 막히면 이동 불가

## 졸

- 전진
- 좌우 이동
- 후퇴 불가

---

# 입력 처리

이번 단계에서는 복잡한 터치 입력까지 구현하지 않아도 된다.

단, 향후 클릭/터치로 선택할 수 있도록 각 Tile 또는 Piece에 클릭 이벤트를 붙일 수 있는 구조를 열어둔다.

가능하면 다음 함수 뼈대를 준비한다.

```cpp
void OnTileClicked(AJanggiTile* Tile);
void OnPieceClicked(AJanggiPieceBase* Piece);
```

---

# 파일 생성 위치

프로젝트의 C++ 소스 폴더에 다음 파일을 생성한다.

```text
Source/dusio/JanggiTile.h
Source/dusio/JanggiTile.cpp

Source/dusio/JanggiPieceBase.h
Source/dusio/JanggiPieceBase.cpp

Source/dusio/JanggiBoard.h
Source/dusio/JanggiBoard.cpp
```

프로젝트 모듈명이 실제로 다르면 현재 `.uproject`와 `Source` 구조를 확인해서 올바른 모듈명에 맞춰 include를 작성한다.

---

# Unreal 코드 작성 요구사항

1. Unreal Engine 5.7 기준으로 컴파일 가능한 C++ 코드로 작성한다.
2. UCLASS, UPROPERTY, UFUNCTION 매크로를 적절히 사용한다.
3. Blueprint에서도 조정 가능하도록 필요한 값은 `EditAnywhere`, `BlueprintReadWrite` 또는 `BlueprintReadOnly`를 사용한다.
4. Null 체크를 넣는다.
5. 컴파일 에러가 나지 않도록 forward declaration과 include를 정리한다.
6. `AJanggiBoard`를 레벨에 배치하면 BeginPlay에서 9 x 10 장기판이 자동 생성되어야 한다.
7. 타일과 기물은 임시 StaticMeshComponent로 보이면 된다.
8. 이번 작업에서는 고급 UI, 애니메이션, FX, 온라인 기능은 구현하지 않는다.

---

# 완료 기준

작업 완료 후 다음이 가능해야 한다.

1. Unreal Editor에서 C++ 컴파일 성공
2. `AJanggiBoard` Actor를 레벨에 배치 가능
3. Play 실행 시 9 x 10 타일이 자동 생성됨
4. 테스트용 왕/졸 기물이 임시 메시로 표시됨
5. 각 타일은 X/Y 좌표를 가지고 있음
6. 이후 장기 룰과 클릭 이동 기능을 붙일 수 있는 구조가 준비됨

---

# 추가 요청

코드 작성 후, 다음 내용을 간단히 정리해줘.

1. 생성한 파일 목록
2. 각 클래스 역할
3. Unreal Editor에서 다음에 해야 할 작업
4. 컴파일 후 문제가 생길 수 있는 부분
