/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <logwrap.hrh>
#include <QDateTime>

#include "logseventparser.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "logsengdefs.h"
#include "LogsApiConsts.h"     //Additional event UIDs
#include "logslogger.h"


// ----------------------------------------------------------------------------
// LogsEventParser::parseL
// ----------------------------------------------------------------------------
//
void LogsEventParser::parseL( 
    const CLogEvent& source, 
    LogsEvent& dest,
    const LogsEventStrings& /*strings*/ )
{
    dest.mNumber = DESC_TO_QSTRING( source.Number() );
}

// ----------------------------------------------------------------------------
// LogsEventParser::resolveEventType
// ----------------------------------------------------------------------------
//
bool LogsEventParser::resolveEventType(LogsEvent& /*dest*/)
{
    return false;
}

// End of file

