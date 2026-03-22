// 명령어	옵션		원본 파일의 위치					사본 파일을 저장할 위치

// Engine
xcopy		/y /i /s	.\Engine\Public\*.h				.\EngineSDK\Inc\
xcopy		/y /i /s	.\Engine\Bin\Engine.lib			.\EngineSDK\lib\
xcopy		/y /i /s	.\Engine\Bin\*.dll				.\Client\Bin\
xcopy		/y /i /s	.\Engine\Bin\*.dll				.\Editor\Bin\

// Game
xcopy		/y /i /s	.\Game_PKM\Public\*.h			.\GameSDK\Inc\
xcopy		/y /i /s	.\Game_PKM\Bin\*.lib			.\GameSDK\lib\
xcopy		/y /i /s	.\Game_PKM\Bin\*.pdb			.\GameSDK\lib\
xcopy		/y /i /s	.\Game_PKM\ShaderFiles\*.* 		.\ShaderFiles\
xcopy		/y /i /s	.\Game_PKM\ShaderFiles\*.* 		.\ShaderFiles\

// ThirdParties
xcopy		/y /D /i /s	.\ThirdParty\inc\*				.\EngineSDK\Inc\
xcopy		/y /D /i /s	.\ThirdParty\lib\*.lib				.\EngineSDK\Lib\
xcopy		/y /D /i /s	.\ThirdParty\lib\*.dll				.\Client\Bin\
xcopy		/y /D /i /s	.\ThirdParty\lib\*.dll				.\Editor\Bin\
xcopy		/y /D /i /s	.\ThirdParty\lib\*.pdb				.\Client\Bin\
xcopy		/y /D /i /s	.\ThirdParty\lib\*.pdb				.\Editor\Bin\