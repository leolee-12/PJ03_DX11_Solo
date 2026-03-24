@echo off

:: 1. Engine 출력물 삭제
del /f /q "EngineSDK\Inc\*.h" 2>nul
del /f /q "EngineSDK\lib\Engine.lib" 2>nul

:: 2. Game 출력물 삭제
rd /s /q "GameSDK" 2>nul
rd /s /q "ShaderFiles" 2>nul

:: 작업 완료 후 바로 종료
exit