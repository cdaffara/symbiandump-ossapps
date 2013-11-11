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

if exist t_hshomescreenstateplugin.sisx del t_hshomescreenstateplugin.sisx

makesis t_hshomescreenstateplugin.pkg
signsis t_hshomescreenstateplugin.sis t_hshomescreenstateplugin.sisx ../../../../../internal/sis/rd.cer ../../../../../internal/sis/rd-key.pem

if exist t_hshomescreenstateplugin.sisx (
echo t_hshomescreenstateplugin.sisx creation SUCCEEDED
del t_hshomescreenstateplugin.sis
)

if not exist t_hshomescreenstateplugin.sisx (
echo t_hshomescreenstateplugin.sisx creation FAILED
)