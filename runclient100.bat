@echo off
for /l %%i in (1,1,50) do (
    start "" .\debug\client.exe normal_%%i %%i
    ping -n 1 -w 500 127.0.0.1 >nul
)
