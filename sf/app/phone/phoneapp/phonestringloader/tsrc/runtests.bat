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

del \epoc32\winscw\c\private\EF78FF4E\ut_phonestringloader.log
\epoc32\release\winscw\udeb\ut_phonestringloader.exe -o ut_phonestringloader.log
type \epoc32\winscw\c\private\EF78FF4E\ut_phonestringloader.log
