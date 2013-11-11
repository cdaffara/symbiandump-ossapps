/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Email interface implementation, translator utilities.
*
*/



#include "BasePlugin.h"
#include "baseplugincommonutils.h"
#include "CFSMailCommon.h"


static void TranslateMsgStoreAttendeeL(
    CMsgStorePropertyContainer& aSrc,
    TUint aIdx,
    MMROrganizer& aAttendee );

static void TranslateMsgStoreAttendeesL(
    CMsgStorePropertyContainer& aMessage,
    CBaseMrInfoObject& aMrInfo,
    const TDesC8& aProperty,
    MMRAttendee::TAttendeeRole aRole );


/**@ the info object's description field is mapped to the message body which can be
of arbitrary size; at the moment cap the max description size to avoid memory problems.*/
const TInt KMaxDescription = 512*1024;

/**
 *
 */
class TDayOfWeekFtor
    {
    public:
        virtual void operator () ( MRRecurrenceRule::TMRRecurrentDay aDay ) = 0;
    };

/**
 *
 */
class TWeekDayTranslator : public TDayOfWeekFtor
    {
    public:
        TWeekDayTranslator( RArray<MRRecurrenceRule::TMRRecurrentDay>& aDays )
            : iDays( aDays )
            {
            };

        virtual void operator () ( MRRecurrenceRule::TMRRecurrentDay aDay )
            {
            iDays.AppendL( aDay );
            };

    private:
        RArray<MRRecurrenceRule::TMRRecurrentDay>& iDays;
    };

/**
 *
 */
class TMonthDayTranslator : public TDayOfWeekFtor
    {
    public:
        TMonthDayTranslator( MRRecurrenceRule::TMRRecurrentDay& aDayOfWeek )
            : iDayOfWeek( aDayOfWeek )
            {
            };

        virtual void operator () ( MRRecurrenceRule::TMRRecurrentDay aDay )
            {
            /**@ the contract with IMS/Outlook is that there is a single day
            set in a monthly by day of week rule; asserts ?*/
            iDayOfWeek = aDay;
            };

    private:
        MRRecurrenceRule::TMRRecurrentDay& iDayOfWeek;
    };


/**
 *
 */
