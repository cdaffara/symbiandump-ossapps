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
rem Description: Command file for project emulatorlan 
rem

call configchange.pl --config ethernetWithCommDB --kernel EKA2 --target winscw --variant UDEB
call \epoc32\RELEASE\winscw\udeb\dbcreator.exe -dtextshell -Mconsole -- E
call \epoc32\RELEASE\winscw\udeb\dbcreator.exe -dtextshell -- f:c:\S60_32_default_snaps.xml
