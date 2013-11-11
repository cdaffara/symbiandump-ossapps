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
* Description:  Meeting request support.
*
*/


#ifndef __BASEMRINFOOBJECT_H__
#define __BASEMRINFOOBJECT_H__

#include <e32std.h>
#include "CFSMailCommon.h"
#include "mmrinfoobject.h"
#include "mmrorganizer.h"
#include "mmrattendee.h"
#include "mmrattachment.h"
#include "mmrinfoprocessor.h"
#include "mmrrecurrencerule.h"


class CBasePlugin;


const TUint KIcalNormalPriority = 5; 


/**
 * Use where you need an attendee and/or an organizer.
 */
NONSHARABLE_CLASS ( CBaseMrAttendee ) :
    public CBase,
    public MMRAttendee
    {

    public:
        IMPORT_C static CBaseMrAttendee* NewL();
        IMPORT_C static CBaseMrAttendee* NewL(
            const MMRAttendee& aAttendee );
        IMPORT_C static CBaseMrAttendee* NewL(
            const MMROrganizer& aOrganizer );
        IMPORT_C virtual ~CBaseMrAttendee();

    private:
        CBaseMrAttendee();
        void ConstructL();
        void ConstructL(
            const MMRAttendee& aAttendee );
        void ConstructL(
            const MMROrganizer& aOrganizer );


    // MRAttendee //
    public:

        IMPORT_C void SetAttendeeRoleL(
            TAttendeeRole aRole );
     
        IMPORT_C TAttendeeRole AttendeeRole() const;
    
        IMPORT_C void SetAttendeeStatusL(
            TAttendeeStatus& aStatus );
    
        IMPORT_C TAttendeeStatus AttendeeStatus() const;
    
        IMPORT_C void SetResponseRequestedL(
            TBool aRsvp );
    
        IMPORT_C TBool ResponseRequested() const;


    // MMROrganizer //
    public:
        IMPORT_C void SetAddressL(  
            const TDesC& aAddress );

        IMPORT_C const TDesC& Address() const;
    
        IMPORT_C void SetCommonNameL(
            const TDesC& aCommonName );
    
        IMPORT_C const TDesC& CommonName() const;
        
    protected:
        HBufC* iAddress;
        HBufC* iCommonName;
        TAttendeeRole iRole;
        TAttendeeStatus iStatus;
        TBool iRsvp;
    };


/**
 *
 */
NONSHARABLE_CLASS ( CBaseMrRecurrenceRule ) :
    public CBase,
    public MRRecurrenceRule
    {

    public:

        IMPORT_C static CBaseMrRecurrenceRule* NewL();

        IMPORT_C static CBaseMrRecurrenceRule* NewL(
            const MRRecurrenceRule& aRrule );

        IMPORT_C virtual ~CBaseMrRecurrenceRule();


    private:
        CBaseMrRecurrenceRule();
        CBaseMrRecurrenceRule(
            const MRRecurrenceRule& aRrule );
        void ConstructL();
        void ConstructL(
            const MRRecurrenceRule& aRrule );

    // MRRecurrenceRule //
    public:
        IMPORT_C void SetRecurrenceTypeL(
            TMRRecurrenceType aType );

        IMPORT_C TMRRecurrenceType Type() const;

        IMPORT_C void SetRecurrenceStartTimeL(
            TTime aStartTime );

        IMPORT_C TTime RecurrenceStartTime() const;

        IMPORT_C void SetRecurrenceUntilL(
            TTime aUntilTime );

        IMPORT_C TTime RecurrentUntil() const;
    
        IMPORT_C void SetRecurrentInterval(
            TInt aInterval );
    
        IMPORT_C TInt RecurrentInterval() const;
    
        IMPORT_C void SetRecurrentCountL(
            TInt aCount );
    
        IMPORT_C TUint RecurrentCount() const;
    
        IMPORT_C void SetRecurrentWeekDaysL(
            RArray<TMRRecurrentDay>& aDays );
    
        IMPORT_C const RArray<TMRRecurrentDay>& RecurrentWeekDays() const;
    
        IMPORT_C void SetRecurrentMonthDaysL(
            RArray<TInt>& aDays );
    
        IMPORT_C const RArray<TInt>& RecurrentMonthDays() const;

	    IMPORT_C void SetRecurrentDaysofMonthL(
	        RArray<TRecurrentDaysofMonth>& aDaysosMonth );
		
	    IMPORT_C const RArray<TRecurrentDaysofMonth>& RecurrentDaysofMonth() const;

        IMPORT_C void SetRecurrentMonthsL(
            RArray<TMRRecurrenceMonth>& aMonths );
    
        IMPORT_C const RArray<TMRRecurrenceMonth>& RecurrentMonths() const;
        
    protected:
        RArray<TRecurrentDaysofMonth> iDaysofMonth;
        RArray<TMRRecurrentDay> iWeekDays;
        RArray<TInt> iMonthDays;
        RArray<TMRRecurrenceMonth> iMonths;
        TTime iStartTime;
        TTime iUntilTime;
        TMRRecurrenceType iType;
        TInt iInterval;
        TInt iCount;
    };


