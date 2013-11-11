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
* Description:  MFSMailIterator implementation header.
*
*/


#ifndef __MAILITERATOR_H__
#define __MAILITERATOR_H__

//<cmail>
#include "MFSMailIterator.h"
#include "MsgStoreSortResultIterator.h"
#include "DebugLogMacros.h"
//</cmail>

#include "BasePlugin.h"


/**
 * A wrapper of the msgstore iterator that translates the msgstore messages to the Freestyle
 * object model.
 */
NONSHARABLE_CLASS ( HMailIterator )  : public MFSMailIterator
    {

    public:

        HMailIterator(
            CBasePlugin& aPlugin,
            CMsgStoreSortResultIterator* aIter,
            TFSMailMsgId aMailBox );

        virtual ~HMailIterator();


    // MFSMailIterator //
    public:

        virtual TBool NextL(
            TFSMailMsgId aCurrentMessageId, 
            TUint aCount,
            RPointerArray<CFSMailMessage>& aMessages );

        virtual TBool NextL(
            const TDesC& aStartWith,
            TUint aCount, 
            RPointerArray<CFSMailMessage>& aMessages );

        virtual TBool PreviousL(
            TFSMailMsgId aCurrentMessageId, 
            TUint aCount,
            RPointerArray<CFSMailMessage>& aMessages );

        virtual TBool PreviousL(
            const TDesC& aStartWith, 
            TUint aCount, 
            RPointerArray<CFSMailMessage>& aMessages );


    private:

        virtual TBool DoNextL(
            TFSMailMsgId* /*ref*/ aCurrentMessageId,
            const TDesC* /*ref*/ aStartWith,
            TUint aCount,
            RPointerArray<CFSMailMessage>& aMessages );

        virtual TBool DoPreviousL(
            TFSMailMsgId* /*ref*/ aCurrentMessageId,
            const TDesC* /*ref*/ aStartWith,
            TUint aCount,
            RPointerArray<CFSMailMessage>& aMessages );


    private:

        CMsgStoreSortResultIterator* iIter;
        CBasePlugin& iPlugin;
        TFSMailMsgId iMailBox;

        __LOG_DECLARATION
    };

#endif
