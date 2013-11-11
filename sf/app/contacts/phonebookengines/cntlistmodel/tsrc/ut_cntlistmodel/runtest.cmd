@echo off
rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem
@echo on

call del MON.sym
call del MON.dat
call del profile.txt

call qmake
call sbs reallyclean
call sbs -c winscw_udeb
call sbs -c winscw_udeb
call qmake
call ctcwrap -i d -C "EXCLUDE+moc_*.cpp" -C "EXCLUDE+ut_*.*" sbs -c winscw_udeb

call \epoc32\release\winscw\udeb\ut_cntlistmodel.exe -noprompt
call ctcpost MON.sym MON.dat -p profile.txt
call ctc2html -i profile.txt -nsb
