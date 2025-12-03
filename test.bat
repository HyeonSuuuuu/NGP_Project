@echo off
title 서버 및 클라이언트 실행기

:MENU
cls
echo ============================
echo   서버 / 클라이언트 실행기
echo ============================
echo.
echo   1. 서버 실행
echo   2. 클라이언트 실행
echo   0. 종료
echo.
set /p choice=원하는 번호를 입력하세요: 

if "%choice%"=="1" goto RUN_SERVER
if "%choice%"=="2" goto RUN_CLIENT
if "%choice%"=="0" exit
goto MENU


:RUN_SERVER
echo 서버를 시작합니다...
start "ServerWindow" "Server\x64\Release\Server.exe"
echo 서버 실행 완료.
pause
goto MENU


:RUN_CLIENT

echo 클라이언트 1개를 시작합니다...
start "Homework4Client_2" "Client\x64\Release\3DGP.exe"
echo 클라이언트가 시작되었습니다.
pause
goto MENU