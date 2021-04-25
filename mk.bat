::编译指令 chengj 20170413
::mk [project] or mk

@echo off
cls

:START
if not "1%~1%"=="1" goto check1
::无项目名参数
:select_project
ECHO.
ECHO *******************************************************
ECHO 0. Log (error message)
ECHO 1. ET310
ECHO 2. ET320
ECHO 3. GT300
ECHO 4. GT300S
ECHO 5. GT370
ECHO 6. GT740
ECHO 7. GB100
ECHO 8. MT200
ECHO 9. NT31
ECHO 10. NT33
ECHO 11. NT36
ECHO 12. ET350
ECHO 13. GT310
ECHO 14. GT380
ECHO 15. NT37
ECHO 16. NT22
ECHO 17. NT55
ECHO 18. JM81
ECHO 19. HVT001
ECHO 20. JM66
ECHO 21. GT420D
ECHO *******************************************************
set /p userInput=选择项目序号(Q退出): 
if /i "%userInput%"=="0" goto select_project_error_log
if /i "%userInput%"=="1" goto select_project_ET310
if /i "%userInput%"=="2" goto select_project_ET320
if /i "%userInput%"=="3" goto select_project_GT300
if /i "%userInput%"=="4" goto select_project_GT300S
if /i "%userInput%"=="5" goto select_project_GT370
if /i "%userInput%"=="6" goto select_project_GT740
if /i "%userInput%"=="7" goto select_project_GB100
if /i "%userInput%"=="8" goto select_project_MT200
if /i "%userInput%"=="9" goto select_project_NT31
if /i "%userInput%"=="10" goto select_project_NT33
if /i "%userInput%"=="11" goto select_project_NT36
if /i "%userInput%"=="12" goto select_project_ET350
if /i "%userInput%"=="13" goto select_project_GT310
if /i "%userInput%"=="14" goto select_project_GT380
if /i "%userInput%"=="15" goto select_project_NT37
if /i "%userInput%"=="16" goto select_project_NT22
if /i "%userInput%"=="17" goto select_project_NT55
if /i "%userInput%"=="18" goto select_project_JM81
if /i "%userInput%"=="19" goto select_project_HVT001
if /i "%userInput%"=="20" goto select_project_JM66
if /i "%userInput%"=="21" goto select_project_GT420D
if /i "%userInput%"=="Q" goto END
echo 你究竟要编译哪个项目?
goto :select_project

:check1
if "1%~2%"=="1" goto check2
echo 只能输入一个项目名参数 
goto :select_project

:check2
::echo %~dp0
::判断输入的项目名
if /i "%1%"=="LOG" goto select_project_error_log
if /i "%1%"=="ET310" goto select_project_ET310
if /i "%1%"=="ET320" goto select_project_ET320
if /i "%1%"=="GT300" goto select_project_GT300
if /i "%1%"=="GT300S" goto select_project_GT300S
if /i "%1%"=="GT370" goto select_project_GT370
if /i "%1%"=="GT740" goto select_project_GT740
if /i "%1%"=="GB100" goto select_project_GB100
if /i "%1%"=="MT200" goto select_project_MT200
if /i "%1%"=="NT31" goto select_project_NT31
if /i "%1%"=="NT33" goto select_project_NT33
if /i "%1%"=="NT36" goto select_project_NT36
if /i "%1%"=="ET350" goto select_project_ET350
if /i "%1%"=="GT310" goto select_project_GT310
if /i "%1%"=="GT380" goto select_project_GT380
if /i "%1%"=="NT37" goto select_project_NT37
if /i "%1%"=="NT22" goto select_project_NT22
if /i "%1%"=="NT55" goto select_project_NT55
if /i "%1%"=="JM81" goto select_project_JM81
if /i "%1%"=="HVT001" goto select_project_HVT001
if /i "%1%"=="JM66" goto select_project_JM66
if /i "%1%"=="GT411" goto select_project_GT420D
echo 项目名不存在，请重新选择
goto select_project

:select_project_error_log
	set mtk_project=GXQ03D_M2M_11C
	IF EXIST .\build\%mtk_project%\err.log DEL .\build\%mtk_project%\err.log /q
	findstr /S Error: .\build\%mtk_project%\bootloader.log >>.\build\%mtk_project%\err.log
	findstr /S Error: .\build\%mtk_project%\bootloader_ext.log >>.\build\%mtk_project%\err.log
	findstr /S Error: .\build\%mtk_project%\log\*.log >>.\build\%mtk_project%\err.log
	findstr /S Error: .\build\%mtk_project%\MT6261.log >>.\build\%mtk_project%\err.log
	ECHO.  >> .\build\%mtk_project%\err.log
	.\build\%mtk_project%\err.log
	goto end

