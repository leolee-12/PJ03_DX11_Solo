

xcopy /옵션 .복사할파일이있는경로 .복사받을경로

xcopy /y .\Engine\Bin\Engine.lib	.\EngineSDK\lib\
xcopy /y .\Engine\Bin\Engine.dll	.\Client\bin\
xcopy /y .\Engine\Public\*.h		.\EngineSDK\Inc\


xcopy /y .\Renderer\Public\*.h		.\RendererSDK\Inc\
xcopy /y .\Renderer\Bin\Renderer.lib	.\RendererSDK\lib\
xcopy /y .\Renderer\Bin\Renderer.dll	.\Client\bin\
xcopy /y .\Renderer\Bin\ShaderFiles\*.* .\Client\bin\ShaderFiles\