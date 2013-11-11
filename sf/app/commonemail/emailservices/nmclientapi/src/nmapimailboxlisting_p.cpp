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

#include "nmapiheaders.h"


namespace EmailClientApi
{
NmApiMailboxListingPrivate::NmApiMailboxListingPrivate(QObject *parent) :
    QObject(parent), mNmApiEngine(NULL)
{
    NM_FUNCTION;
}

NmApiMailboxListingPrivate::~NmApiMailboxListingPrivate()
{
    NM_FUNCTION;
}

/*!
   \brief It initialize engine for email operations. 
   
   When use initializeEngine need to remember release it.
   It return value if initialization go good.
   \sa releaseEngine 
   \return Return true if engine is good initilialized.
 */
bool NmApiMailboxListingPrivate::initializeEngine()
{
    NM_FUNCTION;
    
    if (!mNmApiEngine) {
        mNmApiEngine = NmApiEngine::instance();
    }

    return mNmApiEngine ? true : false;
}

/*!
   \brief It release engine for email operations.
   
   \sa initializeEngine
 */
void NmApiMailboxListingPrivate::releaseEngine()
{
    NM_FUNCTION;
    
    NmApiEngine::releaseInstance(mNmApiEngine);
}

/*!
   \brief It grab mailboxes from engine. 
   
   When it start grabing, it release all old.
   Because it uses NmApiMailbox with sharedData we don't need care about release memory.
   
   \return Count of mailboxes or "-1" if there is no engine
 */
qint32 NmApiMailboxListingPrivate::grabMailboxes()
{
    NM_FUNCTION;
    
    if (!mNmApiEngine) {
        return -1;
    }
    
    mMailboxes.clear();

    mNmApiEngine->listMailboxes(mMailboxes);
    return mMailboxes.count();
}
}
