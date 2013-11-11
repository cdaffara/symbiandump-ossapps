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
#include "logseventdataparser.h"
#include "logseventdata.h"
#include <e32std.h>
#include <logeng.h>
#include <logsmspdudata.h>
#include "LogsApiConsts.h"


// Separator for gprs data (old legacy non-tagged format for sent and
// received grps data)
_LIT8(KDataSeparator,",");  

// ----------------------------------------------------------------------------
// LogsEventDataParser::parse
// ----------------------------------------------------------------------------
//
int LogsEventDataParser::parse( 
    const CLogEvent& source, LogsEventData& dest ) 
{  
        // For SMS'es part data is packed into data field (by sms stack), so in
    // this case there is no  need to try to parse S60 specific data from it.
    bool readMessageParts = ( 
        source.EventType() == KLogShortMessageEventTypeUid ||
        source.EventType() == KLogsEngMmsEventTypeUid );
    const TDesC8& data = source.Data();
    
    //All default values must already be set in case there is no data in the
    //event's data field.
    dest.mIsCNAP = false;
    dest.mIsVT = false;
    dest.mIsVoIP = false;
    dest.mIsPoC = false;
    dest.mIsEmerg = false;
    dest.mMsgPartsNumber = 0; //Meaningful only for sms
    dest.mServiceId = 0;
    dest.mContactLocalId = 0;
    dest.mDataSent = 0;
    dest.mDataReceived = 0;
    dest.mRemoteUrl.clear();
    dest.mLocalUrl.clear();
    
    // 1. Msg parts are written in different format in data field.
    if( readMessageParts ) {
        setMsgPartsNumber( data, dest );
    }
    // 2. else check if the sent & received data is in old format (this can be
    // removed when creating log data field entries is similar to format of
    // other data field entries (i.e is tagged))
    else if ( !checkNonTaggedData( data, dest ) ) {
        // 3. Otherwise check if tagged data is available.
        checkTaggedData( data, dest );
    }
    return 0;
}

// ----------------------------------------------------------------------------
// LogsEventDataParser::setMsgPartsNumber
// Read msg parts. They are written in format of TLogSmsPduData in Data field
// ----------------------------------------------------------------------------
//
void LogsEventDataParser::setMsgPartsNumber( const TDesC8 &data, LogsEventData& dest ) 
{
    TPckgBuf<TLogSmsPduData> packedData;
    packedData.Copy( data.Ptr(), sizeof( TLogSmsPduData ) );
    dest.mMsgPartsNumber = packedData().iTotal;
}

// ----------------------------------------------------------------------------
// LogsEventDataParser::checkNonTaggedData
// ----------------------------------------------------------------------------
//
bool LogsEventDataParser::checkNonTaggedData( const TDesC8 &data, LogsEventData& dest ) 
{
    if( data.Length() < 1 ) {
        return false;
    }   

    //At least 1 byte of data available. Check does it begin with number.
    TInt v;
    TPtrC8 ptr( data.Left(1) );
    TLex8 lex = ptr;
    if( lex.Val( v ) != KErrNone ) {
        //First byte does not contain number
        return false;
    }

    //Ok, data begins with number. Try to read a pair of comma separated numbers
    TInt separatorOffset = data.Find( KDataSeparator );
    if( separatorOffset + 1 > data.Length() || separatorOffset < 0 ) {
        //No separator found.
        return false;   
    }

    ptr.Set( data.Left( separatorOffset ) );
    lex = ptr;
    lex.Val( dest.mDataSent );

    if( (separatorOffset + 1) < data.Length() ){
        // Parse rest
        ptr.Set( data.Mid(separatorOffset + 1, data.Length() - separatorOffset - 1) );
        lex = ptr;
        lex.Val( dest.mDataReceived );
    }

    return true;
}

