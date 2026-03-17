#!/bin/bash
# SessionStart hook: 세션 초기화 자동화
# - origin fetch
# - 모델별 최신 claude 브랜치 탐색
# - orphan 브랜치 방식: claude 브랜치는 코드 없이 docs/config만 보유
# - 결과를 stdout으로 출력하여 Claude에게 전달

INPUT=$(cat)
CWD=$(echo "$INPUT" | jq -r '.cwd')
SOURCE=$(echo "$INPUT" | jq -r '.source')
MODEL=$(echo "$INPUT" | jq -r '.model')

cd "$CWD" || exit 0

# resume/compact 시에는 스킵
if [ "$SOURCE" != "startup" ]; then
  exit 0
fi

# 모델명에서 prefix 추출
case "$MODEL" in
  *opus*)   PREFIX="claude/opus" ;;
  *sonnet*) PREFIX="claude/sonnet" ;;
  *haiku*)  PREFIX="claude/haiku" ;;
  *)        PREFIX="claude" ;;
esac

# 원격 브랜치 최신화
git fetch origin 2>/dev/null

# 해당 모델의 최신 claude 브랜치 찾기 (커밋 날짜 기준 정렬)
LATEST_BRANCH=$(git for-each-ref \
  --sort=-committerdate \
  --format='%(refname:short)' \
  refs/remotes/origin/${PREFIX}* 2>/dev/null \
  | head -1)

echo "=== Session Init ==="
echo "MODEL: $MODEL"
echo "PREFIX: $PREFIX"

if [ -n "$LATEST_BRANCH" ]; then
  echo "LATEST_CLAUDE_BRANCH: $LATEST_BRANCH"
  echo "BRANCH_MODE: inherit"
  echo "ACTION: 세션 브랜치를 ${LATEST_BRANCH} 기반으로 생성하세요. (이전 작업 이어받기)"
else
  echo "LATEST_CLAUDE_BRANCH: (없음)"
  echo "BRANCH_MODE: orphan"
  echo "ACTION: orphan 브랜치로 새로 생성하세요. (코드 없이 CLAUDE.md, .claude/, docs/ 만 포함)"
fi

echo ""
echo "※ claude 브랜치에는 코드 파일이 없습니다."
echo "※ 코드 확인은 반드시 git show origin/main:path 또는 git show origin/feature/*:path 를 사용하세요."
