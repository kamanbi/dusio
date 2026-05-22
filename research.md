# Research

## 현재 구조
- Android Development 패키징과 실기기 배포는 `RunUAT BuildCookRun`으로 성공한다.
- Android 앱 방향은 `Orientation=Portrait`로 세로 고정되어 있다.
- 고정 카메라는 `AJanggiPlayerController::SetupFixedCamera()`에서 보드 크기, 뷰포트 비율, 배율로 위치를 계산한다.
- 드래그 입력은 `AJanggiPlayerController`의 마우스/터치 공통 흐름을 사용한다.

## 확인된 요구
- Android 화면 방향을 세로 고정으로 유지한다.
- 세로 화면은 가로폭이 좁아 기존 카메라 거리로는 장기판 좌우/상하가 잘릴 수 있다.
- 카메라는 보드 중심을 바라보는 기존 사선뷰를 유지하되, 뷰포트 비율을 보고 거리만 보정한다.

## 검증 결과
- Win64 Editor/Game 빌드가 성공했다.
- Android `BuildCookRun` cook, package, deploy가 성공했다.
- 실기기에서 `requestedOrientation=SCREEN_ORIENTATION_PORTRAIT`가 확인됐다.
- FOV, 기물 크기, 이동 규칙, 턴 로직은 변경하지 않았다.

## 제약
- 세로 화면에서는 장기판 전체를 보이게 하는 것이 우선이고, 기물이 가로 화면보다 작아질 수 있다.
- 실제 터치 오프셋과 화면 잘림은 패키징 후 실기기에서 확인해야 한다.
