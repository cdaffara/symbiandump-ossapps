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
@echo off

if "%1"=="winscw" (
call runtest.cmd ut_bubbleutils
call runtest.cmd ut_bubbleheader
call runtest.cmd ut_bubbleconferenceheader
call runtest.cmd ut_bubblepartlistmodel
call runtest.cmd ut_bubblecontainerwidget
call runtest.cmd ut_bubbleheadingwidget
call runtest.cmd ut_bubbleimagewidget
call runtest.cmd ut_bubbleparticipantlistitem
call runtest.cmd ut_bubblebutton
rem call runtest.cmd mt_bubblemanager2
)

@echo off
if "%1"=="win32" (
call runtest_w32.cmd ut_bubbleutils
call runtest_w32.cmd ut_bubbleheader
call runtest_w32.cmd ut_bubbleconferenceheader
call runtest_w32.cmd ut_bubblepartlistmodel
call runtest_w32.cmd ut_bubblecontainerwidget
call runtest_w32.cmd ut_bubbleheadingwidget
call runtest_w32.cmd ut_bubbleimagewidget
call runtest_w32.cmd ut_bubbleparticipantlistitem
call runtest_w32.cmd ut_bubblebutton
rem call runtest_w32.cmd mt_bubblemanager2
)

@echo off
if "%2"=="ctc" (
call ctcpost^
 ut_bubbleutils\mon.sym ut_bubbleutils\mon.dat^
 ut_bubbleheader\mon.sym ut_bubbleheader\mon.dat^
 ut_bubbleconferenceheader\mon.sym ut_bubbleconferenceheader\mon.dat^
 ut_bubblepartlistmodel\mon.sym ut_bubblepartlistmodel\mon.dat^
 ut_bubblecontainerwidget\mon.sym ut_bubblecontainerwidget\mon.dat^
 ut_bubbleheadingwidget\mon.sym ut_bubbleheadingwidget\mon.dat^
 ut_bubbleimagewidget\mon.sym ut_bubbleimagewidget\mon.dat^
 ut_bubbleparticipantlistitem\mon.sym ut_bubbleparticipantlistitem\mon.dat^
 ut_bubbleanimiconitem\mon.sym ut_bubbleanimiconitem\mon.dat^
  -p profile.txt
call ctc2html -i profile.txt
)

@echo off
if "%1"=="" echo Usage: 'runall winscw' or 'runall win32'

