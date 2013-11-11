@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off
REM Builds IMCM test harnesses for WINS UDEB
REM Runs all automated tests which do not need any sort of connection or user interaction
REM More functionality to be added to this script file e.g. make it into proper batch file 
REM depending on arguments etc.- KP.

call test.bat
call cd \commdb\group\
call bldmake bldfiles
call abld test build wins udeb t_db
call /epoc32/release/wins/udeb/t_db.exe
call cd \msg\imcm\

REM Build WINS version of each of the test harnesses
call abld test build wins udeb

REM Run each of the test harnesses
call /epoc32/release/wins/udeb/t_miut01.exe
REM call /epoc32/release/wins/urel/t_miut01.exe
call /epoc32/release/wins/udeb/t_miut02.exe
REM call /epoc32/release/wins/urel/t_miut02.exe
call /epoc32/release/wins/udeb/t_miut03.exe
REM call /epoc32/release/wins/urel/t_miut03.exe
call /epoc32/release/wins/udeb/t_miut04.exe
REM call /epoc32/release/wins/urel/t_miut04.exe
call /epoc32/release/wins/udeb/t_miut05.exe
REM call /epoc32/release/wins/urel/t_miut05.exe
call /epoc32/release/wins/udeb/t_miut06.exe
REM call /epoc32/release/wins/urel/t_miut06.exe
call /epoc32/release/wins/udeb/t_miut07.exe
REM call /epoc32/release/wins/urel/t_miut07.exe
call /epoc32/release/wins/udeb/t_miut09.exe
REM call /epoc32/release/wins/urel/t_miut09.exe
call /epoc32/release/wins/udeb/t_miut10.exe
REM call /epoc32/release/wins/urel/t_miut10.exe
call /epoc32/release/wins/udeb/t_smtsnd.exe
REM call /epoc32/release/wins/urel/t_smtsnd.exe
call /epoc32/release/wins/udeb/t_impc.exe
REM call /epoc32/release/wins/urel/t_impc.exe
call /epoc32/release/wins/udeb/t_imcm01.exe
REM call /epoc32/release/wins/urel/t_imcm01.exe
call /epoc32/release/wins/udeb/t_imcm02a.exe
REM call /epoc32/release/wins/urel/t_imcm02a.exe
call /epoc32/release/wins/udeb/t_imcm02b.exe
REM call /epoc32/release/wins/urel/t_imcm02b.exe
call /epoc32/release/wins/udeb/t_imcm02c.exe
REM call /epoc32/release/wins/urel/t_imcm02c.exe
call /epoc32/release/wins/udeb/t_imcm02d.exe
REM call /epoc32/release/wins/urel/t_imcm02d.exe
call /epoc32/release/wins/udeb/t_imcm02e.exe
REM call /epoc32/release/wins/urel/t_imcm02e.exe
call /epoc32/release/wins/udeb/t_imcm02_heap.exe
REM call /epoc32/release/wins/urel/t_imcm02_heap.exe
call /epoc32/release/wins/udeb/t_imcm03.exe
REM call /epoc32/release/wins/urel/t_imcm03.exe
call /epoc32/release/wins/udeb/t_imcm03_heap.exe
REM call /epoc32/release/wins/urel/t_imcm03_heap.exe
call /epoc32/release/wins/udeb/t_imcm05.exe
REM call /epoc32/release/wins/urel/t_imcm05.exe
call /epoc32/release/wins/udeb/t_imcm06.exe
REM call /epoc32/release/wins/urel/t_imcm06.exe

REM Tests which require connection
call /epoc32/release/wins/udeb/t_smtc.exe
REM call /epoc32/release/wins/urel/t_smtc.exe
call /epoc32/release/wins/udeb/t_popc.exe
REM call /epoc32/release/wins/urel/t_popc.exe
call /epoc32/release/wins/udeb/t_impc01.exe
REM call /epoc32/release/wins/urel/t_impc01.exe
call /epoc32/release/wins/udeb/t_imcm07.exe
REM call /epoc32/release/wins/urel/t_imcm07.exe
call /epoc32/release/wins/udeb/t_imcm07_imap.exe
REM call /epoc32/release/wins/urel/t_imcm07_imap.exe
