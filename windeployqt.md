
C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\\`버전-번호`\\`툴체인`\bin\qtenv2.bat

windeployqt `프로젝트_경로`\\SolTranslator.exe

- translations qt_*.qm 파일 제거
  - sol_*.qm 파일에 이미 포함되어져 있기 때문에 제거할 수 있습니다.


### Qt 6.9.0 / MSVC 2022 / Debug

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\msvc2022_64\bin\qtenv2.bat

windeployqt --pdb F:\Dev\FinPoint\SolTranslator\cmake-build-debug_vs_69\SolTranslator.exe


### Qt 6.9.0 / MinGW / Debug

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\mingw_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-debug-mingw\SolTranslator.exe

->.dll 파일 위치에 동명의 .debug 파일을 복사해야 디버그할 수 있습니다.(수동)


### Qt 6.9.0 / LLVM/Clang / Debug

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\msvc2022_64\bin\qtenv2.bat

windeployqt --pdb F:\Dev\FinPoint\SolTranslator\cmake-build-debug-llvm-clang\SolTranslator.exe

