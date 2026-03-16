// 명령어	옵션		원본 파일의 위치			사본 파일을 저장할 위치
xcopy		/y /i /s	.\Engine\Public\*.h		.\EngineSDK\Inc\
xcopy		/y /i /s	.\Engine\Bin\Engine.lib		.\EngineSDK\lib\
xcopy		/y /i /s	.\Engine\Bin\*.dll			.\Client\Bin\
xcopy		/y /i /s	.\Engine\Bin\*.dll			.\Editor\Bin\
xcopy		/y /i /s	.\Game_PKM\Bin\ShaderFiles\*.* 	.\Client\Bin\ShaderFiles\

// 외부 라이브러리 h, lib 파일 복사
xcopy		/y /i /s	.\ThirdParty\inc\*			.\EngineSDK\Inc\
xcopy		/y /i	.\ThirdParty\lib\*.lib		.\EngineSDK\Lib\