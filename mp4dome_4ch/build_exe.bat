@echo off
chcp 65001 > nul
setlocal

set "PYTHON_EXE="

if exist "%~dp0..\.conda\python.exe" (
    set "PYTHON_EXE=%~dp0..\.conda\python.exe"
)

if not defined PYTHON_EXE (
    py -3 -c "import sys" >nul 2>nul
    if not errorlevel 1 (
        set "PYTHON_EXE=py -3"
    )
)

if not defined PYTHON_EXE (
    python -c "import sys" >nul 2>nul
    if not errorlevel 1 (
        set "PYTHON_EXE=python"
    )
)

echo ============================================
echo  MP 4Dome Controller - EXE 빌드 스크립트
echo ============================================
echo.

if not defined PYTHON_EXE (
    echo [!] Python 실행 파일을 찾을 수 없습니다.
    echo [!] Python 또는 py launcher 설치 상태를 확인해 주세요.
    pause
    exit /b 1
)

echo [*] 사용할 Python: %PYTHON_EXE%
echo.

:: ---- PyInstaller 설치 확인 ----
%PYTHON_EXE% -c "import PyInstaller" 2>nul
if errorlevel 1 (
    echo [*] PyInstaller 가 없습니다. 설치합니다...
    %PYTHON_EXE% -m pip install pyinstaller
    if errorlevel 1 (
        echo [!] PyInstaller 설치 실패. Python/pip 환경을 확인해 주세요.
        pause
        exit /b 1
    )
)

:: ---- pyserial / tweepy 설치 확인 ----
%PYTHON_EXE% -c "import serial" 2>nul
if errorlevel 1 (
    echo [*] pyserial 설치 중...
    %PYTHON_EXE% -m pip install pyserial
)
%PYTHON_EXE% -c "import tweepy" 2>nul
if errorlevel 1 (
    echo [*] tweepy 설치 중...
    %PYTHON_EXE% -m pip install tweepy
)

echo.
echo [*] 빌드를 시작합니다...
echo.

:: ---- 이전 빌드 산출물 정리 ----
if exist "build" (
    echo [*] 이전 build 폴더를 삭제합니다...
    rmdir /s /q "build"
)

if exist "dist" (
    echo [*] 이전 dist 폴더를 삭제합니다...
    rmdir /s /q "dist"
)

echo [*] PyInstaller clean 빌드를 수행합니다...
echo.

:: ---- 아이콘 파일 유무에 따라 빌드 명령 분기 ----
if exist "app.ico" (
    %PYTHON_EXE% -m PyInstaller --clean --noconfirm MP4DomeController.spec
) else (
    echo [주의] app.ico 가 없습니다. 아이콘 없이 빌드합니다.
    %PYTHON_EXE% -m PyInstaller --clean --noconfirm MP4DomeController.spec
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