void CBasePlugin::TranslateMsgStorePropsL(
    const TFSMailMsgId& aMailBoxId,
    CMsgStorePropertyContainer& aMessage,
    CFSMailMessagePart& aFsMsg,
    const TFSMailDetails aDetails )

    {
    __LOG_ENTER( "TranslateMsgStorePropsL" )

    __LOG_WRITE_FORMAT1_INFO( "aDetails = 0x%X.", aDetails )

    TFSMailMsgId fsId( GetPluginId(), aMessage.ParentId() );
    aFsMsg.SetFolderId( fsId );
    
    TUint idx = 0;
    //subject.
    if ( ( EFSMsgDataEnvelope & aDetails || EFSMsgDataStructure & aDetails
        || EFSMsgDataSubject & aDetails )
        && aMessage.FindProperty( KMsgStorePropertySubject, idx ) )
        {
        const TDesC& subject = aMessage.PropertyValueDesL( idx );
        aFsMsg.SetSubject( subject );
        idx = 0;

        __LOG_WRITE_FORMAT1_INFO( "Subject: %S.", &subject );
        }

    //from.
    if ( ( EFSMsgDataEnvelope & aDetails|| EFSMsgDataStructure & aDetails
        || EFSMsgDataSender & aDetails )
        && aMessage.FindProperty( KMsgStorePropertyFrom, idx ) )
        {
        CFSMailAddress* address = FetchEmailAddressL( aMessage, idx );
        aFsMsg.SetSender( address );
        idx = 0;
        }

    //received.
    if ( ( EFSMsgDataEnvelope & aDetails || EFSMsgDataStructure & aDetails
        || EFSMsgDataDate & aDetails )
        && aMessage.FindProperty( KMsgStorePropertyReceivedAt, idx ) )
        {
        TTime time;
        aMessage.PropertyValueTimeL( idx, time );
        aFsMsg.SetDate( time );
        idx = 0;
        
        __LOG_WRITE8_FORMAT1_INFO( "Received: %ld.", time.Int64() );
        }
    
    //sent.
    if ( ( EFSMsgDataEnvelope & aDetails || EFSMsgDataStructure & aDetails
        || EFSMsgDataDate & aDetails )
        && aMessage.FindProperty( KMsgStorePropertySent, idx ) )
        {
        TTime time;
        aMessage.PropertyValueTimeL( idx, time );
        aFsMsg.SetDate( time );
        idx = 0;
        
        __LOG_WRITE8_FORMAT1_INFO( "Sent: %ld.", time.Int64() );
        }

    if ( EFSMsgDataEnvelope & aDetails || EFSMsgDataStructure & aDetails )
        {
        //to recipients.
        while ( aMessage.FindProperty( KMsgStorePropertyTo, idx, idx ) )
            {
            CFSMailAddress* address = FetchEmailAddressL( aMessage, idx );
            aFsMsg.AppendToRecipient( address );
            ++idx;
            }
        idx = 0;

        //cc recipients.
        while ( aMessage.FindProperty( KMsgStorePropertyCc, idx, idx ) )
            {
            CFSMailAddress* address = FetchEmailAddressL( aMessage, idx );
            aFsMsg.AppendCCRecipient( address );
            ++idx;
            }
        idx = 0;
    
        //bcc recipients.
        while ( aMessage.FindProperty( KMsgStorePropertyBcc, idx, idx ) )
            {
            CFSMailAddress* address = FetchEmailAddressL( aMessage, idx );
            aFsMsg.AppendBCCRecipient( address );
            ++idx;
            }
        idx = 0;
        
        //flags.
        if ( aMessage.FindProperty( KMsgStorePropertyFlags, idx ) )
            {
            TUint32 i = aMessage.PropertyValueUint32L( idx );
            aFsMsg.SetFlag( i );
            idx = 0;
            
            __LOG_WRITE_FORMAT1_INFO( "Flags: 0x%X.", i )
            }

        //content-size.
        if ( aMessage.FindProperty( KMsgStorePropertySize, idx ) )
            {
            TUint32 i = aMessage.PropertyValueUint32L( idx );
            aFsMsg.SetContentSize( i );
            idx = 0;

            __LOG_WRITE_FORMAT1_INFO( "Content-size: %d.", i )
            }

        //content-type.
        if ( aMessage.FindProperty( KMsgStorePropertyContentType, idx ) )
            {
            
            const TDesC& contentType = aMessage.PropertyValueDesL( idx );
            
            idx = 0;
            if ( aMessage.FindProperty( KMsgStorePropertyCharset, idx ) )
                {
                _LIT( KCharSemicolon, ";" );
                const TDesC& charSet = aMessage.PropertyValueDesL( idx );
                
                TInt maxLength = contentType.Length() + KFSMailContentTypeParamCharset().Length() +
                    KCharSemicolon().Length() + charSet.Length();
                
                HBufC* buf = HBufC::NewLC( maxLength );
                TPtr bufPtr( buf->Des() );
                
                bufPtr.Append( contentType );
                bufPtr.Append( KCharSemicolon );
                bufPtr.Append( KFSMailContentTypeParamCharset );
                bufPtr.Append( charSet );
                aFsMsg.SetContentType( bufPtr );
                __LOG_WRITE_FORMAT1_INFO( "Content-type: %S.", &bufPtr );
                CleanupStack::PopAndDestroy( buf );
                }
            else
                {
                aFsMsg.SetContentType( contentType );
                __LOG_WRITE_FORMAT1_INFO( "Content-type: %S.", &contentType );
                }
 
            idx = 0;
            }
        
        //content-id.
        if ( aMessage.FindProperty( KMsgStorePropertyContentId, idx ) )
            {
            const TDesC& contentId = aMessage.PropertyValueDesL( idx );
            aFsMsg.SetContentIDL( contentId );
            idx = 0;
            
            __LOG_WRITE_FORMAT1_INFO( "Content-id: %S.", &contentId );
            }
        
        //content-description.
        if ( aMessage.FindProperty( KMsgStorePropertyContentDescription, idx ) )
            {
            const TDesC& contentDescription = aMessage.PropertyValueDesL( idx );
            aFsMsg.SetContentDescription( contentDescription );
            idx = 0;
            
            __LOG_WRITE_FORMAT1_INFO(
                "Content-description: %S.", &contentDescription );
            }

        //content-class.
        if ( aMessage.FindProperty( KMsgStorePropertyContentClass, idx ) )
            {
            const TDesC& contentClass = aMessage.PropertyValueDesL( idx );
            aFsMsg.SetContentClass( contentClass );
            idx = 0;
            }

        //fetched content size.
        if ( aMessage.FindProperty( KMsgStorePropertyRetrievedSize, idx ) )
            {
            TUint32 retrievedSize = aMessage.PropertyValueUint32L( idx );
            aFsMsg.SetFetchedContentSize( retrievedSize );
            idx = 0;
            }

        //content-disposition.
        if ( aMessage.FindProperty( KMsgStorePropertyContentDisposition, idx ) )
            {
            const TDesC& contentDisposition = aMessage.PropertyValueDesL( idx );
            aFsMsg.SetContentDisposition( contentDisposition );
            idx = 0;
            
            __LOG_WRITE_FORMAT1_INFO(
                "Content-desposition: %S.", &contentDisposition );
            }

        //meeting request info.
        if ( aMessage.FindProperty( KMsgStorePropertyMeetingRequest, idx ) )
            {
            CMsgStorePropertyContainer* mrInfo
                = aMessage.PropertyValueContainerL( idx );
            CleanupStack::PushL( mrInfo );
            TranslateMsgStoreMrL( aMailBoxId, aMessage, *mrInfo, aFsMsg );
            CleanupStack::PopAndDestroy( mrInfo ); 
            idx = 0;
            }

        //read-only size
        if ( aMessage.FindProperty( KMsgStorePropertyReadOnlySize, idx ) )
            {
            TUint readOnlySize = aMessage.PropertyValueUint32L(idx);
            aFsMsg.SetReadOnlyPartSize(readOnlySize);
            idx = 0;
            }
        }
    
    __LOG_EXIT
    } //TranslateMsgStorePropsL.


/**
 * Common msgstore meeting request fields translation to Freestyle.
 *
 * @param aMessage
 * @param aFsMsg
 */
