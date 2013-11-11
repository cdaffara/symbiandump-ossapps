/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Meeting Request Info object processor
*
*/


#ifndef M_MRINFOOBJECT_H
#define M_MRINFOOBJECT_H

#include <e32base.h>
//<cmail>
#include "mmrattachment.h"
//</cmail>

// Forward declaration
class MMROrganizer;
class MMRAttendee;
class MRRecurrenceRule;

/**
 * MMRInfoObject defines meeting request interface.
 *
 * MMRInfoObject can be fetched from ES Email framework, when
 * meeting request is received in flattened format.
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class MMRInfoObject
    {

public: // Datatypes and definitions

    /**  Meeting request fields */
    enum TESMRInfoField
        {
        /** Meeting request ID field */
        EESMRInfoFieldId,
        /** Meeting request sequence number field */
        EESMRInfoFieldSeqNo,
        /** Meeting request method field */
        EESMRInfoFieldMethod,
        /** Meeting request status field */
        EESMRInfoFieldMStatus,
        /** Meeting request creation time field */
        EESMRInfoFieldCreationDateTime,
        /** Meeting request organizer field */
        EESMRInfoFieldOrganizer,
        /** Meeting request attendee field */
        EESMRInfoFieldAttendee,
        /** Meeting request optional attendee field */
        EESMRInfoFieldOptAttendee,
        /** Meeting request start time field */
        EESMRInfoFieldStartDateTime,
        /** Meeting request end time field */
        EESMRInfoFieldStopDateTime,
        /** Meeting request location field */
        EESMRInfoFieldLocation,
        /** Meeting request alarm field */
        EESMRInfoFieldAlarm,
        /** Meeting request subject field */
        EESMRInfoFieldSubject,
        /** Meeting request description field */
        EESMRInfoFieldDescription,
        /** Meeting request recurrence */
        EESMRInfoFieldRecurrence,
        /** Priority field */
        EESMRInfoFieldPriority,
        };

    /** Definition for meeting request method */
    enum TMRMethod
        {
        /** Meeting request request method */
        EMRMethodRequest,
        /** Meeting request cancel method */
        EMRMethodCancel,
        /** Meeting request response method */
        EMRMethodResponse
        };

	enum TResponse
		{     
      		EMrCmdResponseAccept,
			EMrCmdResponseTentative,
			EMrCmdResponseDecline
		};    

public: // Destruction

    /**
     * Virtual destructor.
     */
    virtual ~MMRInfoObject()  { }

