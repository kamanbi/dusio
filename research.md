# Research

## 현재 구조
- Android Development 패키징과 실기기 배포는 `RunUAT BuildCookRun`으로 성공한다.
- 최근 세로 고정으로 바꿨지만, 장기판과 기물이 너무 작아져 플레이성이 떨어진다.
- 드래그 입력은 `AJanggiPlayerController`가 `LineTraceMultiByChannel` 결과를 해석해 처리한다.
- 기물 Mesh가 작거나 터치 위치가 조금 벗어나면 기물 대신 점유 타일이 맞을 수 있다.

## 확인된 문제
- Android는 다시 가로 고정이 필요하다.
- 터치 드래그가 되는 칸과 안 되는 칸이 섞여 있다.
- 현재 드래그 시작은 `Piece` hit만 허용하고, `Tile` hit는 무시한다.
- 현재 드래그 종료 trace는 선택 기물이 있을 때 같은 팀 기물을 먼저 반환할 수 있어, 뒤쪽 타일 판정을 방해할 수 있다.

## 판단
- `Orientation=Landscape`로 되돌린다.
- 드래그 시작 시 점유 타일이 hit되면 그 타일의 `CurrentPiece`로 드래그를 시작한다.
- 드래그 종료 trace는 합법 목적지 타일을 같은 팀 기물보다 우선한다.
- 카메라, 이동 규칙, 턴 로직은 유지한다.

## 검증 결과
- `dusioEditor Win64 Development` 빌드는 성공했다.
- Android `BuildCookRun` 패키징과 실기기 배포가 성공했다.
- 실기기에서 `SCREEN_ORIENTATION_LANDSCAPE`로 실행되는 것을 확인했다.
- 로그에서 점유 타일 터치가 해당 기물 드래그 시작으로 전환되는 것을 확인했다.

## 제약
- 이동 방식은 계속 드래그 전용이다.
- 실제 장기 규칙과 기물 배치에는 손대지 않는다.
