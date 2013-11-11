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
* Description:   Meeting request support implementation.
*
*/




#include "BaseMrInfoObject.h"
#include "BasePlugin.h"


/**
 * Defaults: Method of EMRMethodRequest.
 */
EXPORT_C CBaseMrInfoObject* CBaseMrInfoObject::NewL()
    {
    CBaseMrInfoObject* self = new(ELeave) CBaseMrInfoObject();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


/**
 * Copy construction.
 */
EXPORT_C CBaseMrInfoObject* CBaseMrInfoObject::NewL(
    MMRInfoObject& aMrInfo )
    {
    CBaseMrInfoObject* self = new(ELeave) CBaseMrInfoObject();
    CleanupStack::PushL( self );
    self->ConstructL( aMrInfo );
    CleanupStack::Pop( self );
    return self;
    }
    
    
/**
 *
 */
CBaseMrInfoObject::CBaseMrInfoObject()
    : iCreationTime( Time::NullTTime() ), iStartTime( Time::NullTTime() ),
    iEndTime( Time::NullTTime() ), iAlarmTime( Time::NullTTime() ),
    iRecurrenceId( Time::NullTTime() ), iPriority( KIcalNormalPriority ), iMethod( EMRMethodRequest ),
    iResponse( EMrCmdResponseTentative )
    {
    }


/**
 * Copy construction
 */
void CBaseMrInfoObject::ConstructL(
    MMRInfoObject& aMrInfo )
    {    
    iSubject = aMrInfo.SubjectL().AllocL();
    iDescription = aMrInfo.DescriptionL().AllocL();
    iLocation = aMrInfo.LocationL().AllocL();
    iUid = aMrInfo.UidL().AllocL();
    
    iCreationTime = aMrInfo.CreationTimeInUtcL();
    iStartTime = aMrInfo.StartTimeInUtcL();
    iEndTime = aMrInfo.EndTimeInUtcL();
    iAlarmTime = aMrInfo.AlarmInUtcL();
    iMethod = aMrInfo.MRMethodL();
    iResponse = aMrInfo.MRResponse();
    iRecurrenceId = aMrInfo.MRRecurrenceId();
    
    iSequenceNumber = aMrInfo.MRSequenceNumberL();
    iPriority = aMrInfo.PriorityL();

    iOrganizer = CBaseMrAttendee::NewL( aMrInfo.MROrganizerL() );
//<qmail> Commented out in 10.1
    
    /*RPointerArray<MMRAttendee>& attendees = aMrInfo.AttendeesL();
    TInt count = attendees.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        MMRAttendee* attendee = CBaseMrAttendee::NewL( *(attendees[i]) );
        CleanupStack::PushL( attendee );
        iAttendees.AppendL( attendee );
        CleanupStack::Pop( attendee );
        }*/

//</qmail>
    
    iRrule = CBaseMrRecurrenceRule::NewL( aMrInfo.RecurrenceRuleL() );
    /**@ copy construction for iExceptions, iAttachments.*/
    }


/**
 *
 */
void CBaseMrInfoObject::ConstructL()
    {    
    iSubject = KNullDesC().AllocL();
    iDescription = KNullDesC().AllocL();
    iLocation = KNullDesC().AllocL();
    iUid = KNullDesC().AllocL();
    
    iOrganizer = CBaseMrAttendee::NewL();
    }

    
/**
 *
 */
EXPORT_C CBaseMrInfoObject::~CBaseMrInfoObject()
    {
    delete iSubject;
    delete iDescription;
    delete iLocation;
    delete iUid;

    delete iOrganizer;

    iExceptions.ResetAndDestroy();
    iAttachments.ResetAndDestroy();
    iAttendees.ResetAndDestroy();

    delete iRrule;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetMRRecurrenceIdL( TTime aRecId )
    {
    iRecurrenceId = aRecId;
    }


/**
 *
 */
EXPORT_C TTime CBaseMrInfoObject::MRRecurrenceId() const
    {
    return iRecurrenceId;
    }

/**
 * 
 */
EXPORT_C void CBaseMrInfoObject::SetMRSequenceNumber( TInt aSeqNo )
	{
	iSequenceNumber = aSeqNo;
	}

/**
 *
 */
EXPORT_C TInt CBaseMrInfoObject::SupportedFields(
    RArray<TESMRInfoField>& aFields ) const
    
    {
    /**@ have more fields to add.*/
    TESMRInfoField fields[] = 
        {
        EESMRInfoFieldId,
        EESMRInfoFieldSeqNo,
        EESMRInfoFieldMethod,
        EESMRInfoFieldMStatus,
        EESMRInfoFieldCreationDateTime,
        EESMRInfoFieldOrganizer,
        EESMRInfoFieldAttendee,
        EESMRInfoFieldOptAttendee,
        EESMRInfoFieldStartDateTime,
        EESMRInfoFieldStopDateTime,
        EESMRInfoFieldLocation,
        EESMRInfoFieldAlarm,
        EESMRInfoFieldSubject,
        EESMRInfoFieldDescription,
        EESMRInfoFieldRecurrence,
        EESMRInfoFieldPriority
        };
    
    TInt result = KErrNone;
    for ( TInt i = 0;
        i < sizeof( fields ) / sizeof( TESMRInfoField ) && result == KErrNone;
        i++ )
        {
        result = aFields.Append( fields[i] );
        }
    
    return result;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetUidL(
    const TDesC& aUid )
    {
    delete iUid;
    iUid = NULL;
    
    iUid = aUid.AllocL();
    }


/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetUidL(
    HBufC* aUid )
    {
    delete iUid;
    iUid = NULL;

    iUid = aUid;
    }


/**
 *
 */
EXPORT_C const TDesC& CBaseMrInfoObject::UidL() const
    {
    return *iUid;
    }
    
    
/**
 *
 */
EXPORT_C TInt CBaseMrInfoObject::MRSequenceNumberL() const
    {
    return iSequenceNumber;
    }
    
    
/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetMethod(
    TMRMethod aMethod )
    {
    iMethod = aMethod;
    }
    
        
/**
 *
 */
EXPORT_C MMRInfoObject::TMRMethod CBaseMrInfoObject::MRMethodL() const
    {
    return iMethod;
    }
        

/**
 *
 */
EXPORT_C MMROrganizer& CBaseMrInfoObject::MROrganizerL()
    {
    return *iOrganizer;
    }
        

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::AddAttendeeL(
    MMRAttendee* aAttendee )
    {
    iAttendees.AppendL( aAttendee );
    }
        

/**
 *
 */
EXPORT_C RPointerArray<MMRAttendee>& CBaseMrInfoObject::AttendeesL()
    {
    return iAttendees;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetCreationTimeInUtcL(
    TTime aTime )
    {
    iCreationTime = aTime;
    }
    

/**
 *
 */
EXPORT_C TTime CBaseMrInfoObject::CreationTimeInUtcL() const
    {
    return iCreationTime;
    }
        

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetStartTimeInUtcL(
    TTime aTime )
    {
    iStartTime = aTime;
    }
    

/**
 *
 */
EXPORT_C TTime CBaseMrInfoObject::StartTimeInUtcL() const
    {
    return iStartTime;
    }
        

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetEndTimeInUtcL(
    TTime aTime )
    {
    iEndTime = aTime;
    }
        

/**
 *
 */
EXPORT_C TTime CBaseMrInfoObject::EndTimeInUtcL() const
    {
    return iEndTime;
    }
        

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetAlarmInUtcL(
    TTime aTime )
    {
    iAlarmTime = aTime;
    }
        

/**
 *
 */
EXPORT_C TTime CBaseMrInfoObject::AlarmInUtcL() const
    {
    return iAlarmTime;
    }
        

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetLocationL(
    const TDesC& aLocation )
    {
    delete iLocation;
    iLocation = NULL;
    
    iLocation = aLocation.AllocL();
    }
        

/**
 *
 */
EXPORT_C const TDesC& CBaseMrInfoObject::LocationL() const
    {
    return *iLocation;
    }
        
        
/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetDescriptionL(
    const TDesC& aDescription )
    {
    delete iDescription;
    iDescription = NULL;
    
    iDescription = aDescription.AllocL();
    }
        
        
/**
 *
 */
EXPORT_C const TDesC& CBaseMrInfoObject::DescriptionL() const
    {
    return *iDescription;
    }
        

/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetSubjectL(
    const TDesC& aSubject )
    {
    delete iSubject;
    iSubject = NULL;
    
    iSubject = aSubject.AllocL();
    }
        

/**
 *
 */
EXPORT_C const TDesC& CBaseMrInfoObject::SubjectL() const
    {
    return *iSubject;
    }
        
        
/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetRecurrenceRuleL(
    const MRRecurrenceRule& aRRule )
    {
    iRrule = &aRRule;
    }
        

/**
 *
 */
EXPORT_C const MRRecurrenceRule& CBaseMrInfoObject::RecurrenceRuleL() const
    {
    return *iRrule;
    }


/**
 *
 */
EXPORT_C void CBaseMrInfoObject::AddRecurrenceExceptionL(
    MMRInfoObject* aException )
    {
    iExceptions.AppendL( aException );
    }
        
        
/**
 *
 */
EXPORT_C const RPointerArray<MMRInfoObject>&
    CBaseMrInfoObject::RecurrenceExceptionsL() const
    {
    return iExceptions;
    }
        
        
/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetPriorityL(
    TUint aPriority )
    {
    iPriority = aPriority;
    }
            

/**
 *
 */
EXPORT_C TUint CBaseMrInfoObject::PriorityL() const
    {
    return iPriority;
    }
        

/**
 *
 */
EXPORT_C const RPointerArray<MMRAttachment>& CBaseMrInfoObject::AttachmentsL() const
    {
    return iAttachments;
    }
        
        
/**
 *
 */
EXPORT_C void CBaseMrInfoObject::AddAttachmentsL(
    MMRAttachment* aAttachment )
    {
    iAttachments.AppendL( aAttachment );
    }
        
        
/**
 *
 */
EXPORT_C TAny* CBaseMrInfoObject::ExtensionL(
    TUid /*aExtensionUid*/ )
    {
    return NULL;
    }


/**
 *
 */
EXPORT_C void CBaseMrInfoObject::SetMRResponseL(
    TResponse aResponse )
    {
    iResponse = aResponse;
    }
    

/**
 *
 */
EXPORT_C MMRInfoObject::TResponse CBaseMrInfoObject::MRResponse() const
    {
    return iResponse;
    }


#pragma mark -


/**
 *
 */
EXPORT_C CBaseMrAttendee* CBaseMrAttendee::NewL()
    {
    CBaseMrAttendee* self = new (ELeave) CBaseMrAttendee();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


/**
 *
 */
EXPORT_C CBaseMrAttendee* CBaseMrAttendee::NewL(
    const MMRAttendee& aAttendee )
    {
    CBaseMrAttendee* self = new (ELeave) CBaseMrAttendee();
    CleanupStack::PushL( self );
    self->ConstructL( aAttendee );
    CleanupStack::Pop( self );
    return self;
    }


/**
 *
 */
EXPORT_C CBaseMrAttendee* CBaseMrAttendee::NewL(
    const MMROrganizer& aOrganizer )
    {
    CBaseMrAttendee* self = new (ELeave) CBaseMrAttendee();
    CleanupStack::PushL( self );
    self->ConstructL( aOrganizer );
    CleanupStack::Pop( self );
    return self;
    }


/**
 *
 */
CBaseMrAttendee::CBaseMrAttendee()
    : iRole( MMRAttendee::EMRAttendeeNotSet ),
    iStatus( MMRAttendee::EMRAttendeeActionNotSet ), iRsvp( EFalse )
    {
    }


/**
 *
 */
void CBaseMrAttendee::ConstructL()
    {
    iAddress = KNullDesC().AllocL();
    iCommonName = KNullDesC().AllocL();
    }


/**
 *
 */
void CBaseMrAttendee::ConstructL(
    const MMRAttendee& aAttendee )
    {
    iAddress = aAttendee.Address().AllocL();
    iCommonName = aAttendee.CommonName().AllocL();
    
    iRole = aAttendee.AttendeeRole();
    iStatus = aAttendee.AttendeeStatus();
    iRsvp = aAttendee.ResponseRequested();
    }


/**
 *
 */
void CBaseMrAttendee::ConstructL(
    const MMROrganizer& aOrganizer )
    {
    iAddress = aOrganizer.Address().AllocL();
    iCommonName = aOrganizer.CommonName().AllocL();
    }


/**
 *
 */
EXPORT_C CBaseMrAttendee::~CBaseMrAttendee()
    {
    delete iCommonName;
    delete iAddress;
    }


/**
 *
 */
EXPORT_C void CBaseMrAttendee::SetAttendeeRoleL(
    TAttendeeRole aRole )
    {
    iRole = aRole;
    }

     
/**
 *
 */
EXPORT_C MMRAttendee::TAttendeeRole CBaseMrAttendee::AttendeeRole() const
    {
    return iRole;
    }


/**
 *
 */
EXPORT_C void CBaseMrAttendee::SetAddressL(  
    const TDesC& aAddress )
    {
    delete iAddress;
    iAddress = NULL;
    
    iAddress = aAddress.AllocL();
    }


/**
 *
 */
EXPORT_C const TDesC& CBaseMrAttendee::Address() const
    {
    return *iAddress;    
    }
    
    
/**
 *
 */
EXPORT_C void CBaseMrAttendee::SetCommonNameL(
    const TDesC& aCommonName )
    {
    delete iCommonName;
    iCommonName = NULL;
    
    iCommonName = aCommonName.AllocL();
    }
    
    
/**
 *
 */
EXPORT_C const TDesC& CBaseMrAttendee::CommonName() const
    {
    return *iCommonName;
    }
    
    
/**
 *
 */
EXPORT_C void CBaseMrAttendee::SetAttendeeStatusL(
    TAttendeeStatus& aStatus )
    {
    iStatus = aStatus;
    }
    

/**
 *
 */
EXPORT_C MMRAttendee::TAttendeeStatus CBaseMrAttendee::AttendeeStatus() const
    {
    return iStatus;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrAttendee::SetResponseRequestedL(
    TBool aRsvp )
    {
    iRsvp = aRsvp;
    }
    

/**
 *
 */
EXPORT_C TBool CBaseMrAttendee::ResponseRequested() const
    {
    return iRsvp;
    }


#pragma mark -


/**
 *
 */
EXPORT_C CBaseMrRecurrenceRule* CBaseMrRecurrenceRule::NewL()
    {
    CBaseMrRecurrenceRule* self = new(ELeave) CBaseMrRecurrenceRule();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


/**
 *
 */
EXPORT_C CBaseMrRecurrenceRule* CBaseMrRecurrenceRule::NewL(
    const MRRecurrenceRule& aRrule )
    {
    CBaseMrRecurrenceRule* self = new(ELeave) CBaseMrRecurrenceRule( aRrule );
    CleanupStack::PushL( self );
    self->ConstructL( aRrule );
    CleanupStack::Pop( self );
    return self;
    }


/**
 *
 */
CBaseMrRecurrenceRule::CBaseMrRecurrenceRule()
    : iStartTime( Time::NullTTime() ), iUntilTime( Time::NullTTime() )
    {
    }
    
    
/**
 *
 */
CBaseMrRecurrenceRule::CBaseMrRecurrenceRule(
    const MRRecurrenceRule& aRrule )
    : iStartTime( aRrule.RecurrenceStartTime() ), iUntilTime( aRrule.RecurrentUntil() ), iType( aRrule.Type() ),
    iInterval( aRrule.RecurrentInterval() ), iCount( aRrule.RecurrentCount() )
    {

    }


/**
 *
 */
void CBaseMrRecurrenceRule::ConstructL()
    {    
    }


/**
 *
 */
void CBaseMrRecurrenceRule::ConstructL(
    const MRRecurrenceRule& aRrule )
    {
    
    const RArray<TMRRecurrentDay>& weekDays = aRrule.RecurrentWeekDays();
    TInt count = weekDays.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iWeekDays.AppendL( weekDays[i] );
        }
    
    const RArray<TInt>& monthDays = aRrule.RecurrentMonthDays();
    count = monthDays.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iMonthDays.AppendL( monthDays[i] );
        }
    
    const RArray<TMRRecurrenceMonth>& months = aRrule.RecurrentMonths();
    count = months.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iMonths.AppendL( months[i] );
        }
    
    const RArray<TRecurrentDaysofMonth>& daysofMonth = aRrule.RecurrentDaysofMonth();
    count = daysofMonth.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iDaysofMonth.AppendL( daysofMonth[i] );
        }
    }

    
/**
 *
 */
EXPORT_C CBaseMrRecurrenceRule::~CBaseMrRecurrenceRule()
    {
    iDaysofMonth.Close();
    iWeekDays.Close();
    iMonthDays.Close();
    iMonths.Close();
    }


/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrenceTypeL(
    MRRecurrenceRule::TMRRecurrenceType aType )
    {
    iType = aType;
    }


/**
 *
 */
EXPORT_C MRRecurrenceRule::TMRRecurrenceType CBaseMrRecurrenceRule::Type() const
    {
    return iType;
    }


/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrenceStartTimeL(
    TTime aStartTime )
    {
    iStartTime = aStartTime;
    }


/**
 *
 */
EXPORT_C TTime CBaseMrRecurrenceRule::RecurrenceStartTime() const
    {
    return iStartTime;
    }


/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrenceUntilL(
    TTime aUntilTime )
    {
    iUntilTime = aUntilTime;
    }


/**
 *
 */
EXPORT_C TTime CBaseMrRecurrenceRule::RecurrentUntil() const
    {
    return iUntilTime;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrentInterval(
    TInt aInterval )
    {
    iInterval = aInterval;
    }
    

/**
 *
 */
EXPORT_C TInt CBaseMrRecurrenceRule::RecurrentInterval() const
    {
    return iInterval;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrentCountL(
    TInt aCount )
    {
    iCount = aCount;
    }
    

/**
 *
 */
EXPORT_C TUint CBaseMrRecurrenceRule::RecurrentCount() const
    {
    return iCount;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrentWeekDaysL(
    RArray<TMRRecurrentDay>& aDays )
    {
    TInt count = aDays.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iWeekDays.AppendL( aDays[i] );
        }
    }
    

/**
 *
 */
EXPORT_C const RArray<MRRecurrenceRule::TMRRecurrentDay>&
    CBaseMrRecurrenceRule::RecurrentWeekDays() const
    {
    return iWeekDays;
    }
    

/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrentMonthDaysL(
    RArray<TInt>& aDays )
    {
    TInt count = aDays.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iMonthDays.AppendL( aDays[i] );
        }    
    }
    

/**
 *
 */
EXPORT_C const RArray<TInt>& CBaseMrRecurrenceRule::RecurrentMonthDays() const
    {
    return iMonthDays;    
    }


/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrentDaysofMonthL(
    RArray<TRecurrentDaysofMonth>& aDaysofMonth )
    {
    TInt count = aDaysofMonth.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iDaysofMonth.AppendL( aDaysofMonth[i] );
        }
    }


/**
 *
 */
EXPORT_C const RArray<MRRecurrenceRule::TRecurrentDaysofMonth>&
    CBaseMrRecurrenceRule::RecurrentDaysofMonth() const
    {
    return iDaysofMonth;
    }


/**
 *
 */
EXPORT_C void CBaseMrRecurrenceRule::SetRecurrentMonthsL(
    RArray<TMRRecurrenceMonth>& aMonths )
    {
    TInt count = aMonths.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iMonths.AppendL( aMonths[i] );
        }
    }
    

/**
 *
 */
EXPORT_C const RArray<MRRecurrenceRule::TMRRecurrenceMonth>&
    CBaseMrRecurrenceRule::RecurrentMonths() const
    {
    return iMonths;    
    }


#pragma mark -


/**
 *
 */
EXPORT_C CBaseMrInfoProcessor* CBaseMrInfoProcessor::NewL(
    CBasePlugin& aPlugin )
    {
    CBaseMrInfoProcessor* self = new (ELeave) CBaseMrInfoProcessor( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


/**
 *
 */
CBaseMrInfoProcessor::CBaseMrInfoProcessor( CBasePlugin& aPlugin )
    : iPlugin( aPlugin )
    {
    }


/**
 *
 */
void CBaseMrInfoProcessor::ConstructL()
    {
    }


/**
 *
 */
EXPORT_C CBaseMrInfoProcessor::~CBaseMrInfoProcessor()
    {
    }


/**
 * Create and send a MR response email.
 */
EXPORT_C void CBaseMrInfoProcessor::ReplyToMeetingRequestL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    MMRInfoObject& aMeetingRequest,
    TMRInfoResponseMode& aResponseMode,
    const TDesC& aResponseText /*= KNullDesC()*/ )

    {
    if ( MMRInfoProcessor::EMRInfoResponseSync != aResponseMode )
        {
        CFSMailMessage* reply = iPlugin.CreateMrReplyMessageL(
            aMailBoxId, aMeetingRequest, aMessageId );
        CleanupStack::PushL( reply );
        
        //body.
        if ( aResponseText != KNullDesC )
            {
            CFSMailMessagePart* body = reply->PlainTextBodyPartL();
            if ( NULL != body )
                {
                CleanupStack::PushL( body );
                
                /**@ completely unnecessary but the fw needs to accept a TDesC in
                SetContent instead.*/
                HBufC* descCopy = aResponseText.AllocLC();
                TPtr ptr = descCopy->Des();
                body->SetContent( ptr );
                
                CleanupStack::PopAndDestroy( descCopy );
                CleanupStack::PopAndDestroy( body );
                }
            }
        
        CBaseMrInfoObject* mrInfo = CBaseMrInfoObject::NewL( aMeetingRequest );
    
        if ( MMRInfoProcessor::EMRInfoRemoveFromCal == aResponseMode )
        	{
        	mrInfo->SetMethod( MMRInfoObject::EMRMethodCancel );
        	}
        else
        	{
        	mrInfo->SetMethod( MMRInfoObject::EMRMethodResponse );
        	}
    
        reply->SetMRInfo( mrInfo );
        iPlugin.SendMessageL( *reply );
    
        CleanupStack::PopAndDestroy( reply );
        }
    	
    } //ReplyToMeetingRequestL.
