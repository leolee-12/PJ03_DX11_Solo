// 명령어	옵션		원본 파일의 위치			사본 파일을 저장할 위치

xcopy		/y /i		.\Engine\Public\*.h			.\EngineSDK\Inc\
xcopy		/y /i		.\Engine\Bin\Engine.lib		.\EngineSDK\lib\
xcopy		/y /i		.\Engine\Bin\*.dll			.\Client\Bin\
xcopy		/y /i		.\Engine\Bin\*.dll			.\Tool\Bin\