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

/*!
   constructor for NmEmailService
 */
NmApiEmailService::NmApiEmailService(QObject *parent) :
    QObject(parent), mEngine(NULL), mIsRunning(false)
{
    NM_FUNCTION;
}

/*!
   destructor for NmApiEmailService
 */
NmApiEmailService::~NmApiEmailService()
{
    NM_FUNCTION;
    
    if (mEngine) {
        uninitialise();
    }
}

/*!
   gets mail message envelope by id (see also NmEventNotifier)
 */
bool NmApiEmailService::getEnvelope(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 envelopeId,
    EmailClientApi::NmApiMessageEnvelope &envelope)
{
    NM_FUNCTION;
    
    if (!mEngine) {
        return false;
    }
    return mEngine->getEnvelopeById(mailboxId, folderId, envelopeId, envelope);
}

/*!
    gets mailbox folder by id (see also NmEventNotifier)
 */
bool NmApiEmailService::getFolder(
    const quint64 mailboxId,
    const quint64 folderId,
    EmailClientApi::NmApiFolder &mailboxFolder)
{
    NM_FUNCTION;

    bool found = false;

    if (mEngine) {
        found = mEngine->getFolderById(mailboxId, folderId, mailboxFolder);
    }

    return found;
}

/*!
    gets mailbox info by id (see also NmEventNotifier)
 */
bool NmApiEmailService::getMailbox(const quint64 mailboxId, EmailClientApi::NmApiMailbox &mailboxInfo)
{
    NM_FUNCTION;
    
    if (!mEngine) {
        return false;
    }
    return mEngine->getMailboxById(mailboxId, mailboxInfo);
}

/*!
   gets mail message envelope by id (see also NmEventNotifier)
 */
bool NmApiEmailService::getMessage(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 messageId,
    EmailClientApi::NmApiMessage &message)
{
    NM_FUNCTION;
    
    if (!mEngine) {
        return false;
    }
    return mEngine->getMessageById(mailboxId, folderId, messageId, message);
}

/*!
   Initialises email service. this must be called and initialised signal received 
   before services of the library are used.
 */
void NmApiEmailService::initialise()
{
    NM_FUNCTION;
    
    if (!mEngine) {
        mEngine = NmApiEngine::instance();
    }

    if (mEngine) {
        mIsRunning = true;
        emit initialized(true);
    }
    else {
        emit initialized(false);
    }
}

/*!
    frees resources.
 */
void NmApiEmailService::uninitialise()
{
    NM_FUNCTION;
    
    NmApiEngine::releaseInstance(mEngine);
    mIsRunning = false;
}

/*!
   returns isrunning flag value
 */
bool NmApiEmailService::isRunning() const
{
    NM_FUNCTION;
    
    return mIsRunning;
}

}

