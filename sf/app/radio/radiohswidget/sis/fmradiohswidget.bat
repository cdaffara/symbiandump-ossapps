@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

if exist fmradiohswidget.sisx del fmradiohswidget.sisx

makesis fmradiohswidget.pkg
signsis fmradiohswidget.sis fmradiohswidget.sisx ../../internal/sis/nokia_rndcert_02.der ../../internal/sis/nokia_rndcert_02.key

if exist fmradiohswidget.sisx (
echo fmradiohswidget.sisx creation SUCCEEDED
del fmradiohswidget.sis
)

if not exist fmradiohswidget.sisx (
echo fmradiohswidget.sisx creation FAILED
)