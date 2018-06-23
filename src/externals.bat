
setlocal
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do (
  set InstallDir=%%i
)

if exist "%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe" (
  set MSBUILD_PATH="%InstallDir%\MSBuild\15.0\Bin\MSBuild.exe"
) ELSE (
  GOTO End
)

premake5 --file=external/command_all.lua "clone"
premake5 --file=external/command_all.lua "build"

:End
endlocal
