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

if not exist \epoc32\wins\c\logs\email\nul md \epoc32\wins\c\logs\email\
if not exist \epoc32\wins\c\logs\msgtest\nul md \epoc32\wins\c\logs\msgtest\

if not exist \epoc32\wins\c\mailtest\nul md \epoc32\wins\c\mailtest
if not exist \epoc32\wins\c\mailtest\rfc822\nul md \epoc32\wins\c\mailtest\rfc822
copy \msg\impstsrc\rfc_*.txt \epoc32\wins\c\mailtest\rfc822\*.* >nul
copy \msg\impstsrc\config.txt \epoc32\wins\c\mailtest\rfc822\config.txt