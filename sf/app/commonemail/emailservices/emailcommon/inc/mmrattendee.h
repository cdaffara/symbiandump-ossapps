/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MR organizer definition
*
*/


#ifndef M_MRINFOATTENDEE_H
#define M_MRINFOATTENDEE_H

#include <e32base.h>
//<cmail>
#include "mmrorganizer.h"
//</cmail>

/**
 * MRAttendee defines meeting request attendee.

 */
class MMRAttendee : public MMROrganizer
    {
public: // Enumerations and definitions

    /** Attendee role definition */
    enum TAttendeeRole
        {
        /** Attendee role is not set */
        EMRAttendeeNotSet,
        /** Attendee chairperson */
        EMRAttendeeChair,
        /** Required participant */
        EMRAttendeeParticipant,
        /** Optional participant */
        EMRAttendeeOptionalParticipant,
        /** Non participant */
        EMRAttendeeNonParticipant
        };

    /** Definition for attendee status */
    enum TAttendeeStatus
        {
        /** Action has not been set */
        EMRAttendeeActionNotSet,
        /** Action is required by attendee. */
        EMRAttendeeActionNeeded,
        /** Attendee has accepted request */
        EMRAttendeeActionAccepted,
        /** Attendee has tentatively accepted the request */
        EMRAttendeeActionTentative,
        /**  Attendee's presence is confirmed */
        EMRAttendeeActionConfirmed,
        /** Attendee has declined request */
        EMRAttendeeActionDeclined,
        /** The required action has been completed by attendee (i.e no actions required) */
        EMRAttendeeActionCompleted
        };

public: // Destruction

    /**
     * Virtual destructor.
     */
    virtual ~MMRAttendee()  { }

public: // Interface

    /**
     * Sets attendee role.
     * @param aRole Attendee role.
     * @exception System wide error code.
     */
    virtual void SetAttendeeRoleL(TAttendeeRole aRole ) = 0;

    /**
     * Fetches attendee role.
     * @return Attendee role.
     */
    virtual TAttendeeRole AttendeeRole() const = 0;

    /**
     * Sets attendee status.
     * @param aStatus Attendee status.
     * @exception System wide error code.
     */
    virtual void SetAttendeeStatusL( TAttendeeStatus& aStatus ) = 0;

    /**
     * Fetches attendee status.
     * @return attendee status.
     */
    virtual TAttendeeStatus AttendeeStatus() const = 0;

    /**
     * Sets whether or not a response was requested for this attendee.
     * @param aRsvp ETrue if a response was requested, EFalse if not.
     * @exception System wide error code.
     */
    virtual void SetResponseRequestedL( TBool aRsvp ) = 0;

    /**
     * Gets whether or not a response was requested for this attendee.
     * @return ETrue if a response was requested, EFalse if not.
     */
    virtual TBool ResponseRequested() const = 0;
    };


#endif // M_MRINFOATTENDEE_H
