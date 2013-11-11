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

if exist homescreenapp_ut_applib.sisx del homescreenapp_ut_applib.sisx

makesis homescreenapp_ut_applib.pkg
signsis homescreenapp_ut_applib.sis homescreenapp_ut_applib.sisx ../internal/sis/rd.cer ../internal/sis/rd-key.pem

if exist homescreenapp_ut_applib.sisx (
echo homescreenapp_ut_applib.sisx creation SUCCEEDED
del homescreenapp_ut_applib.sis
)

if not exist homescreenapp_ut_applib.sisx (
echo homescreenapp_ut_applib.sisx creation FAILED
)