void CBasePlugin::TranslateMsgStoreMrL(
    const TFSMailMsgId& aMailBoxId,
    CMsgStorePropertyContainer& aMessage,
    CMsgStorePropertyContainer& aCalendar,
    CFSMailMessagePart& aFsMsg )
    
    {
    __LOG_ENTER( "TranslateMsgStoreMrL" )
    
    CBaseMrInfoObject* mmrInfo = CBaseMrInfoObject::NewL();
    CleanupStack::PushL( mmrInfo );

    TUint idx;
    
    //uid.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrGuid, idx ) )
        {
        const TDesC8& uid8 = aCalendar.PropertyValueDes8L( idx );

        HBufC* uid = HBufC::NewLC( uid8.Length() );
        //uid not utf8-encoded.
        uid->Des().Copy( uid8 );
        mmrInfo->SetUidL( uid );

        CleanupStack::Pop( uid );
        }
    
    //subject.
    idx = 0;
    if ( aMessage.FindProperty( KMsgStorePropertySubject, idx ) )
        {
        const TDesC& subject = aMessage.PropertyValueDesL( idx );
        mmrInfo->SetSubjectL( subject );

        __LOG_WRITE_FORMAT1_INFO( "MR Subject: %S.", &subject );
        }

    //description - the plain/text email body.
    __LOG_WRITE_INFO( "About to translate the description field." );
    idx = 0;
    TMsgStoreId msgId = aMessage.Id();
    
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    CMsgStoreMessage* theMessage = mailBox().FetchMessageL( msgId, KMsgStoreInvalidId );
    CleanupStack::PushL( theMessage );
    
    CMsgStoreMessagePart* plainText = GetBodyPartL( *theMessage );
    if ( NULL != plainText )
        {
        CleanupStack::PushL( plainText );

        TUint length = plainText->ContentLengthL();
        //the body can be of arbitrary size - taking precautions until resolved.
        length = length < KMaxDescription ? length : KMaxDescription;
        HBufC* description = HBufC::NewLC( length );
        
        RBuf8 buf8;
        CleanupClosePushL( buf8 );
        buf8.CreateL( length );
        
        plainText->FetchContentToBufferL( buf8 );
        
        //replace the cr with the lf per iCal spec otherwise the Symbian iCal importer would leave with -20;
        //see ANN0001 for more info.

        _LIT8( KCrLf, "\r\x00\n\x00" );
        _LIT8( KLf, "\n\x00" );
        const TUint crLfSize = KCrLf().Size();

        TInt idx = KErrNotFound;
        while ( KErrNotFound != ( idx = buf8.Find( KCrLf ) ) )
        	{
        	buf8.Replace( idx, crLfSize, KLf );
        	}
        
        TPtrC ptr( KNullDesC );
        ptr.Set( reinterpret_cast<const TUint16*>( buf8.Ptr() ), buf8.Length() / 2 );

        mmrInfo->SetDescriptionL( ptr );
        
        //CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( &buf8 );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( plainText );
        }
    CleanupStack::PopAndDestroy( theMessage );
    
    //creation date.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrCreationDate, idx ) )
        {
        TTime time;
        aCalendar.PropertyValueTimeL( idx, time );
        mmrInfo->SetCreationTimeInUtcL( time );
        }
        
    //start date.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrStartDate, idx ) )
        {
        TTime time;
        aCalendar.PropertyValueTimeL( idx, time );
        mmrInfo->SetStartTimeInUtcL( time );
        }

    //end date.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrEndDate, idx ) )
        {
        TTime time;
        aCalendar.PropertyValueTimeL( idx, time );
        mmrInfo->SetEndTimeInUtcL( time );
        }

    //location.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrLocation, idx ) )
        {
        const TDesC& location = aCalendar.PropertyValueDesL( idx );
        mmrInfo->SetLocationL( location );
        }

    //organizer / from.
    idx = 0;
    if ( aMessage.FindProperty( KMsgStorePropertyFrom, idx ) )
        {
        TranslateMsgStoreAttendeeL( aMessage, idx, mmrInfo->MROrganizerL() );
        }

    //attendees / to.
    TranslateMsgStoreAttendeesL(
        aMessage, *mmrInfo, KMsgStorePropertyTo, MMRAttendee::EMRAttendeeParticipant );

    //attendees / cc.
    TranslateMsgStoreAttendeesL(
        aMessage, *mmrInfo, KMsgStorePropertyCc, MMRAttendee::EMRAttendeeOptionalParticipant );

    //recurrence info.
    __LOG_WRITE_INFO( "About to start translating the recurrence rule." )
    
    CBaseMrRecurrenceRule* recurrenceRule = CBaseMrRecurrenceRule::NewL();
    CleanupStack::PushL( recurrenceRule );

    //start date.
    /**@ dupes the infoobject one ?*/
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrStartDate, idx ) )
        {
        TTime time;
        aCalendar.PropertyValueTimeL( idx, time );
        recurrenceRule->SetRecurrenceStartTimeL( time );
        }
    
    //end date.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrRecurEndDate, idx ) )
        {
        TTime time;
        aCalendar.PropertyValueTimeL( idx, time );
        recurrenceRule->SetRecurrenceUntilL( time );
        }

    //frequency.    
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrFrequency, idx ) )
        {
        TUint32 imsFreq = aCalendar.PropertyValueUint32L( idx );
        recurrenceRule->SetRecurrentInterval( imsFreq );
        }
        
    //priority.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrPriority, idx ) )
        {
        TUint32 priority = aCalendar.PropertyValueUint32L( idx );
        mmrInfo->SetPriorityL( priority );
        }
        
    /**@ privacy/sensitivity missing ? */
    /*idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrPrivate, idx ) )
        {
        TBool private = aCalendar.PropertyValueBoolL( idx );
        }*/

    //method.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrMethod, idx ) )
        {
        TUint32 method = aCalendar.PropertyValueUint32L( idx );
        mmrInfo->SetMethod( static_cast<MMRInfoObject::TMRMethod>( method ) );
        }

    //status.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrStatus, idx ) )
        {
        TUint32 status = aCalendar.PropertyValueUint32L( idx );
        mmrInfo->SetMRResponseL( static_cast<MMRInfoObject::TResponse>( status ) );
        }

    //alarm/reminder time.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrReminder, idx ) )
        {
        TBool hasReminder = aCalendar.PropertyValueBoolL( idx );
        if ( hasReminder )
            {
            idx = 0;
            if ( aCalendar.FindProperty( KMsgStorePropertyMrReminderTime, idx ) )
                {
                if ( Time::NullTTime() != mmrInfo->StartTimeInUtcL() )
                    {
                    TUint32 offsetMinutes;
                    offsetMinutes = aCalendar.PropertyValueUint32L( idx );
                
                    TTime time = mmrInfo->StartTimeInUtcL();
                    time -= TTimeIntervalMinutes( offsetMinutes );
                    mmrInfo->SetAlarmInUtcL( time );
                    }
                else
                    {
                    __LOG_WRITE_INFO(
                        "Could not find a start date/time so can't set the reminder time." )
                    }   
                }
            }
        }

    //recurrent count.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrRecurCount, idx ) )
        {
        TUint32 count = aCalendar.PropertyValueUint32L( idx );
        recurrenceRule->SetRecurrentCountL( count );
        }

    //recurrence rule type.
    idx = 0;
    MRRecurrenceRule::TMRRecurrenceType recurrenceType
        = MRRecurrenceRule::EMRRecurrenceInvalid;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrRecurType, idx ) )
        {
        TUint32 fsType = aCalendar.PropertyValueUint32L( idx );
        recurrenceType = static_cast<MRRecurrenceRule::TMRRecurrenceType>( fsType );
        recurrenceRule->SetRecurrenceTypeL( recurrenceType );
        }

    //monthly-by-day-of-week.
    if ( MRRecurrenceRule::EMRRecurrenceMonthly == recurrenceType )
        {
        MRRecurrenceRule::TRecurrentDaysofMonth dayOfMonth;
        
        //week number.
        idx = 0;
        if ( aCalendar.FindProperty( KMsgStorePropertyMrWeekNo, idx ) )
            {
            TUint32 weekNo = aCalendar.PropertyValueUint32L( idx );
            /**@ double check that when testing starts.*/
            dayOfMonth.iWeekOfMonth =
                static_cast<MRRecurrenceRule::TMRRecurrenceWeek>( weekNo );
            }

        //day of week mask.
        idx = 0 ;    
        if ( aCalendar.FindProperty( KMsgStorePropertyMrDayOfWeekMask, idx ) )
            {
            TUint32 dowMask = aCalendar.PropertyValueUint32L( idx );
            
            TMonthDayTranslator mdt( dayOfMonth.iDayOfWeek );
            TranslateMsgStoreDayOfWeek( dowMask, mdt );
            }
        
        /** IMS, Outlook support a single day, intellisync-specific.*/
        RArray<MRRecurrenceRule::TRecurrentDaysofMonth> days;
        CleanupClosePushL( days );
        
        days.AppendL( dayOfMonth );
        recurrenceRule->SetRecurrentDaysofMonthL( days );
        
        CleanupStack::PopAndDestroy( &days );
        
        }
    //monthly-by-monthday.
    if ( MRRecurrenceRule::EMRRecurrenceMonthlyByDay == recurrenceType )
        {
        //Day number.
        idx = 0;
        RArray<TInt> days;
        CleanupClosePushL( days );
        
        if ( aCalendar.FindProperty( KMsgStorePropertyMrMonthDay, idx ) )
            {
            TUint32 monthDayNo = aCalendar.PropertyValueUint32L( idx );
            days.AppendL(monthDayNo);
            /**@ double check that when testing starts.*/
            }
            
        recurrenceRule->SetRecurrentMonthDaysL( days );
        
        CleanupStack::PopAndDestroy( &days );

        }    
    //weekly.
    else if ( MRRecurrenceRule::EMRRecurrenceWeekly == recurrenceType )
        {
        //day of week mask.
        idx = 0 ;    
        if ( aCalendar.FindProperty( KMsgStorePropertyMrDayOfWeekMask, idx ) )
            {
            TUint32 dowMask = aCalendar.PropertyValueUint32L( idx );
            
            RArray<MRRecurrenceRule::TMRRecurrentDay> days;
            CleanupClosePushL( days );

            TWeekDayTranslator wdt( days );
            TranslateMsgStoreDayOfWeek( dowMask, wdt );
            recurrenceRule->SetRecurrentWeekDaysL( days );
        
            CleanupStack::PopAndDestroy( &days );
            }
        }
    //yearly.
    else if ( MRRecurrenceRule::EMRRecurrenceYearly == recurrenceType )
        {
        //week number.
        idx = 0;
        if ( aCalendar.FindProperty( KMsgStorePropertyMrWeekNo, idx ) )
            {
            /**@ needs redoing after the latest changes to the MMR ifaces
            by activesync.*/
            /*TUint32 weekNo = aCalendar.PropertyValueUint32L( idx );

            MRRecurrenceRule::TRecurrentDaysofMonth dayOfMonth;*/
            /**@ double check that when testing starts.*/
            /*dayOfMonth.iWeekOfMonth =
                static_cast<MRRecurrenceRule::TMRRecurrenceWeek>( weekNo );
    
            //add to mmr info object.
            RArray<TInt> days;
            CleanupClosePushL( days );
            
            days.AppendL( dayOfMonth );
            recurrenceRule->SetRecurrentMonthDaysL( days );
            
            CleanupStack::PopAndDestroy( &days );*/
            }

        //day of week mask.
        idx = 0;
        if ( aCalendar.FindProperty( KMsgStorePropertyMrDayOfWeekMask, idx ) )
            {
            TUint32 dowMask = aCalendar.PropertyValueUint32L( idx );
            
            RArray<MRRecurrenceRule::TMRRecurrentDay> days;
            CleanupClosePushL( days );

            TWeekDayTranslator wdt( days );
            TranslateMsgStoreDayOfWeek( dowMask, wdt );
            recurrenceRule->SetRecurrentWeekDaysL( days );
        
            CleanupStack::PopAndDestroy( &days );
            }
        }

    mmrInfo->SetRecurrenceRuleL( *recurrenceRule );
    CleanupStack::Pop( recurrenceRule );
    
    //recurrence id.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrRRID, idx ) )
        {
        TTime time;
        aCalendar.PropertyValueTimeL( idx, time );
        mmrInfo->SetMRRecurrenceIdL( time );
        }
    
    //sequence number.
    idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrSeqNo, idx ) )
        {
        TUint32 seqNo = aCalendar.PropertyValueUint32L( idx );
        mmrInfo->SetMRSequenceNumber( static_cast<TInt>( seqNo ) );
        }

    aFsMsg.SetMRInfo( mmrInfo );
    CleanupStack::Pop( mmrInfo );
    
    __LOG_EXIT
    } //TranslateMsgStoreMrL.


