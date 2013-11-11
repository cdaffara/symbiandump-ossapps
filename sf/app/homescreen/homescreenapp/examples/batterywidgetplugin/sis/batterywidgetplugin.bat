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

if exist batterywidgetplugin.sisx del batterywidgetplugin.sisx

makesis batterywidgetplugin.pkg
signsis batterywidgetplugin.sis batterywidgetplugin.sisx ../../../internal/sis/rd.cer ../../../internal/sis/rd-key.pem

if exist batterywidgetplugin.sisx (
echo batterywidgetplugin.sisx creation SUCCEEDED
del batterywidgetplugin.sis
)

if not exist batterywidgetplugin.sisx (
echo batterywidgetplugin.sisx creation FAILED
)