# Plan

## 목표
Android 앱을 세로 고정으로 전환하고, 세로 화면에서 장기판 전체가 보이도록 카메라 거리를 자동 보정한다.

## 구현 결과
1. Android 설정 `Orientation=Portrait`로 변경한다.
2. `AJanggiPlayerController`에 세로 화면 카메라 보정 최소/최대 배율 값을 추가한다.
3. `SetupFixedCamera()`에서 뷰포트 가로/세로 비율을 계산한다.
4. 비율이 1보다 작으면 카메라 거리 배율에 세로 보정값을 곱한다.
5. 카메라 로그에 뷰포트 크기, aspect ratio, 세로 보정 배율을 출력한다.
6. Win64 빌드와 Android `BuildCookRun` 배포가 성공했다.

## 가독성 5칙
- Early Return: 뷰포트 크기가 유효하지 않으면 보정 없이 기존 계산을 사용한다.
- Contextual Naming: `PortraitCameraMinAspect`, `PortraitCameraMaxDistanceScale`처럼 역할이 드러나는 이름을 사용한다.
- Magic Number Hunter: 세로 보정 임계값과 최대 배율은 UPROPERTY로 둔다.
- Parameter Object: 카메라 계산 인자가 아직 작아 객체화하지 않는다.
- Complexity Check: 기존 7/10에서 화면 비율 보정을 분리해 8/10을 목표로 한다.

## 완료 기준
- Android가 세로 방향으로 실행된다.
- 세로 화면에서 장기판 전체가 화면 안에 들어오는지 실기기에서 확인한다.
- 터치 드래그 이동 흐름은 유지된다.
- Git에는 설정/코드/문서만 남고 패키징 산출물은 제외된다.
