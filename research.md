# Research

## 현재 구조
- 프로젝트는 UE 5.7 C++ 런타임 모듈 `dusio`와 `L_JanggiBoard` 맵을 사용한다.
- Android SDK/NDK 설치와 UE Android 플랫폼 인식은 완료되어 Android APK 빌드가 가능하다.
- `AJanggiPlayerController`는 PC/모바일 드래그 입력을 직접 처리한다.

## 확인된 문제
- 휴대폰 실행 시 `Failed to open descriptor file ../../../dusio/dusio.uproject`가 표시된다.
- 직전 설치 APK는 UBT Android 타깃 빌드 산출물이라 게임 content cook/stage/package 흐름을 완전히 거치지 않았다.
- 현재 Android 설정은 `bPackageDataInsideApk=False`라 실기기 단독 APK 설치 테스트에서 descriptor/content 누락 문제가 생기기 쉽다.

## 판단
- 실기기 Development 테스트는 `RunUAT BuildCookRun`으로 cook, stage, pak, package, deploy까지 수행해야 한다.
- 초기 실기기 테스트 편의상 `bPackageDataInsideApk=True`로 바꿔 APK 단독 설치에 필요한 데이터를 포함한다.
- `Build/` 폴더는 Android 패키징 중 생성되는 산출물이므로 Git 추적 대상에서 제외한다.

## 제약
- 드래그 이동 규칙, 턴 전환, 기물 배치, 카메라 게임플레이 값은 변경하지 않는다.
