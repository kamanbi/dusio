# Plan

## 목표
PC 마우스와 모바일 터치 모두에서 드래그 전용 이동을 확정한다.

## 구현 순서
1. `AJanggiPlayerController`에 드래그 상태, 드래그 기물, 원래 타일, 원래 월드 위치를 둔다.
2. 마우스 press/release와 터치 start/move/end를 공통 드래그 시작, 미리보기, 종료 함수로 연결한다.
3. 타일 press는 이동하지 않고 무시한다.
4. release 대상이 기물이면 해당 기물의 타일, 타일이면 해당 타일을 목적지로 판정한다.
5. 합법 이동이면 기존 보드 이동/잡기 로직으로 확정하고 턴을 넘긴다.
6. 불법 이동, 같은 팀 기물, 빈 공간이면 원래 월드 위치로 복귀하고 턴을 유지한다.
7. 드롭다운 이동 함수는 호환용으로 남기되 이동하지 않는다.

## 가독성 5칙
- Early Return: 보드 없음, 활성 드래그 없음, 잘못된 터치 손가락, 히트 없음은 함수 초반에 종료한다.
- Contextual Naming: `BeginDragFromActor`, `FinishDragAtActor`, `CancelActiveDrag`, `OriginalWorldLocation`을 사용한다.
- Magic Number Hunter: 이번 작업에서는 새 하드코딩 수치를 추가하지 않는다.
- Parameter Object: 현재 헬퍼 인자가 작아 객체화는 보류한다.
- Complexity Check: 입력 상태와 보드 규칙을 분리해 가독성 목표를 8/10으로 둔다.

## 완료 기준
- 현재 턴 기물 press에서만 드래그가 시작된다.
- 드래그 중 보드 좌표는 변경되지 않고 미리보기 위치만 움직인다.
- 합법 타일 또는 상대 기물 위 release는 이동/잡기 후 턴 전환된다.
- 불법 위치 또는 같은 팀 기물 위 release는 원위치 복귀하고 턴 유지된다.
- 클릭 이동과 드롭다운 좌표 이동은 실행되지 않는다.
