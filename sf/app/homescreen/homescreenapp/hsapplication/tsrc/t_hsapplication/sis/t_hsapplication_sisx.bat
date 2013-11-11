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

if exist t_hsapplication.sisx del t_hsapplication.sisx

makesis t_hsapplication.pkg
signsis t_hsapplication.sis t_hsapplication.sisx ../../../../internal/sis/rd.cer ../../../../internal/sis/rd-key.pem

if exist t_hsapplication.sisx (
echo t_hsapplication.sisx creation SUCCEEDED
del t_hsapplication.sis
)

if not exist t_hsapplication.sisx (
echo t_hsapplication.sisx creation FAILED
)