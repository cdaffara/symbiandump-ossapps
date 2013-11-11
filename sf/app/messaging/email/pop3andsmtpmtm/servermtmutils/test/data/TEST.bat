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
REM list the different test builds for this component
set _testbuildlist=T_IMSK01 T_IMSK07 T_IMCV00  T_IMCV00 T_IMCV01

REM list the different test executables for this component
set _testrunlist=T_IMSK01 T_IMSK07 T_IMCV00  T_IMCV00 T_IMCV01

copy \epoc32\release\wins\deb\z\system\libs\imcm.dll \epoc32\release\wins\deb\imcm.dll 

copy \msg\imuttsrc\Entry_RichTextBodies.txt \epoc32\wins\c\mailtest\imcv\Entry_RichTextBodies.txt
copy \msg\imuttsrc\Entry_822Headers.txt \epoc32\wins\c\mailtest\imcv\Entry_822Headers.txt
copy \msg\imuttsrc\Entry_MimeHeaders.txt \epoc32\wins\c\mailtest\imcv\Entry_MimeHeaders.txt
copy \msg\imuttsrc\Entry_Structure.txt \epoc32\wins\c\mailtest\imcv\Entry_Structure.txt
copy \msg\imuttsrc\Entry_Files.txt \epoc32\wins\c\mailtest\imcv\Entry_Files.txt
rem
copy \msg\imuttsrc\ImcvSendOutputText.txt \epoc32\wins\c\mailtest\imcv\ImcvSendOutputText.txt
copy \msg\imuttsrc\ImcvSendOutputText_MIME.txt  \epoc32\wins\c\mailtest\imcv\ImcvSendOutputText_MIME.txt
rem
rem
copy \msg\imuttsrc\rfc822\*.* \epoc32\wins\c\mailtest\rfc822\*.*
