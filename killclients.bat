@echo off

:: 실행 중인 client.exe 프로세스를 모두 종료
taskkill /IM client.exe /F

echo All client.exe processes have been terminated.
pause