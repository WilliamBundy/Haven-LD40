@echo off

set baseName=Haven
set filePrefix=ld40

set compiler=cl
set msvcdir="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\"
if not defined DevEnvDir call %msvcdir%vcvars64.bat >nul

set ctime=usr\bin\ctime.exe
set lineify=usr\bin\lineify.exe
set wirmpht=usr\bin\wirmpht.exe

%ctime% -begin usr\bin\%baseName%.ctm

if not exist bin mkdir bin
copy usr\lib\*.dll bin\ >nul 2>&1

rem Kill task if it's running
taskkill /IM %baseName%.exe 1>NUL 2>&1

rem if exist %wirmpht% %wirmpht% -p -s -t src\%filePrefix%Main.c > src\%filePrefix%Generated.h

set disabled=/wd4477 ^
	/wd4244 ^
	/wd4267 ^
	/wd4334 ^
	/wd4305 ^
	/wd4101 ^
	/D_CRT_SECURE_NO_WARNINGS

if "%~1"=="release" goto ReleaseBuild
if "%~1"=="wpl" goto WplBuild

%compiler%  /nologo ^
	/TC ^
	/Gd ^
	/Zi ^
	/c ^
	/EHsc ^
	/W3 ^
	/fp:fast ^
	%disabled% ^
	/Iusr\include\ ^
	src\wpl\wpl.c ^
	/Fewpl.obj ^
	/Fdwpl.pdb

lib /NOLOGO /SUBSYSTEM:WINDOWS /LIBPATH:usr/lib wpl.obj SDL2.lib
move *.pdb bin\ >NUL 2>&1
del *.obj >NUL 2>&1

%compiler%  /nologo ^
	/TC ^
	/Gd ^
	/Zi ^
	/EHsc ^
	/W3 ^
	/fp:fast ^
	%disabled% ^
	/Iusr\include\ ^
	src\%filePrefix%Main.c ^
	/Febin\%baseName%.exe ^
	/Fdbin\%baseName%.pdb ^
/link ^
	/nologo ^
	/LIBPATH:"usr\lib" ^
	kernel32.lib user32.lib Shlwapi.lib wpl.lib ^
	/SUBSYSTEM:CONSOLE ^
	/INCREMENTAL:NO

goto EndBuild

:ReleaseBuild
echo Build Start
%compiler%  /nologo ^
	/TC ^
	/Gd ^
	/O2 ^
	/EHsc ^
	/W3 ^
	/fp:fast ^
	%disabled% ^
	/Iusr\include\ ^
	src\%filePrefix%Main.c ^
	/Febin\%baseName%.exe ^
	/Fdbin\%baseName%.pdb ^
/link ^
	/nologo ^
	/LIBPATH:"usr\lib" ^
	kernel32.lib user32.lib wpl.lib ^
	/SUBSYSTEM:CONSOLE ^
	/INCREMENTAL:NO
goto EndBuild

:EndBuild
set LastError=%ERRORLEVEL%
del *.obj >nul 2>&1
	
echo]
echo Build Complete 
%ctime% -end usr\bin\%baseName%.ctm LastError

if "%~1"=="run" goto RUN
if "%~2"=="run" goto RUN
goto Quit

:Run
if not errorlevel 1 start bin\%baseName%.exe 

:Quit
