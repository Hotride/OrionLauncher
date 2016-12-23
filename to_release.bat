chcp 1251>nul

set exePath="D:\CPP\build-OrionLauncher-Desktop_Qt_5_6_1_MinGW_32bit-Release\release\OrionLauncher.exe"
set gccPath="C:\Qt\5.6\mingw49_32\bin"
set targetPath="%cd%\bin"

chcp 866>nul

C:
cd %gccPath%
windeployqt -dir %targetPath% %exePath%
copy %exePath% /Y /B %targetPath%\OrionLauncher.exe
pause
