# Research

## 현재 구조
- 프로젝트는 UE 5.7 C++ 런타임 모듈 `dusio`와 `L_JanggiBoard` 맵을 사용한다.
- 기본 맵과 기본 게임모드는 `DefaultEngine.ini`에서 `L_JanggiBoard`, `JanggiGameMode`로 설정되어 있다.
- 입력은 `AJanggiPlayerController`가 마우스/터치 드래그를 직접 처리한다.
- Android 전용 프로젝트 설정 섹션은 아직 프로젝트 `DefaultEngine.ini`에 없다.

## 확인된 모바일 관련 설정
- `TargetedHardwareClass=Mobile`, `DefaultGraphicsPerformance=Scalable`은 이미 적용되어 있다.
- `r.MobileHDR=True`, `r.Nanite.ProjectEnabled=True`, `r.DynamicGlobalIlluminationMethod=1`, `r.ReflectionMethod=1`, `r.RayTracing=True`, `r.PathTracing=True`가 켜져 있어 실기기 테스트 기준으로 무겁다.
- Engine 기본 Android 값은 `MinSDKVersion=26`, `TargetSDKVersion=34`, `bBuildForArm64=true`, `Orientation=SensorLandscape`다.
- `SetupAndroid.bat` 실행으로 Android Studio SDK, `android-34`, build-tools `35.0.1`, CMake `3.22.1`, NDK `27.2.12479018` 설치는 완료됐다.
- UE 5.7 설치에는 Android 플랫폼 지원 핵심 파일(`DataDrivenPlatformInfo.ini`, `SDK.json`, Android UBT 파일)이 빠져 있어 Android 타깃 빌드는 아직 불가하다.

## 리스크
- Epic Games Launcher에서 UE 5.7 Android optional component 설치가 필요하다.
- USB 드라이버, 개발자 옵션, 실기기 연결은 로컬 환경 문제라 코드에서 보장할 수 없다.
- 실제 화면 잘림 여부는 실기기 해상도와 노치/시스템 UI에 따라 패키징 후 확인해야 한다.

## 제약
- 이번 단계는 Android 실기기 테스트 준비다.
- 드래그 이동 규칙, 턴 전환, 기물 배치, 카메라 게임플레이 값은 변경하지 않는다.
