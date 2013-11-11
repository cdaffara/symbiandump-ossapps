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

if exist contentpublishclient.sisx del contentpublishclient.sisx

makesis contentpublishclient.pkg
signsis contentpublishclient.sis contentpublishclient.sisx ../../../internal/sis/rd.cer ../../../internal/sis/rd-key.pem

if exist contentpublishclient.sisx (
echo contentpublishclient.sisx creation SUCCEEDED
del contentpublishclient.sis
)

if not exist contentpublishclient.sisx (
echo contentpublishclient.sisx creation FAILED
)