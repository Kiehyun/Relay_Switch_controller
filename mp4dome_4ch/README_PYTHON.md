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
- `mp4dome_4ch.py`: 메인 앱
- `requirements-python.txt`: Python 패키지

## 설치
```bash
pip install -r requirements-python.txt
```

패키지를 미리 설치하지 않아도, 앱 실행 시 `pyserial`, `tweepy`가 없으면 자동으로 설치를 시도합니다.
자동 설치된 패키지는 전역 Python이 아니라 앱 폴더의 `.python-packages/` 아래에 저장됩니다.
(단, 첫 실행 시 인터넷 연결과 pip 사용 가능한 환경이 필요합니다.)

## 실행
Windows:
```bash
py mp4dome_4ch.py
```

macOS / Ubuntu:
```bash
python3 mp4dome_4ch.py
```

## OS별 참고
- Windows/macOS는 일반 Python 설치에 `tkinter`가 포함되는 경우가 많습니다.
- Ubuntu에서 `tkinter` 오류가 나면 `sudo apt install python3-tk`를 실행하세요.
- Ubuntu에서 시리얼 포트 권한 오류가 나면 `sudo usermod -aG dialout $USER` 실행 후 로그아웃/로그인하세요.
- 자동 시간 동기화는 Windows에서만 켜집니다. macOS/Ubuntu에서는 `Time Sync` 버튼을 눌렀을 때만 OS별 동기화 명령을 시도합니다.
- 시간 동기화는 OS에 따라 관리자 권한이 필요할 수 있습니다. 권한이 없으면 앱은 계속 실행되고 시간 동기화만 실패로 표시됩니다.

## 트위터 전송 사용(선택)
원본과 동일하게 채널 ON/OFF 시 트윗을 보내려면 아래 환경변수를 설정하세요.

- `TWITTER_CONSUMER_KEY`
- `TWITTER_CONSUMER_SECRET`
- `TWITTER_ACCESS_TOKEN`
- `TWITTER_ACCESS_TOKEN_SECRET`

설정이 없거나 `tweepy`가 없으면 트위터 기능은 자동으로 비활성화됩니다.
