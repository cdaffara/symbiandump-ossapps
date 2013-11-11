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
#include <logeng.h>

#include "logseventdataparser.h"
#include "logseventdata.h"
#include "LogsApiConsts.h"

// Separator for gprs data (old legacy non-tagged format for sent and
// received grps data)
//_LIT8(KDataSeparator,",");  

// ----------------------------------------------------------------------------
// LogsEventDataParser::parse
// ----------------------------------------------------------------------------
//
int LogsEventDataParser::parse( 
    const CLogEvent& /*source*/, LogsEventData& dest ) 
{
    dest.mIsVT = true;
    return 0;
}

// ----------------------------------------------------------------------------
// LogsEventDataParser::setMsgPartsNumber
// Read msg parts. They are written in format of TLogSmsPduData in Data field
// ----------------------------------------------------------------------------
//
void LogsEventDataParser::setMsgPartsNumber( const TDesC8 &/*data*/, LogsEventData& /*dest*/ ) 
{
}

// ----------------------------------------------------------------------------
// LogsEventDataParser::checkNonTaggedData
// ----------------------------------------------------------------------------
//
bool LogsEventDataParser::checkNonTaggedData( const TDesC8 &/*data*/, LogsEventData& /*dest*/ ) 
{
return true;
}


// ----------------------------------------------------------------------------
// LogsEventDataParser::checkTaggedData
// ----------------------------------------------------------------------------
//
void LogsEventDataParser::checkTaggedData( const TDesC8 &/*data*/, LogsEventData& /*dest*/ ) 
{
}
