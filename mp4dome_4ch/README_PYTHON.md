# mp4dome_4ch Python 버전

이 폴더의 `mp4dome.pde`(Processing) 기능을 Python으로 이식한 버전입니다.

## 구현된 기능
- 시리얼 포트/보드레이트 선택
- Connect / Disconnect
- 4채널 ON/OFF 버튼
- OFF 시 확인 팝업
- 채널 상태 토글 표시
- 하단 상태 메시지
- 시계(Com Clock) 표시
- 연결 상태에서 0/15/30/45초 타임스탬프 콘솔 출력
- 트위터 전송(선택)

## 파일
- `mp4dome_py.py`: 메인 앱
- `requirements-python.txt`: Python 패키지

## 설치
```bash
pip install -r requirements-python.txt
```

패키지를 미리 설치하지 않아도, 앱 실행 시 `pyserial`, `tweepy`가 없으면 자동으로 설치를 시도합니다.
(단, 인터넷 연결과 pip 사용 가능한 환경이 필요합니다.)

## 실행
```bash
python mp4dome_py.py
```

## 트위터 전송 사용(선택)
원본과 동일하게 채널 ON/OFF 시 트윗을 보내려면 아래 환경변수를 설정하세요.

- `TWITTER_CONSUMER_KEY`
- `TWITTER_CONSUMER_SECRET`
- `TWITTER_ACCESS_TOKEN`
- `TWITTER_ACCESS_TOKEN_SECRET`

설정이 없거나 `tweepy`가 없으면 트위터 기능은 자동으로 비활성화됩니다.
