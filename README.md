## 프로젝트 설명
**프로젝트명:** 던전주식회사\
**엔진:** Unreal5.4\
**참여인원:** 개인프로젝트

## 주요 기능 소개
### 1. 인벤토리 & 퀵슬롯 시스템 
- **이벤트 기반 UI 갱신:** `Delegate`를 활용해 데이터 변경 시에만 UI를 업데이트하여 리소스 낭비 최소화
[[code]](https://github.com/yoonjiii218/Dungeon/blob/main/InventorySystem/Inventory/FPSInventoryComponent.cpp#L243)

- **인터페이스 기반 상호작용:** `IItemInterface`로 아이템 공통 동작을 추상화하여 유지보수성 및 확장성 확보

- **드래그 앤 드롭:** `NativeDrop`을 오버라이드하여 아이템 이동 및 외부 드롭 로직 처리

- **슬롯 로직:** 모듈러 연산(`%`)을 이용한 슬롯 인덱스 제어
[[code]](https://github.com/yoonjiii218/Dungeon/blob/main/UI/FPSMainUI.cpp#L41)

### 2. 플레이어 추적 시스템
- **Behavior Tree 상태 관리:** 직관적이고 확장성 있는 행동 패턴 구현

- **정밀한 공격 판정:**  `AnimMontage`와 `AnimNotify`를 이용하여 애니메이션과 공격 판정 타이밍의 불일치 문제 해결

