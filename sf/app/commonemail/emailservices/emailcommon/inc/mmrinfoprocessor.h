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


#ifndef M_MRINFOPROCESSOR_H
#define M_MRINFOPROCESSOR_H

#include <e32base.h>
//<cmail>
#include "CFSMailCommon.h"
//</cmail>

// Forward declaration
class MMRInfoObject;

/**
 * MMRInfoProcessor defines interface for processing meeting requests.
 *
 * MMRInfoProcessor contains method for responding to meeting request.
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class MMRInfoProcessor
    {
public:

    /**
     * Defines response mode when replying to MR using MRINFO object.
     */
    enum TMRInfoResponseMode
        {
        /**
         * Response message is sent to organizer
         * but no syncing os done.
         */
        EMRInfoResponseSend,
        /**
         * Meeting request data is only synced. No
         * response message is sent to organizer.
         */ 
        EMRInfoResponseSync,
        /** 
         * Meeting request data is synced and response message 
         * is sent to organizer.
         */ 
        EMRInfoResponseSendAndSync,
        
        /** 
         * Meeting Requset is removed from calendar
         */
        EMRInfoRemoveFromCal
        };

    /**
     * Virtual destructor
     */
    virtual ~MMRInfoProcessor() { }

public: // Interface

    /**
     * Sends response to meeting request. Response is sent to meeting
     * request organzier. MMRInfoObject' status contains the response
     * (i.e is MR accepted, tentatively accepted or declined ).
     * Caller can attach response text to response.
     *
     * @param aMessageId defines
     * @param aMeetingRequest Reference to meeting request to be sent.
     * @param aResponseText Reference to response text attached to response.
     * @exception Normal system wide error codes.
     */
    virtual void ReplyToMeetingRequestL(
            const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aMessageId,
            MMRInfoObject& aMeetingRequest,
            TMRInfoResponseMode& aResponseMode,
            const TDesC& aResponseText = KNullDesC() ) = 0;
    };

#endif // M_MRINFOPROCESSOR_H
