@echo off
setlocal

:: %0 - Name of the batch script, including its path
:: ~  - Used to remove the remaining quotes
:: d  - Extracts the drive letter from the path Eg C from C:\
:: p  - Extracts the directory from the path
set "TargetDirectory=%~dp0"

echo Cleaning project files and build directories at %TargetDirectory%
for /r %TargetDirectory% %%f in (*.sln *.vcxproj *.csproj *.vbproj *.user *.suo *.vcxproj.filters cmake_install.cmake CMakeCache.txt) do (
    if exist "%%f" (
        del "%%f" 2>nul
        echo Deleted: %%f
    )
)

:: /s - Remove all directories and files in the target directory
:: /q - Enables quite mode so it doesn't ask you to type Y/N
for /d /r "%TargetDirectory%" %%d in (Build out .idea .vs .vscode CMakeFiles x64 *.dir) do (
    if exist "%%d" (
        rd /s /q "%%d"
        echo Deleted: %%d
    )
)

echo Cleaning complete
endlocal