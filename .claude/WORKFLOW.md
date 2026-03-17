# 작업 흐름

## 작업 시작 전 필수 체크 (매번 실행!)
1. ✅ **CLAUDE.md 읽기** — Read tool로 명시적으로 읽고 작업 규칙 확인
2. ✅ **작업 브랜치 확인** — 현재 브랜치에서만 작업 (새 브랜치 생성 금지)

## 작업 흐름

### 1. 코드 확인 (필요 시)
- ⚠️ **원격 브랜치 확인 전 반드시 `git fetch origin` 먼저 실행**
- 코드는 항상 원격에서 확인: `git show origin/main:경로`
- ⚠️ **로컬 워킹 트리에 코드 파일이 없으므로 Read/Grep 직접 사용 불가** → `git show` 사용

### 2. 작업 수행
- 작업 브랜치에서만 작업 (md 파일 작성)
- 작업 완료 후 커밋

### 3. 커밋/푸시
- 작업 브랜치에서 커밋 후 푸시
- 푸시 명령: `git push -u origin {작업_브랜치}`

### 4. docs/ 결과물을 main에 반영 (plumbing 방식)
main에 직접 push 불가하므로, plumbing으로 main 기반 커밋을 만들어 claude/ 브랜치에 push → 사용자가 fast-forward merge

**⚠️ 필수 체크리스트 (반드시 순서대로):**
1. **gh CLI 사용 금지** — 이 환경은 git proxy만 지원, GitHub API 인증 불가
2. **포함 대상은 docs/ 내 파일만** — .claude/, CLAUDE.md 등 설정 파일 절대 포함 금지
3. **이번 세션 작업분만 선별** — `origin/main..HEAD` 전체 diff가 아니라, 이번 세션에서 실제 수정한 파일만 명시적으로 나열
   - 이전 PR에서 이미 반영된 파일이 diff에 포함될 수 있으므로 주의
   - 파일 목록을 update-index 전에 사용자에게 확인받을 것

```bash
# 1. main 최신화
git fetch origin main
MAIN_SHA=$(git rev-parse origin/main)

# 2. main 트리에 이번 세션 결과 파일만 추가 (파일을 명시적으로 나열)
GIT_INDEX_FILE=/tmp/idx git read-tree origin/main
for file in "docs/대상1.md" "docs/대상2.md"; do
  BLOB=$(git rev-parse "HEAD:$file")
  MODE=$(git ls-tree HEAD -- "$file" | awk '{print $1}')
  GIT_INDEX_FILE=/tmp/idx git update-index --add --cacheinfo "$MODE,$BLOB,$file"
done
NEW_TREE=$(GIT_INDEX_FILE=/tmp/idx git write-tree)

# 3. main을 부모로 서명된 커밋 생성
NEW_COMMIT=$(git commit-tree $NEW_TREE -p $MAIN_SHA -S -m "커밋 메시지")

# 4. claude/ 브랜치에 push → 사용자가 merge
git push origin $NEW_COMMIT:refs/heads/claude/docs-to-main-{세션ID}
```
사용자: GitHub PR merge 또는 `git merge --ff-only` 실행

## 토큰 최적화 규칙

### 최우선 원칙
0. **원격 브랜치 확인 전 반드시 fetch** — 작업 중 원격 확인 시 `git fetch origin` 실행
1. **브랜치 전환 대신 git show 사용** — 다른 브랜치 코드 확인 시 checkout 대신 `git show origin/branch:path/file` 사용
2. **Read 대신 Grep 우선 사용** — 전체 파일보다 필요한 부분만 검색 (output_mode: "content", -A/-B/-C 활용)
3. **offset/limit 적극 활용** — 큰 파일은 부분 읽기 (Read의 offset/limit, Grep의 head_limit)
4. **병렬 도구 호출 최소화** — 독립적인 작업만 병렬 실행, 의존성 있는 작업은 순차 실행
5. **Task tool 활용** — 반복적인 탐색/검색 작업은 Task tool의 Explore agent 사용

### 문서 작성/수정
- **500줄 이상 파일은 전체 재작성 금지** (Edit 또는 새 파일로 작성)
- 설계 변경 시: 새 파일로 작성, 기존 파일 보관
- 소폭 수정: Edit 사용

## 문서 작성 원칙
- **길이 제한**: 400줄 이내 (넘으면 분할)
  - **예외**: 선행 작업을 총망라하는 적용/종합 문서(예: X-7, F-1)는 제한 없음 — 압축 시 내용 왜곡 위험
- **코드 예시**: 전체 구현 X, 패턴만 (3-5줄)
- **배제 항목**: 예상 시간, 불확실한 추정치
- **방안 비교**: 장단점 간략히, 권장안 명시