::根据项目配置参数
:select_project_ET310
	set project=ET310
	set chip=MT2503D
	goto CompileCopy

:select_project_ET320
	set project=ET320
	set chip=MT2503D
	goto CompileCopy

:select_project_GT300
	set project=GT300
	set chip=MT2503D
	goto CompileCopy

:select_project_GT300S
	set project=GT300S
	set chip=MT2503D
	goto CompileCopy

:select_project_GT370
	set project=GT370
	set chip=MT2503D
	goto CompileCopy

:select_project_GT740
	set project=GT740
	set chip=MT2503D
	goto CompileCopy

:select_project_GB100
	set project=GB100
::please set MT2503A or MT2503D
	set chip=MT2503D
	goto CompileCopy

:select_project_MT200
	set project=MT200
	set chip=MT2503D
	goto CompileCopy

:select_project_NT31
	set project=NT31
	set chip=MT2503D
	goto CompileCopy

:select_project_NT33
	set project=NT33
	set chip=MT2503D
	goto CompileCopy

:select_project_NT36
	set project=NT36
	set chip=MT2503D
	goto CompileCopy

:select_project_ET350
	set project=ET350
	set chip=MT2503D
	goto CompileCopy
	
:select_project_GT310
	set project=GT310
	set chip=MT2503D
	goto CompileCopy

:select_project_GT380
	set project=GT380
	set chip=MT2503D
	goto CompileCopy

:select_project_NT37
	set project=NT37
	set chip=MT2503D
	goto CompileCopy
	
:select_project_NT22
	set project=NT22
	set chip=MT2503D
	goto CompileCopy

:select_project_NT55
	set project=NT55
	set chip=MT2503D
	goto CompileCopy

:select_project_JM81
	set project=JM81
	set chip=MT2503D
	goto CompileCopy
	
:select_project_HVT001
	set project=HVT001
	set chip=MT2503D
	goto CompileCopy

:select_project_JM66
	set project=JM66
	set chip=MT2503D
	goto CompileCopy
:select_project_GT420D
	set project=GT420D
	set chip=MT2503D
	goto CompileCopy
:CompileCopy

::1、复制项目mak文件
set fileSource=%~dp0make\GXQ03D_M2M_11C_GPRS_%project%.mak
echo 复制%fileSource%
set fileTarget=%~dp0make\GXQ03D_M2M_11C_GPRS.mak
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%project%.mak文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::2、复制ell_fota_memory_define.h
set fileSource=%~dp0track\mtk_sal\%chip%\ell_fota_memory_define.h
set fileTarget=%~dp0ELL\ell-common\ell_fota_memory_define.h
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\ell_fota_memory_define.h文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::3、复制AutoGenFotaScat.pl
set fileSource=%~dp0track\mtk_sal\%chip%\AutoGenFotaScat.pl
set fileTarget=%~dp0tools\AutoGenFotaScat.pl
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\AutoGenFotaScat.pl文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::4、复制custom_MemoryDevice.h
set fileSource=%~dp0track\mtk_sal\%chip%\custom_MemoryDevice.h
set fileTarget=%~dp0custom\system\GXQ03D_M2M_11C_BB\custom_MemoryDevice.h
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\custom_MemoryDevice.h文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::5、复制FOTA_Tracker.pl
set fileSource=%~dp0track\mtk_sal\%chip%\FOTA_Tracker.pl
set fileTarget=%~dp0tools\FOTA_Tracker.pl
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\FOTA_Tracker.pl文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::6、复制MergeSections.pl
set fileSource=%~dp0track\mtk_sal\%chip%\MergeSections.pl
set fileTarget=%~dp0tools\MergeSections.pl
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\MergeSections.pl文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::7、复制scatGenLib.pl
set fileSource=%~dp0track\mtk_sal\%chip%\scatGenLib.pl
set fileTarget=%~dp0tools\scatGenLib.pl
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\scatGenLib.pl文件 
) else (
echo %fileSource% 不存在
goto paramError
)

::8、复制emigenMD.pl
set fileSource=%~dp0track\mtk_sal\%chip%\emigenMD.pl
set fileTarget=%~dp0tools\emigenMD.pl
if exist %fileSource% (
if exist %fileTarget% del %fileTarget% 
copy %fileSource%  %fileTarget% 
echo 复制%chip%\emigenMD.pl文件 
) else (
echo %fileSource% 不存在
goto paramError
)


::编译new+fota
ping -n 2 127.0.0.1>nul
::复制项目mak文件后new并且fota生成软件包
CALL new.bat
ping -n 2 127.0.0.1>nul
CALL fota.bat newall
goto END

:paramError
echo 输入参数错误无法执行 
goto END

:END
ping -n 1 127.0.0.1>nul

echo "mk >> %project% (%chip%) 执行结束"

