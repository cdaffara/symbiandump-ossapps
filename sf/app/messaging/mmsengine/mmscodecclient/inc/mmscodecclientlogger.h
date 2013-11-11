/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*     Logging macros.
*
*/



#ifndef MMSCODECCLIENTLOGGER_H
#define MMSCODECCLIENTLOGGER_H

#ifdef _DEBUG
#include <flogger.h>

// MACROS
// Related to logging
// Currently not used
_LIT(KMmsCCLogFile,"MmsCodecClient.txt");
_LIT(KMmsCCLogFolder,"mmss");

#define LOG(AAA) RFileLogger::Write(KMmsCCLogFolder(),KMmsCCLogFile(),EFileLoggingModeAppend,AAA)

#else // no _DEBUG defined

#define LOG(AAA)

#endif // _DEBUG
#endif // MMSCODECCLIENTLOGGER_H
