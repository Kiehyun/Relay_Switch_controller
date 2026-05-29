# Release Notes (Draft)

## Unreleased (2026-03-21)

### ✨ 주요 변경사항
- `mp4dome_4ch` Python 버전 구성 파일을 추가했습니다.
- 4채널 릴레이 제어 UI/로직(버튼, 채널 토글, 연결/해제 처리)을 포함한 이식 코드를 반영했습니다.
- 시리얼 제어 및 트위터(X) 연동을 위한 기본 의존성 정의와 환경변수 예시를 추가했습니다.
- 루트 `.gitignore`를 추가해 Python/빌드/로그/가상환경 산출물 등 불필요 파일이 커밋되지 않도록 정리했습니다.

### ➕ Added
- `.gitignore`
- `mp4dome_4ch/.env.example`
- `mp4dome_4ch/README_PYTHON.md`
- `mp4dome_4ch/buttons.pde`
- `mp4dome_4ch/channels.pde`
- `mp4dome_4ch/mp4dome.pde`
- `mp4dome_4ch/mp4dome_4ch.py`
- `mp4dome_4ch/mp4dome_4ch/mp4domez_4ch.ino`
- `mp4dome_4ch/requirements-python.txt`

### 📦 Dependencies
- `pyserial>=3.5`
- `tweepy>=4.14`

### ⚙️ Environment
아래 값이 설정된 경우에만 트위터(X) 전송 기능이 활성화됩니다.
- `TWITTER_CONSUMER_KEY`
- `TWITTER_CONSUMER_SECRET`
- `TWITTER_ACCESS_TOKEN`
- `TWITTER_ACCESS_TOKEN_SECRET`

### 🚀 배포/운영 참고
- 기존 `master` 기준 커밋: `c98042e`
- Python 실행 전 의존성 설치:
  - `pip install -r mp4dome_4ch/requirements-python.txt`
