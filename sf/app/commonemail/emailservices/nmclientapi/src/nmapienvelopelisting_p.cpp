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
NmApiEnvelopeListingPrivate::NmApiEnvelopeListingPrivate(const quint64 folderId, const quint64 mailboxId, QObject *parent)
:QObject(parent),
mFolderId(folderId), 
mMailboxId(mailboxId), 
mIsRunning(false),
mEngine(NULL)
{
    NM_FUNCTION;
    mEngine = NmApiEngine::instance();
    Q_CHECK_PTR(mEngine);
}

NmApiEnvelopeListingPrivate::~NmApiEnvelopeListingPrivate()
{
    NM_FUNCTION;
    NmApiEngine::releaseInstance(mEngine);
}

/*! 
   \brief Fills envelopes to the input parameter.
   
    Caller gets ownership of envelopes. Returns true if results were available.
 */
bool NmApiEnvelopeListingPrivate::envelopes(QList<EmailClientApi::NmApiMessageEnvelope> &envelopes)
{
    NM_FUNCTION;
    bool ret(mIsRunning);
    envelopes.clear();
    while (!mEnvelopes.isEmpty()) {
        envelopes << mEnvelopes.takeFirst();
    }
    mIsRunning = false;
    return ret;
}

/*!
   \brief It fetches envelopes from engine. 
   
   Because it uses NmApiMessageEnvelope with sharedData we don't need care about release memory.
   
   \return Count of envelopes 
 */
qint32 NmApiEnvelopeListingPrivate::listEnvelopes()
{
    NM_FUNCTION;
    mIsRunning = true;
    mEnvelopes.clear();
    mEngine->listEnvelopes(mMailboxId, mFolderId, mEnvelopes);
    return mEnvelopes.count();
}
/*!
   \brief Return info if listing is running
 */
bool NmApiEnvelopeListingPrivate::isRunning() const
{
    return mIsRunning;
}
/*!
   \brief Clears list of envelopes.
 */
void NmApiEnvelopeListingPrivate::cancel() 
{
    mIsRunning = false;
    mEnvelopes.clear();
}
}