/**
 *
 */
void CBasePlugin::TranslateEmailFwMessageL(
    CFSMailMessagePart& aSrc,
    CMsgStoreMessagePart& aDst,
    TBool aInInbox )

    {
    __LOG_ENTER( "TranslateEmailFwMessageL" )
    
    //subject.
    TDesC& value = aSrc.GetSubject();
    if ( NULL != &value && KNullDesC() != value )
        {
        aDst.AddOrUpdatePropertyL( KMsgStorePropertySubject, value );
        }

    RMsgStoreAddress msgStoreAddress;
    CleanupClosePushL( msgStoreAddress );                                       //+msgStoreAddress

    //from.
    CFSMailAddress* address = aSrc.GetSender();
    if ( NULL != address )
        {
        CBasePlugin::RemoveAllPropertiesL( aDst, KMsgStorePropertyFrom );
        
        if ( address->GetEmailAddress().Length() )
            {
            msgStoreAddress.iEmailAddress.Create( address->GetEmailAddress() );            
            }
        
        if ( address->GetDisplayName().Length() )
            {
            msgStoreAddress.iDisplayName.Create( address->GetDisplayName() );
            }

        aDst.AddOrUpdatePropertyL( KMsgStorePropertyFrom, msgStoreAddress );
        msgStoreAddress.Close();
        }

    CleanupStack::PopAndDestroy( &msgStoreAddress );                            //-msgStoreAddress

    //received or sent, depending on the folder.
    if ( aInInbox )
    	{
	    if ( aSrc.GetDate() != 0 )
	    	{
	        aDst.AddOrUpdatePropertyL( KMsgStorePropertyReceivedAt, aSrc.GetDate() );
	    	}
    	}
    else
    	{
        //set the sent stamp.
	    TTime sentTime;
	    sentTime.UniversalTime();
	    aDst.AddOrUpdatePropertyL( KMsgStorePropertySent, sentTime );
	    // same date needs to be set as received date , because of sorting
	    // feature in message store, which enable sorting only according to
	    // received date.
	    aDst.AddOrUpdatePropertyL( KMsgStorePropertyReceivedAt, sentTime );
	    aSrc.SetDate( sentTime );
    	}
    
    //to recipients.
//<qmail> Return by val (as in earlier Cmail version)
    RPointerArray<CFSMailAddress> toRecipients = aSrc.GetToRecipients();
    TranslateEmailFwRecipientsL( aDst, KMsgStorePropertyTo, toRecipients );

    //cc recipients.
    RPointerArray<CFSMailAddress> ccRecipients = aSrc.GetCCRecipients();
    TranslateEmailFwRecipientsL( aDst, KMsgStorePropertyCc, ccRecipients );

    //bcc recipients.
    RPointerArray<CFSMailAddress> bccRecipients = aSrc.GetBCCRecipients();
    TranslateEmailFwRecipientsL( aDst, KMsgStorePropertyBcc, bccRecipients );
//</qmail>

    //flags.
    //the cast is important otherwise the TBool overload gets called.
    TUint32 flag = static_cast<TUint32>( aSrc.GetFlags() );
    aDst.AddOrUpdatePropertyL(
        KMsgStorePropertyFlags, flag );

    //content type.
    const TPtrC& contentType = aSrc.GetContentType();
    if ( 0 < contentType.Length() )
        {
        aDst.AddOrUpdatePropertyL( KMsgStorePropertyContentType, contentType );
        }

    //content id.
    const TPtrC& contentId = aSrc.ContentID();
    if ( 0 < contentId.Length() )
        {
        aDst.AddOrUpdatePropertyL( KMsgStorePropertyContentId, contentId );
        }

    //content description.
    const TPtrC& contentDescription = aSrc.ContentDescription();
    if ( 0 < contentDescription.Length() )
        {
        aDst.AddOrUpdatePropertyL(
            KMsgStorePropertyContentDescription, contentDescription );
        }

    //content disposition.
    const TPtrC& contentDisposition = aSrc.ContentDisposition();
    if ( 0 < contentDisposition.Length() )
        {
        aDst.AddOrUpdatePropertyL(
            KMsgStorePropertyContentDisposition, contentDisposition );
        }

    //content class.
    const TDesC& contentClass = aSrc.GetContentClass();
    if ( 0 < contentClass.Length() )
        {
        aDst.AddOrUpdatePropertyL(
            KMsgStorePropertyContentClass, contentClass );
        }

    //attachment name.
    const TDesC& attachmentName = aSrc.AttachmentNameL();
    if ( 0 < attachmentName.Length() )
        {
        aDst.AddOrUpdatePropertyL( KMsgStorePropertyName, attachmentName );
        }

    //meeting request info.
    if ( aSrc.IsMRInfoSet() )
		{
		MMRInfoObject& mmrInfo = aSrc.GetMRInfo(); //no ownership transfer.
		TranslateEmailFwMrL( mmrInfo, aDst );
		}
	else
		{
	    //in the case of a MR origination the MRInfo object is not used thus is never set but still
		//the calendar property container is present and necessary.
		TUint index = 0;
		if ( !( flag & EFSMsgFlag_CalendarMsg )
			&& aDst.FindProperty( KMsgStorePropertyMeetingRequest, index ) )
			{
			aDst.RemovePropertyL( index );
			}
		}

    //read-only size
    if (aSrc.ReadOnlyPartSize() > 0)    
        {
        aDst.AddOrUpdatePropertyL(KMsgStorePropertyReadOnlySize, (TUint32)aSrc.ReadOnlyPartSize());
        }

    __LOG_EXIT
    } //TranslateEmailFwMessageL.


