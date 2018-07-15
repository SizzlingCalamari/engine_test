
setlocal

where msbuild >nul 2>nul
if %ERRORLEVEL% == 0 (
  set MSBUILD_PATH="msbuild"
  GOTO Build
)

for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" (
  set MSBUILD_PATH="%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe"
) ELSE (
  GOTO End
)

:Build

premake5 --file=external/command_all.lua "clone"
premake5 --file=external/command_all.lua "build"

:End
endlocal
