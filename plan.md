# Plan

## 목표

프로젝트 이동 방식을 드래그앤드롭 전용으로 확정하고, 드롭다운/HUD 좌표 이동 작업 흔적을 제거한다.

## 작업 순서

1. UMG/Slate 의존성과 임시 HUD 위젯 파일을 제거한다.
2. `BeginDragPiece()`에서 현재 턴 기물 선택과 드래그 시작을 유지한다.
3. `MoveSelectedPieceToTile()`은 드래그 중일 때만 이동을 허용한다.
4. `MoveSelectedPieceToBoardPosition()`은 이동하지 않고 드래그 전용 로그만 남긴다.
5. 마우스/터치 press는 기물 드래그 시작 경로를 사용한다.
6. UnrealBuildTool로 Editor/Game 타깃 빌드를 확인한다.

## 가독성 5칙

- Early Return: null Piece, Tile, Board 검사는 함수 초반에 처리한다.
- Contextual Naming: `BeginDragPiece`, `FinishDragOnTile`, `bIsDraggingPiece` 흐름을 유지한다.
- Magic Number Hunter: 새 하드코딩 숫자를 추가하지 않는다.
- Parameter Object: 이동 대상은 기존 Tile 포인터 구조를 유지한다.
- Complexity Check: 현재 목표 가독성 점수는 8/10이다.

## 완료 기준

- 기물을 누르고 드래그한 뒤 이동 가능 타일에 놓으면 이동된다.
- 이동 성공 후 턴이 전환된다.
- 드롭다운 좌표 API는 이동하지 않는다.
- 빌드가 성공한다.
