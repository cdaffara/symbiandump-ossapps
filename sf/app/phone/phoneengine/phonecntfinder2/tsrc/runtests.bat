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

echo OFF

del \epoc32\winscw\c\private\e84eae66\u_testmatch2.log
\epoc32\release\winscw\udeb\testmatch2.exe -o u_testmatch2.log
type \epoc32\winscw\c\private\e84eae66\u_testmatch2.log

del \epoc32\winscw\c\private\e39991fe\u_cphcntcontactidimpl2.log
\epoc32\release\winscw\udeb\testcphcntcontactidimpl2.exe -o u_cphcntcontactidimpl2.log
type \epoc32\winscw\c\private\e39991fe\u_cphcntcontactidimpl2.log

del \epoc32\winscw\c\private\e38991fe\u_testfactoryimpl2.log
\epoc32\release\winscw\udeb\testfactoryimpl2.exe -o u_testfactoryimpl2.log
type \epoc32\winscw\c\private\e38991fe\u_testfactoryimpl2.log

del \epoc32\winscw\c\private\e84eae56\u_testmatcherimpl2.log
\epoc32\release\winscw\udeb\testmatcherimpl2.exe -o u_testmatcherimpl2.log
type \epoc32\winscw\c\private\e84eae56\u_testmatcherimpl2.log


