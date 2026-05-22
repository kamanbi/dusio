# Plan

## 목표
Android를 다시 가로 고정으로 되돌리고, 모바일 터치 드래그의 hit 판정을 안정화한다.

## 구현 결과
1. Android 설정을 `Orientation=Landscape`로 되돌린다.
2. 드래그 시작 trace에서 점유 타일이 맞으면 `Tile->CurrentPiece`로 드래그를 시작한다.
3. trace 선택 함수에 드래그 시작/드래그 종료 목적을 구분하는 인자를 추가한다.
4. 드래그 종료 중에는 합법 타일을 같은 팀 기물보다 우선 반환한다.
5. Win64 Editor 빌드와 Android `BuildCookRun` 배포로 검증한다.

## 가독성 5칙
- Early Return: 보드 없음, hit 없음, 점유 기물 없음은 초반에 종료한다.
- Contextual Naming: `bPreferDragTarget`으로 trace 목적을 드러낸다.
- Magic Number Hunter: 새 숫자 상수는 추가하지 않는다.
- Parameter Object: trace 인자가 3개라 객체화는 보류한다.
- Complexity Check: 시작/종료 trace 우선순위를 분리해 입력 안정성 목표를 8/10으로 둔다.

## 완료 기준
- Android가 가로 방향으로 실행된다.
- 기물 대신 점유 타일을 터치해도 해당 기물 드래그가 시작된다.
- 드래그 종료 시 합법 타일이 같은 팀 기물보다 우선되어 불필요한 실패가 줄어든다.
- 터치 드래그 이동 흐름과 턴 전환은 유지된다.
