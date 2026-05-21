# Research

## 현재 구조
- `AJanggiBoard`는 보드 상태, 합법 이동, 선택 하이라이트, 이동, 잡기, 턴 전환을 담당한다.
- `AJanggiPlayerController`는 마우스/터치 입력, 포인터 트레이스, 드래그 미리보기, 고정 카메라, 조명 위치 보정을 담당한다.
- `AJanggiPieceBase`는 기물 팀/타입, 외형, 스케일, 머티리얼, 보드 좌표, 잡힘 상태를 담당한다.
- `AJanggiTile`은 보드 좌표, 점유 상태, 충돌, 하이라이트 머티리얼을 담당한다.

## 확인된 동작
- 이동 UX는 드래그 전용이다: 기물을 누르면 선택/하이라이트, 드래그 중 미리보기, 놓으면 이동/잡기 판정.
- 타일 클릭 이동과 드롭다운 좌표 이동은 `AJanggiBoard`에서 비활성화되어 있다.
- `CurrentTurn` 기준으로 현재 턴 기물만 선택/드래그 가능하고, 이동 성공 후 턴이 전환된다.
- `LineTraceMultiByChannel`로 같은 포인터 선상의 기물과 타일을 함께 확인한다.

## 이번 정리 결과
- `AJanggiPlayerController`가 `bIsDraggingPiece`, `DraggedPiece`, `OriginalTile`, `OriginalWorldLocation`을 명시적으로 보관한다.
- 터치 입력은 유효한 기물 드래그가 시작된 경우에만 터치 드래그 상태로 들어간다.
- 타일 press는 이동으로 처리하지 않고 로그만 남긴다.
- 이동 실패, 같은 팀 기물, 빈 공간 release는 원위치 복귀와 하이라이트 정리를 수행한다.

## 제약
- 실제 장기 이동 규칙, 기물 색상, 바닥 정렬, 스케일, 카메라 값은 변경하지 않았다.
- AI, 온라인, 광고, 모델 교체, 애니메이션, FX는 이번 범위가 아니다.
