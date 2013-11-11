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
echo First step is to edit /s60/misc/release/bldvariant/elaf/data/bldvariant.hrh
echo Enable/disable flags __MEETING_REQUEST_SUPPORT and __MEETING_REQUEST_ENABLER
pause

cd/
cd /s60/misc/release/bldvariant/elaf/group
call bldmake bldfiles
call abld build winscw udeb
cd/
cd /s60/osext/systemswextensions/group
call bldmake bldfiles
call abld build winscw udeb
cd/
cd /s60/app/organizer/meetingui/group
call bldmake bldfiles
call abld build winscw udeb
cd/
cd /s60/mw/pimservices/meetingrequestengines/group
call bldmake bldfiles
call abld build winscw udeb
cd/
cd /s60/mw/pimservices/calendarengines/CalenInterimUtils/tsrc/public/basic/Group

echo
echo
echo Remember to call - abld test build winscw udeb
echo eunit_dll (copies the dll to right directory)
echo To check code coverage run ct_en.cmd and compile CalenInterimUtils
pause

@echo on
