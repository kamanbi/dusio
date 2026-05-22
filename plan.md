# Plan

## 목표
Android 실기기에서 descriptor 누락 오류 없이 실행되는 Development 패키지를 만든다.

## 구현 순서
1. `DefaultEngine.ini`에서 `bPackageDataInsideApk=True`로 변경한다.
2. `.gitignore`에 Android 패키징 산출물 `Build/`를 추가한다.
3. `RunUAT BuildCookRun`으로 Android cook, stage, pak, package, deploy를 실행한다.
4. 실기기에서 앱 실행 상태와 로그를 확인한다.
5. 성공한 설정 변경을 커밋/푸시한다.

## 가독성 5칙
- Early Return: 코드 변경 없음.
- Contextual Naming: 엔진 표준 Android 설정 키를 그대로 사용한다.
- Magic Number Hunter: 새 숫자 상수 추가 없음.
- Parameter Object: 설정 변경만 수행하므로 해당 없음.
- Complexity Check: 패키징 흐름을 UBT 단독 빌드에서 UAT 패키징으로 분리해 운영 명확도를 8/10으로 유지한다.

## 완료 기준
- APK 설치 후 `Failed to open descriptor file` 오류가 재현되지 않는다.
- 앱이 `GameActivity`로 실행되고 장기판 확인 단계로 진입한다.
- Git에는 소스/설정만 남고 Android 생성 산출물은 제외된다.
