@echo off
for /l %%i in (1,1,10) do (
    start "" .\debug\client.exe normal_%%i %%i
)