/**
 *
 */
void CBasePlugin::TranslateEmailFwMrL(
    MMRInfoObject& aSrc,
    CMsgStorePropertyContainer& aDst )

    {
    __LOG_ENTER( "TranslateEmailFwMrL" )
    
    TUint index;
    CMsgStorePropertyContainer* calendar = aDst.FindProperty(
        KMsgStorePropertyMeetingRequest, index ) ?
            aDst.PropertyValueContainerL( index )
            : CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( calendar );

    //response code.
    calendar->AddOrUpdatePropertyL(
        KMsgStorePropertyMrMethod, static_cast<TUint32>( aSrc.MRMethodL() ) );
        
    calendar->AddOrUpdatePropertyL(
        KMsgStorePropertyMrStatus, static_cast<TUint32>( aSrc.MRResponse() ) );
    
    //uid.
    const TDesC& uid = aSrc.UidL();
    if ( uid != KNullDesC )
        {
        HBufC8* uid8 = HBufC8::NewLC( uid.Length() );
        //uid not utf8-encoded.
        uid8->Des().Copy( uid );
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrGuid, *uid8 );
        CleanupStack::PopAndDestroy( uid8 );
        }
    
    //description.
    const TDesC& description = aSrc.DescriptionL();
    if ( description != KNullDesC )
        {
        /**@ set the body of the plain text part instead ?*/
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrCalBody, description );
        }
    
    //creation date.
    TTime time = aSrc.CreationTimeInUtcL();
    if ( Time::NullTTime() != time )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrCreationDate, time );
        }
        
    //start date.
    time = aSrc.StartTimeInUtcL();
    if ( Time::NullTTime() != time )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrStartDate, time );
        }

    //end date.
    time = aSrc.EndTimeInUtcL();
    if ( Time::NullTTime() != time )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrEndDate, time );
        }

    //alarm/reminder time.
    time = aSrc.AlarmInUtcL();
    if ( Time::NullTTime() != time )
        {
        if ( Time::NullTTime() != aSrc.StartTimeInUtcL() )
            {
            TTimeIntervalMinutes offsetMinutes;
            TInt error = aSrc.StartTimeInUtcL().MinutesFrom( time, offsetMinutes );
            
            if ( KErrNone != error )
                {
                calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrReminderTime,
                    static_cast<TUint32>( offsetMinutes.Int() ) );
                calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrReminder, ETrue );
                }
            else
                {
                __LOG_WRITE_FORMAT1_INFO(
                    "Failed to compute the reminder time, error %d.", error )
                }
            }
        }

    //location.
    const TDesC& location = aSrc.LocationL();
    if ( location != KNullDesC )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrLocation, location );
        }

    const TDesC& summary = aSrc.SubjectL();
    if ( summary != KNullDesC )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrSubject, summary );
        }

    //organizer / from.
    RMsgStoreAddress msgStoreAddress;
    CleanupClosePushL( msgStoreAddress );
    
    /*TranslateEmailFwAttendeeL( aSrc.MROrganizerL(), msgStoreAddress );
    aDst.AddOrUpdatePropertyL( KMsgStorePropertyFrom, msgStoreAddress );*/
    
    //attendees / to.
    /*RPointerArray<MMRAttendee>& attendees = aSrc.AttendeesL();
    TInt count = attendees.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        msgStoreAddress.Close();

        TranslateEmailFwAttendeeL( *(attendees[i]), msgStoreAddress );
        aDst.AddPropertyL( KMsgStorePropertyTo, msgStoreAddress );
        }*/

    CleanupStack::PopAndDestroy( &msgStoreAddress );

    //recurrence info.
    __LOG_WRITE_INFO( "Translating MMR info object recurrence rule to MsgStore." )
    const MRRecurrenceRule& rRule = aSrc.RecurrenceRuleL();

    //start date.
    time = rRule.RecurrenceStartTime();
    if ( Time::NullTTime() != time )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrStartDate, time );
        }

    //end date.
    time = rRule.RecurrentUntil();
    if ( Time::NullTTime() != time )
        {
        calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrRecurEndDate, time );
        }

    //frequency.
    TInt frequency = rRule.RecurrentInterval();
    if ( 0 != frequency )
        {
        calendar->AddOrUpdatePropertyL(
            KMsgStorePropertyMrFrequency, static_cast<TUint32>( rRule.RecurrentInterval() ) );
        }
    else
        {
        TUint index = 0;
        if ( calendar->FindProperty( KMsgStorePropertyMrFrequency, index ) )
            {
            calendar->RemovePropertyL( index );
            }
        }

    //priority.
    calendar->AddOrUpdatePropertyL(
        KMsgStorePropertyMrPriority, static_cast<TUint32>( aSrc.PriorityL() ) );

    /**@ privacy/sensitivity missing ? */
    /*idx = 0;
    if ( aCalendar.FindProperty( KMsgStorePropertyMrPrivate, idx ) )
        {
        TBool private = aCalendar.PropertyValueBoolL( idx );
        }*/

    //recurrent count.
    calendar->AddOrUpdatePropertyL(
        KMsgStorePropertyMrRecurCount, static_cast<TUint32>( rRule.RecurrentCount() ) );

    //recurrence rule type.
    MRRecurrenceRule::TMRRecurrenceType rType = rRule.Type();
    calendar->AddOrUpdatePropertyL(
        KMsgStorePropertyMrRecurType, static_cast<TUint32>( rType ) );

    //monthly-by-day-of-week.
    if ( MRRecurrenceRule::EMRRecurrenceMonthly == rType )
        {
        /**@ needs redoing after the latest MMR iface changes by activesync.*/
        /*const RArray<TInt>& monthDays = rRule.RecurrentMonthDays();
        if ( monthDays.Count() )
            {
            //week number.
            calendar->AddOrUpdatePropertyL(
                KMsgStorePropertyMrWeekNo, static_cast<TUint32>( monthDays[0].iWeekOfMonth ) );

            //day of week mask.
            TUint32 dowMask = 0;
            TranslateEmailFwDayOfWeek( monthDays[0].iDayOfWeek, dowMask );
            calendar->AddOrUpdatePropertyL(
                KMsgStorePropertyMrDayOfWeekMask, static_cast<TUint32>( dowMask ) );
            }*/
        }
    //weekly.
    else if ( MRRecurrenceRule::EMRRecurrenceWeekly == rType )
        {
        const RArray<MRRecurrenceRule::TMRRecurrentDay>& days = rRule.RecurrentWeekDays();
    
        //day of week mask.
        if ( days.Count() )
            {
            TUint32 dowMask = 0;
            
            for ( TInt i = 0; i < days.Count(); i++ )
                {
                TranslateEmailFwDayOfWeek( days[i], dowMask );
                }
            
            calendar->AddOrUpdatePropertyL(
                KMsgStorePropertyMrDayOfWeekMask, static_cast<TUint32>( dowMask ) );
            }
        }
    //yearly.
    else if ( MRRecurrenceRule::EMRRecurrenceYearly == rType )
        {
        /**@ needs redoing after the latest MMR iface changes by activesync.*/
        /*const RArray<MRRecurrenceRule::TRecurrentDaysofMonth>& monthDays = rRule.RecurrentMonthDays();
        if ( monthDays.Count() )
            {
            //week number.
            calendar->AddOrUpdatePropertyL(
                KMsgStorePropertyMrWeekNo, static_cast<TUint32>( monthDays[0].iWeekOfMonth ) );

            //day of week mask.
            TUint32 dowMask = 0;
            TranslateEmailFwDayOfWeek( monthDays[0].iDayOfWeek, dowMask );
            calendar->AddOrUpdatePropertyL(
                KMsgStorePropertyMrDayOfWeekMask, static_cast<TUint32>( dowMask ) );
            }*/
        }
    
    //recurrence id.
	calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrRRID, aSrc.MRRecurrenceId() );

    //sequence number.
	calendar->AddOrUpdatePropertyL( KMsgStorePropertyMrSeqNo, static_cast<TUint32>( aSrc.MRSequenceNumberL() ) );
	
    aDst.AddOrUpdatePropertyL( KMsgStorePropertyMeetingRequest, *calendar );
    CleanupStack::PopAndDestroy( calendar );
    __LOG_EXIT
    } //TranslateEmailFwMrL.


