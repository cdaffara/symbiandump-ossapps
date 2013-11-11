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
rem Description: mms_z.batrem

copy z:\TestFramework\TestFramework_MMSClientMTMBCTester.ini c:\TestFramework\TestFramework.ini
copy z:\TestFramework\MMSClientMTMBCTester.ini c:\TestFramework\MMSClientMTMBCTester.ini 
copy z:\TestFramework\MMSClientMTMBCTester_TestImage.jpg c:\TestFramework\MMSClientMTMBCTester_TestImage.jpg

md e:\bctest
md e:\bctest\results

ATSINTERFACE.EXE -testmodule MMSClientMTMBCTester  -moduleini c:\TestFramework\MMSClientMTMBCTester.ini 

copy c:\Logs\TestFramework\TestReport.txt e:\bctest\results\Messaging_MMSClientMTM.txt
