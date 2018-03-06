rem +------------------------------------------------------------------------+
rem |    Author : M.W.Richardson                                             |
rem |    Date   : 03/12/2013                                                 |
rem +------------------------------------------------------------------------+
rem |                                                                        |
rem | 1. Create a job in Jenkins, (make sure the name has no space):         |
rem |                                                                        |
rem | 2. Add build step to invoke this batch file with argument %WORKSPACE%  |
rem |                                                                        |
rem | 3. Add post-build step to archive artifacts: "Report\*.htm"            |
rem |                                                                        |
rem | Note that flexlm requires that Jenkins is started manually             |
rem +------------------------------------------------------------------------+
rem |                                                                        |
rem |    Configure C/C++ LDRA Tool Suite for the MinGW compiler              |
rem |    Run Static Analysis                                                 |
rem |    Instrument and build                                                |
rem |    Run Dynamic Analysis and Dynamic Dataflow Analysis                  |
rem |    Publish a Report into the workspace                                 |
rem |                                                                        |
rem +------------------------------------------------------------------------+


setlocal

rem Configure paths that are likely to change
rem =========================================
set PRJ_NAME=LDRA_demo
set TCF_ROOT=C:\Users\nagabhushana.mallu\Desktop\LDRA_Demo\LDRA_Demo\main\Functionalandunittests

rem remove any existing SET_TEST.BAT file
rem =====================================
if exist SET_TEST.BAT (del /F SET_TEST.BAT)

rem Use TBiniflags to search for where the tool suite and workarea are installed
rem ============================================================================
call %TCF_ROOT%\TBiniflags.exe /L"C/C++" /FTBED /FWORKAREA_BASEDIR

rem Exit if TBiniflags has not run correctly
rem ========================================
if not exist SET_TEST.BAT exit
if %ERRORLEVEL% == 12 exit

rem Configure TBED and WORKAREA_BASEDIR
rem ===================================
call SET_TEST.BAT

rem Configure relative paths 
rem ========================
set HERE=%CD%
cd /d %TCF_ROOT%
cd ..
set CONFIG_DIR=%CD%\Configuration
set SRC_DIR=%CD%\Src
cd ..
set PRJ_ROOT=%CD%
set WORK_DIR=%WORKAREA_BASEDIR%%PRJ_NAME%_tbwrkfls
set LOG_FILE=%WORKAREA_BASEDIR%Testbed.log
set REPORT_DIR="%WORKSPACE%"\Report
if not exist %REPORT_DIR% mkdir %REPORT_DIR%
set SRC_FILES=%PRJ_ROOT%\%PRJ_NAME%.tcf
cd /d %HERE%

rem Delete the log file, so that it no longer contains any previous actions
rem =======================================================================
if exist %LOG_FILE% del /q %LOG_FILE%


rem Delete the work directory, otherwise we will end up with duplicated sequences
rem =============================================================================
%TBED%\contestbed /delete_set=%PRJ_NAME%
if exist %WORK_DIR% rmdir /s /q %WORK_DIR%


rem Set up necessary testbed.ini options
rem ====================================
%TBED%\TBini COMPILER_SELECTED="MinGW200 GCC C/C++ v3.2"
%TBED%\TBini METFILE=%CONFIG_DIR%\metpen.dat
%TBED%\TBini CTBENDFILE=%TBED%\C\ctbend.dat
%TBED%\TBini CPPTBENDFILE=%TBED%\CPP\cpptbend.dat
%TBED%\TBini REMOVE_DUPLICATE_INCLUDES=TRUE
%TBED%\TBini SHORTEN_IGNORE_SYS_INCLUDES=TRUE
%TBED%\TBini FILE_LIMIT=5
%TBED%\TBini CPENFILE="<Default>"
%TBED%\TBini CREPFILE="<Default>"
%TBED%\TBini CSTANDARDS_MODEL="ACME Standard"

rem Configure to use the ACME Standard
rem This uses one existing as well as one new Hungarian rule
rem ========================================================
%TBED%\contestbed %SRC_FILES% /CPENFILE=%CONFIG_DIR%\Cpen.dat /CREPFILE=%CONFIG_DIR%\Creport.dat /CSTANDARDS_MODEL="ACME Standard"
copy %CONFIG_DIR%\Userstandards_combined.exe %TBED%\Userstandards_combined.exe
%TBED%\TBini USER_STANDARDS_GENERATOR="%TBED%\Userstandards_combined.exe"

time /t

rem Run the Main Static, Complexity, Data Flow & Cross Reference
rem ============================================================
%TBED%\contestbed %SRC_FILES% /112a34q

time /t

rem Instrument and build
rem ====================
%TBED%\contestbed %SRC_FILES% /212q

time /t

rem Execute and run the Dynamic Analysis and Dynamic Data Flow Analysis
rem ===================================================================
%SRC_DIR%\LDRA_demo.exe < %TCF_ROOT%\Input_adding_products.txt
move /Y *.exh %SRC_DIR%
%TBED%\contestbed %SRC_FILES% /run_required_dyndflow /dataset=Input_adding_products

%SRC_DIR%\LDRA_demo.exe < %TCF_ROOT%\Input_cancelling_products.txt
move /Y *.exh %SRC_DIR%
%TBED%\contestbed %SRC_FILES% /run_required_dyndflow /dataset=Input_cancelling_products

%SRC_DIR%\LDRA_demo.exe < %TCF_ROOT%\Input_filling_the_basket.txt
move /Y *.exh %SRC_DIR%
%TBED%\contestbed %SRC_FILES% /run_required_dyndflow /dataset=Input_filling_the_basket

%SRC_DIR%\LDRA_demo.exe < %TCF_ROOT%\Input_ignoring_erroneous_commands.txt
move /Y *.exh %SRC_DIR%
%TBED%\contestbed %SRC_FILES% /run_required_dyndflow /dataset=Input_ignoring_erroneous_commands

time /t

rem Generate report using TBpublish into the Jenkins workspace
rem ==========================================================
if not exist %REPORT_DIR% mkdir %REPORT_DIR%
%TBED%\contestbed %SRC_FILES% /1q /publish_to_dir=%REPORT_DIR% /publish_rep_type=HTML

time /t

rem Finally clear up any superfluous files in the workspace
rem =======================================================
if exist "%WORKSPACE%"\SET_TEST.BAT (del /F "%WORKSPACE%"\SET_TEST.BAT)

endlocal