// ----------------------------------------------------------------------------
// LogsEventDataParser::checkTaggedData
// ----------------------------------------------------------------------------
//
void LogsEventDataParser::checkTaggedData( const TDesC8 &data, LogsEventData& dest ) 
{  
    TPtrC8 dataPtr( data );
    TInt dataLeft = dataPtr.Length();
    while( dataLeft > 0 ){
        TInt nextTokenStart = dataPtr.Find( KLogsDataFldNameDelimiter() );
        if ( nextTokenStart > dataPtr.Length() || nextTokenStart < 0 ) {
            nextTokenStart = dataPtr.Length();
        }

        TPtrC8 nameValue = dataPtr.Left( nextTokenStart );
        TPtrC8 name;
        TPtrC8 value;

        TInt delimiterStart = nameValue.Find( KLogsDataFldValueDelimiter() );   
        if( delimiterStart > dataPtr.Length() || delimiterStart < 0 ){
            name.Set( nameValue );
            //No value. Initialised to null in above (TPtrC8 value)
        } else {
            name.Set( dataPtr.Left( delimiterStart ) );  
            TInt length = nameValue.Length() - delimiterStart - 1;
            value.Set( dataPtr.Mid( delimiterStart + 1, length) );
        }
        
        // Below a minor attempt to slightly speed up the string comparisons:
        // If value already found, no need to compare same name anymore.
        // Most likely there is VT, VOIP or POC tag in the beginning of data
        // field if any tags.
        if( !dest.mIsVT && name.Compare( KLogsDataFldTag_VT ) == 0 ){
            dest.mIsVT = true;
        } else if( !dest.mIsCNAP  && name.Compare( KLogsDataFldTag_CNAP ) == 0 ){
            dest.mIsCNAP = true;
        } else if( !dest.mIsEmerg && name.Compare( KLogsDataFldTag_Emergency ) == 0 ){
            dest.mIsEmerg = true;
        } else if( !dest.mIsPoC && name.Compare( KLogsDataFldTag_POC ) == 0 ){
            dest.mIsPoC = true;
        } else if( !dest.mIsVoIP && name.Compare( KLogsDataFldTag_IP ) == 0 ){
            dest.mIsVoIP = true;
        } else if( !dest.mServiceId && name.Compare( KLogsDataFldTag_ServiceId ) == 0 ) {
            TLex8 lex( value );
            TUint32 temp( 0 );
            TInt err = lex.Val( temp , EDecimal );
            if ( KErrNone == err ) {
                //if an error occurred we leave the service id unchanged
                dest.mServiceId = temp;
            }
        } else if ( !dest.mContactLocalId && 
                    name.Compare( KLogsDataFldTag_ContactLink ) == 0 ) {            
            HBufC8* contactLink = value.AllocLC();
            TLex8 lex( contactLink->Des() );
            TUint32 temp( 0 );
            TInt err = lex.Val( temp , EDecimal );
            if ( KErrNone == err ) {
                 dest.mContactLocalId = temp;
            }
            CleanupStack::PopAndDestroy(contactLink);
            
        } else if ( dest.mRemoteUrl.isEmpty() && name.Compare( KLogsDataFldTag_URL ) == 0 ) {
            // VoIP strips unneeded stuff from uri before writing the log
            // event to db (it removes '<' '>', "sip:", and everything after ';', 
            // see svpsslogcall.cpp) so no need to postprocess.
            dest.mRemoteUrl = QString::fromUtf8( (char*)value.Ptr(), value.Length() );
        } else if ( dest.mLocalUrl.isEmpty() && name.Compare( KLogsDataFldTag_MA ) == 0 ) {
            dest.mLocalUrl = QString::fromUtf8( (char*)value.Ptr(), value.Length() );
        }
        
        //Process remaining data
        dataLeft = dataPtr.Length() - nextTokenStart - 1; 
        if ( dataLeft > 0 ) {
            //Continue with remaining data on the right side of token
            nameValue.Set( dataPtr.Right( dataLeft ) );
            dataPtr.Set( nameValue );               
        }
    }    
}
