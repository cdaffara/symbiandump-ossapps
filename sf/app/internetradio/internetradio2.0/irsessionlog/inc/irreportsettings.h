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
* Description:  The implementation for presentation elements.
*
*/


#ifndef IRREPORTSETTINGS_H
#define IRREPORTSETTINGS_H

#include <e32def.h>

_LIT( KSession, "Session" );

_LIT( KNmsLog, "NmsLog" );

_LIT( KLogFileName, "RadioReport.xml" );

_LIT( KGZipLogFileName, "LogUsage.gz" );

_LIT( KField, "Field" );

_LIT( KXmlHeader, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );


//RadioReport

_LIT(KRPSTARTTAG,"<radioReport ver=\"Field\"  IRID=\"Field\" id=\"Field\">");

_LIT( KRPENDTAG, "</radioReport>" );

//Session log

_LIT( KSESSIONSTARTTAG, "<session starttime=\"Field\" duration=\"Field\" terminatedBy=\"Field\" connectedFrom=\"Field\" id=\"Field\"><channelid>Field</channelid>" );

_LIT( KSERVERRESULTTAG, "<serverResult url=\"Field\">Field</serverResult>" );

_LIT( KCONNECTIONTAG, "<connection type=\"Field\"><operator currentNetwork=\"Field\" homeOperator=\"Field\"/></connection>" );

_LIT( KSESSIONENDTAG, "</session>" );

//Browse log

_LIT( KBROWSESTARTTAG, "<browseLog>" );

_LIT( KBROWSEENDTAG, "</browseLog>" );

_LIT( KBROWSETAG, "<browse url=\"Field\" timeStamp=\"Field\" currentNetwork=\"Field\"/>" );


/**
 * for nmsLogs ( Nokia Music Store Logs Information )
 * 
 */
_LIT( KNMSLOGSTARTTAG, "<nmsLog>" );
_LIT( KNMSLOGENDTAG, "</nmsLog>" );
_LIT(KNMSEVENTTAG, "<nmsEvent timeStamp=\"Field\" channelId=\"Field\" type=\"Field\"><operator currentNetwork=\"Field\" homeOperator=\"Field\"/></nmsEvent>" );

/**
 * for song recognition Logs (Shazam Logs Information)
 * 
 */
_LIT(KSONGRECOGLOGSTARTTAG, "<songRecogLog>");
_LIT(KSONGRECOGLOGENDTAG, "</songRecogLog>");
_LIT(KSONGRECOGEVENTTAG, "<songRecogEvent timeStamp=\"Field\"></songRecogEvent>");


//Advertisement log

//Terminatedby
_LIT( KUSER, "user" );

_LIT( KNOCONNECTIONTOSERVER, "noConnectionToServer" );

_LIT( KNOCONNECTIONTONETWORK, "noConnectionToNetwork" );

//Connected from

_LIT( KISDS,"isds" );

_LIT( KPRESETADHOC,"preset-adhoc" );
_LIT( KPRESETISDS,"preset-isds" );

_LIT( KHISTORYADHOC,"history-adhoc" );
_LIT( KHISTORYISDS,"history-isds" );
_LIT( KADHOCMANUAL,"adhoc-manual" );
_LIT( KADHOCEXTERNAL,"adhoc-external" );

//Connection type

//ServerResult
_LIT( KConnected, "Connected" );

_LIT( KFull, "ServerFull" );

_LIT( KConnTimeOut, "ConnectionTimeOut" );

_LIT( KConnectionFailed, "ConnectionFailed" );

#endif //IRREPORTSETTINGS_H
