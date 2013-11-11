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

echo OFF

del \epoc32\winscw\c\private\EEE39B3F\ut_phoneservices.log
\epoc32\release\winscw\udeb\ut_phoneservices.exe -o ut_phoneservices.log
type \epoc32\winscw\c\private\EEE39B3F\ut_phoneservices.log

del \epoc32\winscw\c\private\E31F4ECE\ut_dialservice.log
\epoc32\release\winscw\udeb\ut_dialservice.exe -o ut_dialservice.log
type \epoc32\winscw\c\private\E31F4ECE\ut_dialservice.log

del \epoc32\winscw\c\private\EEE39B2F\ut_dtmfservice.log
\epoc32\release\winscw\udeb\ut_dtmfservice.exe -o ut_dtmfservice.log
type \epoc32\winscw\c\private\EEE39B2F\ut_dtmfservice.log

del \epoc32\winscw\c\private\E31F4ECF\ut_ctiurischemeparser.log
\epoc32\release\winscw\udeb\ut_ctiurischemeparser.exe -o ut_ctiurischemeparser.log
type \epoc32\winscw\c\private\E31F4ECF\ut_ctiurischemeparser.log

del \epoc32\winscw\c\private\E31F4EAF\ut_telurischemeparser.log
\epoc32\release\winscw\udeb\ut_telurischemeparser.exe -o ut_telurischemeparser.log
type \epoc32\winscw\c\private\E31F4EAF\ut_telurischemeparser.log

del \epoc32\winscw\c\private\E31F4EDA\ut_urischemehandlerservice.log
\epoc32\release\winscw\udeb\ut_urischemehandlerservice.exe -o ut_urischemehandlerservice.log
type \epoc32\winscw\c\private\E31F4EDA\ut_urischemehandlerservice.log
