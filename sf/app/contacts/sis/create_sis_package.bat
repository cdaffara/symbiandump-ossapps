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

IF EXIST contacts.sisx (
    echo Deleting old contacts.sisx file.
    del contacts.sisx
)


CALL createsis create -cert ./certs/rd.cer -key ./certs/rd-key.pem contacts.pkg

RENAME contacts.sis contacts.sisx
echo Created new contacts.sisx file.
@echo done.