/**
 *
 */
NONSHARABLE_CLASS ( CBaseMrInfoObject ) :
    public CBase,
    public MMRInfoObject

    {
    
    public:
        IMPORT_C static CBaseMrInfoObject* NewL();
        IMPORT_C static CBaseMrInfoObject* NewL(
            MMRInfoObject& aMrInfo );
        IMPORT_C virtual ~CBaseMrInfoObject();

    private:
        CBaseMrInfoObject();
        void ConstructL();
        void ConstructL(
            MMRInfoObject& aMrInfo );

    public:
        IMPORT_C void SetUidL(
            const TDesC& aUid );
        IMPORT_C void SetUidL(
            HBufC* aUid );
        IMPORT_C void SetMethod(
            TMRMethod aMethod );
        IMPORT_C void SetMRSequenceNumber(
        	TInt aSeqNo );
        
    // MMRInfoObject //
    public:

        IMPORT_C TInt SupportedFields(
            RArray<TESMRInfoField>& aFields ) const;
    
        IMPORT_C const TDesC& UidL() const;
    
        IMPORT_C TInt MRSequenceNumberL() const;
        
        IMPORT_C TMRMethod MRMethodL() const;
        
        IMPORT_C MMROrganizer& MROrganizerL();
        
        IMPORT_C void AddAttendeeL(
            MMRAttendee* aAttendee );
        
        IMPORT_C RPointerArray<MMRAttendee>& AttendeesL();
    
        IMPORT_C void SetCreationTimeInUtcL(
            TTime aTime );
    
        IMPORT_C TTime CreationTimeInUtcL() const;
        
        IMPORT_C void SetStartTimeInUtcL(
            TTime aTime );
    
        IMPORT_C TTime StartTimeInUtcL() const;
        
        IMPORT_C void SetEndTimeInUtcL(
            TTime aTime );
        
        IMPORT_C TTime EndTimeInUtcL() const;
        
        IMPORT_C void SetAlarmInUtcL(
            TTime aTime );    
        
        IMPORT_C TTime AlarmInUtcL() const;
        
        IMPORT_C void SetLocationL(
            const TDesC& aLocation );
        
        IMPORT_C const TDesC& LocationL() const;
        
        IMPORT_C void SetDescriptionL(
            const TDesC& aDescription );    
        
        IMPORT_C const TDesC& DescriptionL() const;
        
        IMPORT_C void SetSubjectL(
            const TDesC& aSubject );       
        
        IMPORT_C const TDesC& SubjectL() const;    
        
        IMPORT_C void SetRecurrenceRuleL(
            const MRRecurrenceRule& aRRule );
        
        IMPORT_C const MRRecurrenceRule& RecurrenceRuleL() const;

        IMPORT_C void AddRecurrenceExceptionL(
            MMRInfoObject* aException );
        
        IMPORT_C const RPointerArray<MMRInfoObject>& RecurrenceExceptionsL() const;
        
        IMPORT_C void SetPriorityL(
            TUint aPriority );
            
        IMPORT_C TUint PriorityL() const;
        
        IMPORT_C const RPointerArray<MMRAttachment>& AttachmentsL() const;
        
        IMPORT_C void AddAttachmentsL(
            MMRAttachment* aAttachment );
        
        IMPORT_C TAny* ExtensionL(
            TUid aExtensionUid );

        IMPORT_C void SetMRResponseL(
            TResponse aResponse );
    
        IMPORT_C TResponse MRResponse() const;

        IMPORT_C void SetMRRecurrenceIdL(
            TTime aRecId );
        
        IMPORT_C TTime MRRecurrenceId() const;


    protected:
        RPointerArray<MMRInfoObject> iExceptions;
        RPointerArray<MMRAttachment> iAttachments;
        RPointerArray<MMRAttendee> iAttendees;
        MMRAttendee* iOrganizer;
        const MRRecurrenceRule* /*ref*/ iRrule;
        HBufC* iUid;
        HBufC* iLocation;
        HBufC* iSubject;
        HBufC* iDescription;        
        TTime iCreationTime;
        TTime iStartTime;
        TTime iEndTime;
        TTime iAlarmTime;
        TTime iRecurrenceId;
        TInt iSequenceNumber;
        TUint iPriority;
        TMRMethod iMethod;
        TResponse iResponse;
    };


/**
 *
 */
NONSHARABLE_CLASS(  CBaseMrInfoProcessor ) :
    public CBase,
    public MMRInfoProcessor
    {

    public:
        IMPORT_C static CBaseMrInfoProcessor* NewL(
            CBasePlugin& aPlugin );
        IMPORT_C virtual ~CBaseMrInfoProcessor();

    private:
        CBaseMrInfoProcessor(
            CBasePlugin& aPlugin );
        void ConstructL();


    public:
        IMPORT_C virtual void ReplyToMeetingRequestL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aMessageId,
            MMRInfoObject& aMeetingRequest,
            TMRInfoResponseMode& aResponseMode,
            const TDesC& aResponseText = KNullDesC() );
            
    private:
        CBasePlugin& iPlugin;
    };


#endif // __BASEMRINFOOBJECT_H__