/**
 * Day of week translator from the MsgStore to the FS format. The actual translation
 * step is supplied via the day of week ftor. See CDayOfWeekFtor for further
 * information. Although not strictly necessary this avoids duplicating all the if's
 * when dealing with rule translation.
 *
 * @param aDayOfWeekMask day of week mask in the MsgStore format, see
 * MsgStorePropertyKeys.h for information.
 * @param aFtor translation ftor.
 */
void CBasePlugin::TranslateMsgStoreDayOfWeek(
    TUint32 aDayOfWeekMask,
    TDayOfWeekFtor& aFtor )

    {
    if ( KMsgStoreDaySunday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceSunday );
        }
    if ( KMsgStoreDayMonday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceMonday );
        }
    if ( KMsgStoreDayTuesday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceTuesday );
        }
    if ( KMsgStoreDayWednesday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceWednesday );
        }
    if ( KMsgStoreDayThursday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceThursday );
        }
    if ( KMsgStoreDayFriday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceFriday );
        }
    if ( KMsgStoreDaySaturday & aDayOfWeekMask )
        {
        aFtor( MRRecurrenceRule::EMRRecurrenceSaturday );
        }
    }


/**
 *
 */
void CBasePlugin::TranslateEmailFwDayOfWeek(
    MRRecurrenceRule::TMRRecurrentDay aSrc,
    TUint32& aDst )

    {
    if ( MRRecurrenceRule::EMRRecurrenceSunday == aSrc )
        {
        aDst |= KMsgStoreDaySunday;
        }
    if ( MRRecurrenceRule::EMRRecurrenceMonday == aSrc )
        {
        aDst |= KMsgStoreDayMonday;
        }
    if ( MRRecurrenceRule::EMRRecurrenceTuesday == aSrc )
        {
        aDst |= KMsgStoreDayTuesday;
        }
    if ( MRRecurrenceRule::EMRRecurrenceWednesday == aSrc )
        {
        aDst |= KMsgStoreDayWednesday;
        }
    if ( MRRecurrenceRule::EMRRecurrenceThursday == aSrc )
        {
        aDst |= KMsgStoreDayThursday;
        }
    if ( MRRecurrenceRule::EMRRecurrenceFriday == aSrc )
        {
        aDst |= KMsgStoreDayFriday;
        }
    if ( MRRecurrenceRule::EMRRecurrenceSaturday == aSrc )
        {
        aDst |= KMsgStoreDaySaturday;
        }
    }


