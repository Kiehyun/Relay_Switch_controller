@echo off
chcp 65001 > nul
setlocal

echo ============================================
echo  MP 4Dome Controller - EXE 빌드 스크립트
echo ============================================
echo.

:: ---- PyInstaller 설치 확인 ----
python -c "import PyInstaller" 2>nul
if errorlevel 1 (
    echo [*] PyInstaller 가 없습니다. 설치합니다...
    pip install pyinstaller
    if errorlevel 1 (
        echo [!] PyInstaller 설치 실패. pip 환경을 확인해 주세요.
        pause
        exit /b 1
    )
)

:: ---- pyserial / tweepy 설치 확인 ----
python -c "import serial" 2>nul
if errorlevel 1 (
    echo [*] pyserial 설치 중...
    pip install pyserial
)
python -c "import tweepy" 2>nul
if errorlevel 1 (
    echo [*] tweepy 설치 중...
    pip install tweepy
)

echo.
echo [*] 빌드를 시작합니다...
echo.

:: ---- 아이콘 파일 유무에 따라 빌드 명령 분기 ----
if exist "app.ico" (
    pyinstaller --noconfirm MP4DomeController.spec
) else (
    echo [주의] app.ico 가 없습니다. 아이콘 없이 빌드합니다.
    pyinstaller --noconfirm MP4DomeController.spec
)

if errorlevel 1 (
    echo.
    echo [!] 빌드 실패. 위 오류 메시지를 확인해 주세요.
    pause
    exit /b 1
)

echo.
echo ============================================
echo  빌드 완료!
echo  실행 파일 위치: dist\MP4DomeController.exe
echo ============================================
echo.
echo [안내] .env 파일이 있다면 dist\ 폴더 안에 함께 복사해 주세요.
echo.

pause
endlocal
