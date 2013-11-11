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
call :RUN ut_phonebubblewrapper
call :RUN ut_phonecommandextensionwrapper
call :RUN ut_phonemessagecontroller
call :RUN ut_phonenotecontroller
call :RUN ut_phoneresourceadapter
call :RUN ut_phoneuicommandcontroller
call :RUN ut_phoneuiqtbuttonscontroller
call :RUN ut_phoneuiqtviewadapter
call :RUN ut_phonevisibilityhandler
call :RUN ut_telephonyservice

:RUN
del \epoc32\winscw\c\data\%1.log
\epoc32\release\winscw\udeb\%1.exe -o c:\data\%1.log
type \epoc32\winscw\c\data\%1.log