public: // Interface

    /**
     * Fetches the supported fields. Supported fields are stored to array 
     * given as parameter. Array defines the fields that can be fetched 
     * from meeting request info object
     *
     * @param aFields Reference to fields table.
     * @return KErrNone if successful, otherwise system wide error code.
     */
    virtual TInt SupportedFields( RArray<TESMRInfoField>& aFields ) const = 0;

    /**
     * Fetches meeting request id. 
     * @return Meeting request id.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const TDesC& UidL() const = 0;

    /**
     * Fetches meeting request sequence number. 
     * @return Meeting request sequence number.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual TInt MRSequenceNumberL() const = 0;
    
    /**
     * Fetches meeting request method. 
     * @return Meeting request method. 
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual TMRMethod MRMethodL() const = 0;
    
    /**
     * Fetches reference to MR Organizer.
     * @return Reference to MR Organizer.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual MMROrganizer& MROrganizerL() = 0;
    
    /**
     * Adds attendee to meeting request. Ownership is transferred
     * from caller.
     * @param aAttandee Pointer to added attendee.
     * @exception System wide error code.
     */
    virtual void AddAttendeeL( MMRAttendee* aAttendee ) = 0;
    
    /**
     * Fetches Meeting request attendees.
     * @return Reference to MR attendees.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual RPointerArray<MMRAttendee>& AttendeesL() = 0;

    /**
     * Sets creation time.
     * @param aTime MR creation time in utc format.
     * @exception System wide error code.
     */
    virtual void SetCreationTimeInUtcL( TTime aTime ) = 0;

    /**
     * Fetches meeting request utc creation time. 
     * @return MR creation time in utc format.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual TTime CreationTimeInUtcL() const = 0;
    
    /**
     * Sets start time.
     * @param aTime MR start time in utc format.
     * @exception System wide error code.
     */
    virtual void SetStartTimeInUtcL( TTime aTime ) = 0;

    /**
     * Fetches meeting request utc start time. 
     * @return MR start time in utc format.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual TTime StartTimeInUtcL() const = 0;
    
    /**
     * Sets end time.
     * @param aTime MR end time in utc format.
     * @exception System wide error code.
     */
    virtual void SetEndTimeInUtcL( TTime aTime ) = 0;
    
    /**
     * Fetches meeting request utc end time. 
     * @return MR end time in utc format.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual TTime EndTimeInUtcL() const = 0;
    
    /**
     * Sets alarm time.
     * @param aTime MR alarm time in utc format.
     * @exception System wide error code.
     */
    virtual void SetAlarmInUtcL( TTime aTime ) = 0;    
    
    /*
     * Fetches meeting request utc alarm time.
     * @return MR alarm time in utc format.
     * @exception KErrNotFound, if field cannot be fetched.     
     */
    virtual TTime AlarmInUtcL() const = 0;
    
    /**
     * Sets meeting request location.
     * @param aLocation Reference to location.
     * @exception System wide error code.
     */
    virtual void SetLocationL( const TDesC& aLocation ) = 0;
    
    /**
     * Fetches meeting request location.
     * @return Reference to MR location.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const TDesC& LocationL() const = 0;
    
    /**
     * Sets meeting request description.
     * @param aLocation Reference to description.
     * @exception System wide error code.
     */
    virtual void SetDescriptionL( const TDesC& aDescription ) = 0;    
    
    /**
     * Fetches meeting request description.
     * @return Reference to MR description.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const TDesC& DescriptionL() const = 0;
    
    /**
     * Sets meeting request subject.
     * @param aLocation Reference to subject.
     * @exception System wide error code.
     */
    virtual void SetSubjectL( const TDesC& aSubject ) = 0;       
    
    /**
     * Fetches meeting request subject.
     * @return Reference to MR subject.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const TDesC& SubjectL() const = 0;    

    /**
     * meeting request response mutator
     * 
     */
    virtual void SetMRResponseL( TResponse aResponse ) = 0;
    
    /**
     * meeting request response accessor
     * 
     */
    virtual TResponse MRResponse() const = 0;
    
    /**
     * meeting request recurrence start time mutator
     * 
     */
    virtual void SetMRRecurrenceIdL( TTime aRecId ) = 0;
    
    /**
     * meeting request recurrence start time accessor
     * 
     */
    virtual TTime MRRecurrenceId() const = 0;
    
    /**
     * Sets recurrence rule information.
     * @return Reference to MR recurrence information.
     * @exception System wide error code.
     */
    virtual void SetRecurrenceRuleL(const MRRecurrenceRule& aRRule) = 0;
    
    /**
     * Fetches recurrence rule information.
     * @return Reference to MR recurrence information.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const MRRecurrenceRule& RecurrenceRuleL() const = 0;
    
    /**
     * Adds recurrence exception to this meeting request. Ownership is transferred
     * from caller.
     * @param aRule Reference to MR recurrence exception information.
     * @exception System wide error code.
     */
    virtual void AddRecurrenceExceptionL( MMRInfoObject* aException ) = 0;
    
    /**
     * Fetches recurrence exception information. 
     * @return Reference to MR recurrence exception information.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const RPointerArray<MMRInfoObject>& RecurrenceExceptionsL() const = 0;
    
    /**
     * Sets MR priority.
     * @param aPriority MR priority.
     * @exception System wide error code.
     */    
    virtual void SetPriorityL( TUint aPriority ) = 0;
        
    /**
     * Fetches MR priority.
     * @return MR priority.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual TUint PriorityL() const = 0;
    
    /**
     * Fetches meeting request attachments.
     * @return MR attachments.
     * @exception KErrNotFound, if field cannot be fetched.
     */
    virtual const RPointerArray<MMRAttachment>& AttachmentsL() const = 0;
    
    /**
     * Adds meeting request attachment. Ownership is transferred.
     * @param aAttachment Attacment to be added.
     * @exception System wide error code.
     */
    virtual void AddAttachmentsL(MMRAttachment* aAttachment) = 0;
    
    /**
     * Fetches MMRInfoObject extension interface. Caller is responsible
     * for casting returned parameter. Ownership is not transferred to 
     * caller.
     * @param aExtensionUid
     * @return Pointer to MMRInfoObject extension interface.
     */
    virtual TAny* ExtensionL( TUid aExtensionUid ) = 0;
    };

#endif // M_MRINFOOBJECT_H
