
C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\\`버전-번호`\\`툴체인`\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\\`폴더`\\SolTranslator.exe

- translations qt_*.qm 파일 제거
  - sol_*.qm 파일에 이미 포함되어져 있기 때문에 제거해도 괜찮습니다.


### Qt 6.9.0 / MSVC 2022 / Debug

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\msvc2022_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-debug_vs_69\SolTranslator.exe

windeployqt --pdb F:\Dev\FinPoint\SolTranslator\cmake-build-debug_vs_69\SolTranslator.exe


### Qt 6.9.0 / MSVC 2022 / Release

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\msvc2022_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-release_vs_69\SolTranslator.exe

windeployqt --release --compiler-runtime F:\Dev\FinPoint\SolTranslator\.sol.release/SolTranslator_Lastest/SolTranslator.exe


### Qt 6.9.0 / MinGW / Debug

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\mingw_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-debug-mingw\SolTranslator.exe

windeployqt --debug F:\Dev\FinPoint\SolTranslator\cmake-build-debug-mingw\SolTranslator.exe
->.dll 파일 위치에 .debug 파일을 같이 위치 시켜야합니다.


### Qt 6.9.0 / MinGW / Release

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\mingw_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-release-mingw\SolTranslator.exe


### Qt 6.9.0 / LLVM/Clang / Debug

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\msvc2022_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-debug-llvm-clang\SolTranslator.exe

windeployqt --pdb F:\Dev\FinPoint\SolTranslator\cmake-build-debug-llvm-clang\SolTranslator.exe



### Qt 6.9.0 / LLVM/Clang / Release

C:\Windows\System32\cmd.exe /A /Q /K C:\Qt\6.9.0\msvc2022_64\bin\qtenv2.bat

windeployqt F:\Dev\FinPoint\SolTranslator\cmake-build-release-llvm-clang\SolTranslator.exe
windeployqt F:\Dev\FinPoint\SolTransla`tor\.sol.release\SolTranslator_Lastest\SolTranslator.exe