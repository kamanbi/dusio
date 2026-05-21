# Research

## 현재 구조

- `AJanggiPlayerController`는 마우스/터치 press에서 기물을 찾고 `AJanggiBoard::BeginDragPiece()`를 호출한다.
- 드래그 중에는 `PreviewDraggedPieceAtWorld()`로 기물이 입력 위치를 따라간다.
- release 시 `FinishDragOnTile()`이 목적지 타일을 받아 이동/잡기를 처리한다.
- `CurrentTurn` 기준으로 현재 턴 기물만 선택/드래그 가능하다.
- 이동 성공 후 `SwitchTurnAfterMove()`가 Red/Blue 턴을 전환한다.

## 확정 방향

- 이동 방식은 드래그앤드롭 전용이다.
- 드롭다운 좌표 이동과 HUD 콤보박스 이동은 사용하지 않는다.
- `MoveSelectedPieceToBoardPosition()`은 호환용 함수로 남기되 이동하지 않는다.
- 타일 단순 클릭만으로는 이동하지 않는다.

## 유지할 범위

- 기물 이동 규칙, 턴 전환, 색상, 바닥 정렬, 스케일, 하이라이트는 유지한다.
- AI, 온라인, 광고, 모델 교체, 애니메이션, FX는 구현하지 않는다.