/**
 *
 */
/*static*/ void TranslateMsgStoreAttendeeL(
    CMsgStorePropertyContainer& aSrc,
    TUint aIdx,
    MMROrganizer& aAttendee )

    {
    RMsgStoreAddress address;
    CleanupClosePushL( address );

    aSrc.PropertyValueAddressL( aIdx, address );

    if ( address.iEmailAddress.Length() )
        {
        aAttendee.SetAddressL( address.iEmailAddress );
        }
    
    if ( address.iDisplayName.Length() )
        {
        aAttendee.SetCommonNameL( address.iDisplayName );
        }
    
    CleanupStack::PopAndDestroy( &address );
    }


/**
 *
 */
void CBasePlugin::TranslateEmailFwAttendeeL(
    MMROrganizer& aSrc,
    RMsgStoreAddress& aDst )

    {
    const TDesC& address = aSrc.Address();
    if ( address != KNullDesC )
        {
        aDst.iEmailAddress.Create( address );
        }

    const TDesC& commonName = aSrc.CommonName();
    if ( commonName != KNullDesC )
        {
        aDst.iDisplayName.Create( commonName );
        }
    }


/**
 *
 */
void CBasePlugin::RemoveAllPropertiesL(
    CMsgStorePropertyContainer& aContainer,
    const TDesC8& aName )

    {
    TUint index = 0;
    while ( aContainer.FindProperty( aName, index ) )
        {
        aContainer.RemovePropertyL( index );
        }
    }


