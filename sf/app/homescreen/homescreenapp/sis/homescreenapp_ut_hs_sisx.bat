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

if exist homescreenapp_ut_hs.sisx del homescreenapp_ut_hs.sisx

makesis homescreenapp_ut_hs.pkg
signsis homescreenapp_ut_hs.sis homescreenapp_ut_hs.sisx ../internal/sis/rd.cer ../internal/sis/rd-key.pem

if exist homescreenapp_ut_hs.sisx (
echo homescreenapp_ut_hs.sisx creation SUCCEEDED
del homescreenapp_ut_hs.sis
)

if not exist homescreenapp_ut_hs.sisx (
echo homescreenapp_ut_hs.sisx creation FAILED
)
