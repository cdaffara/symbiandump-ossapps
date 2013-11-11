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

set MAIN_SIS_DIR=%CD%

if exist ftu_unittest.sisx del ftu_unittest.sisx

makesis ftu_unittest.pkg
signsis ftu_unittest.sis ftu_unittest.sisx rd.cer rd-key.pem

if exist ftu_unittest.sisx (
echo  ftu_unittest.sisx creation SUCCEEDED
del ftu_unittest.sis
)

if not exist ftu_unittest.sisx (
echo  ftu_unittest.sisx creation FAILED
)