/**
 * 
 */
void CBasePlugin::TranslateEmailFwRecipientsL(
    CMsgStoreMessagePart& aDst,
    const TDesC8& aDstProperty,
    RPointerArray<CFSMailAddress>& aRecipients )

    {
    CBasePlugin::RemoveAllPropertiesL( aDst, aDstProperty );

    TInt count = aRecipients.Count();
    if ( 0 < count )
        {
        RMsgStoreAddress msgStoreAddress;
        CleanupClosePushL( msgStoreAddress );
    
        for ( TUint i = 0; i < count; i++ )
            {
            if ( aRecipients[i]->GetEmailAddress().Length() )
                {
                msgStoreAddress.iEmailAddress.Create( aRecipients[i]->GetEmailAddress() );
                }
            if ( aRecipients[i]->GetDisplayName().Length() )
                {
                msgStoreAddress.iDisplayName.Create( aRecipients[i]->GetDisplayName() );
                }
    
            aDst.AddPropertyL( aDstProperty, msgStoreAddress );
            msgStoreAddress.Close();
            }
        
        CleanupStack::PopAndDestroy( &msgStoreAddress );
        }    
    }


/**
 *
 */
/*static*/ void TranslateMsgStoreAttendeesL(
    CMsgStorePropertyContainer& aMessage,
    CBaseMrInfoObject& aMrInfo,
    const TDesC8& aProperty,
    MMRAttendee::TAttendeeRole aRole )
    {
    
    TUint idx = 0;
    while ( aMessage.FindProperty( aProperty, idx, idx ) )
        {
        MMRAttendee* attendee = CBaseMrAttendee::NewL();
        CleanupStack::PushL( attendee );

        TranslateMsgStoreAttendeeL( aMessage, idx, *attendee );
        attendee->SetAttendeeRoleL( aRole );

        aMrInfo.AddAttendeeL( attendee );
        CleanupStack::Pop( attendee );

        ++idx;
        }
    }


/* ANN0001
An intentional formatted text line break MUST only be included in a
"TEXT" property value by representing the line break with the
character sequence of BACKSLASH (US-ASCII decimal 92), followed by a
LATIN SMALL LETTER N (US-ASCII decimal 110) or a LATIN CAPITAL LETTER
N (US-ASCII decimal 78), that is "\n" or "\N".

http://www.kanzaki.com/docs/ical/text.html
 */
