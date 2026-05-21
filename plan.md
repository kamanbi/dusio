# Plan

## 목표
Android 실기기 Development 빌드를 시도할 수 있는 기본 프로젝트 설정을 준비한다.

## 구현 순서
1. `DefaultEngine.ini`에서 모바일 실기기 테스트에 무거운 렌더링 기능을 끈다.
2. 프로젝트 Android 런타임 설정 섹션을 추가해 패키지명, 가로 고정, Arm64, SDK 기본값을 명시한다.
3. `DefaultGame.ini`의 템플릿 프로젝트 이름을 `dusio`로 정리한다.
4. UnrealBuildTool로 Win64 타깃을 빌드해 설정 변경으로 인한 기본 컴파일 문제를 확인한다.
5. `SetupAndroid.bat`로 SDK/NDK 기본 설치를 확인한다.
6. UE 5.7 Android optional component가 없으면 사용자가 Launcher에서 설치할 항목을 정리한다.

## 가독성 5칙
- Early Return: 코드 변경은 없으므로 입력 로직에는 영향이 없다.
- Contextual Naming: Android 설정은 엔진 표준 섹션명과 키를 그대로 사용한다.
- Magic Number Hunter: SDK 26/34는 UE 5.7 기본값 기준으로 명시한다.
- Parameter Object: 설정 파일 변경만 수행하므로 해당 없음.
- Complexity Check: 설정 가독성 목표는 8/10이며, Android 관련 값을 한 섹션에 모은다.

## 완료 기준
- Android 패키지명과 가로 방향 설정이 프로젝트 설정에 기록된다.
- Mobile HDR, Lumen 계열, Nanite, RayTracing/PathTracing이 실기기 테스트 기준으로 꺼진다.
- Win64 Editor/Game 빌드가 성공한다.
- Android SDK/NDK 설치 상태와 UE Android 플랫폼 지원 설치 여부가 구분된다.
- 에디터에서 사용자가 확인할 Android 빌드 체크리스트가 준비된다.
