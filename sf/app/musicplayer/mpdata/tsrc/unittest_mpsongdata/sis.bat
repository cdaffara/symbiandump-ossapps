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

del *.sisx
makesis UT_mpsongdata.pkg
signsis UT_mpsongdata.sis UT_mpsongdata.sisx %RNDCERTS_PATH%\Nokia_RnDCert_02.der %RNDCERTS_PATH%\Nokia_RnDCert_02.key
del     UT_mpsongdata.sis