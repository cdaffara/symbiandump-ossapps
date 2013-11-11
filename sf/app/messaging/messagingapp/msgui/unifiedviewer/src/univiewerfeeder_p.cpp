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
 * Description: Feeder Private class for unified viewer. Fetches data from the
 *              message store for the given message id.
 *
 */
#include "univiewerfeeder_p.h"
// SYSTEM INCLUDES
#include <msvstd.h>
#include <msvids.h>

// USER INCLUDES
#include "nativemessageconsts.h"
#include "univiewerfeeder.h"
#include "unidatamodelloader.h"
#include "msgcontacthandler.h"
#include "debugtraces.h"
#include "msgbiouids.h"

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::UniViewerFeederPrivate
// @see header file
// ---------------------------------------------------------------------------
UniViewerFeederPrivate::UniViewerFeederPrivate(qint32 msgId,
                                               UniViewerFeeder* feeder) :
    q_ptr(feeder), mSlideCount(0), mSession(NULL)
{
    TRAP_IGNORE(initL(msgId));
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::~UniViewerFeederPrivate
// @see header file
// ---------------------------------------------------------------------------
UniViewerFeederPrivate::~UniViewerFeederPrivate()
{
    q_ptr = NULL;
    clearContent();
    if(mPluginLoader)
    {
        delete mPluginLoader;
        mPluginLoader = NULL;
    }
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::initL
// Symbian specific constructions
// ---------------------------------------------------------------------------
//
void UniViewerFeederPrivate::initL(qint32 msgId)
{
    QDEBUG_WRITE("UniViewerFeederPrivate::initL start");
    TMsvId serviceId = KMsvNullIndexEntryId;
    mPluginInterface = NULL;
    mMsgId = msgId;
    mPluginLoader = new UniDataModelLoader();
    mPluginInterface = mPluginLoader->getDataModelPlugin(ConvergedMessage::Sms);

    mSession = mPluginInterface->session();
    mSession->GetEntry(msgId, serviceId, mEntry);
    QDEBUG_WRITE("UniViewerFeederPrivate::initL end");
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::msgType
// Returns the message type.
// ---------------------------------------------------------------------------
qint32 UniViewerFeederPrivate::msgType()
{
    return mEntry.iMtm.iUid;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::isIncoming
// Returns if it is a incoming message.
// ---------------------------------------------------------------------------
bool UniViewerFeederPrivate::isIncoming()
{
    if (mEntry.Parent() == KMsvGlobalInBoxIndexEntryId)
    {
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::priority
// Returns the message priority.
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::priority()
{
    if (mEntry.Priority() == EMsvHighPriority)
    {
        return ConvergedMessage::High;
    }
    else if (mEntry.Priority() == EMsvLowPriority)
    {
        return ConvergedMessage::Low;
    }
    return ConvergedMessage::Normal;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::subject
// Returns the message subject.
// ---------------------------------------------------------------------------
QString UniViewerFeederPrivate::subject()
{
    return mPluginInterface->subject();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::subject
// Returns the message subject.
// ---------------------------------------------------------------------------

int UniViewerFeederPrivate::sendingState()
{
    return mEntry.SendingState();
}
// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::timeStamp
// Returns the time stamp
// ---------------------------------------------------------------------------
QDateTime UniViewerFeederPrivate::timeStamp()
{

    return mPluginInterface->timeStamp();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::fetchDetailsL
// Fetches message details from the store.
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::fetchDetails()
{
    QDEBUG_WRITE("UniViewerFeederPrivate fetchDetailsL : SMS start");
    if (msgType() == KSenduiMtmSmsUidValue || (msgType() == KSenduiMtmBioUidValue
        && mEntry.iBioType == KMsgBioNokiaServiceMessage.iUid)) {
        QString body;
        mPluginInterface->body(body);
        q_ptr->emitMsgBody(body);
    }
    QDEBUG_WRITE("UniViewerFeederPrivate fetchDetailsL : SMS END");
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::hasAttachments
// @see header file
// ---------------------------------------------------------------------------
bool UniViewerFeederPrivate::hasAttachments()
{
    return mPluginInterface->hasAttachment();
}
// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::attachmentsList
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeederPrivate::attachmentsList()
{
    return mPluginInterface->attachmentList();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::attachmentCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::attachmentCount()
{
    return mPluginInterface->attachmentCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::objectsList
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeederPrivate::objectsList()
{
    return mPluginInterface->objectList();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::objectCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::objectCount()
{
    return mPluginInterface->objectCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::slideCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::slideCount()
{
    return mSlideCount;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::slideContent
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeederPrivate::slideContent(int slidenum)
{
    return mPluginInterface->slideContent(slidenum);
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::updateContent
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::updateContent(qint32 msgId)
{
  
    if (msgId != mMsgId)
    {
        mMsgId = msgId;
        TMsvId serviceId = KMsvNullIndexEntryId;
        mSession->GetEntry(msgId, serviceId, mEntry);
        clearContent();
    }

    if (msgType() == KSenduiMtmSmsUidValue)
    {
        mPluginInterface = mPluginLoader->getDataModelPlugin(ConvergedMessage::Sms);
        mPluginInterface->setMessageId(msgId);
    }

    else if (msgType() == KSenduiMtmMmsUidValue) {
        mPluginInterface = mPluginLoader->getDataModelPlugin(ConvergedMessage::Mms);
        mPluginInterface->setMessageId(msgId);
        mSlideCount = mPluginInterface->slideCount();

    }
    else if (msgType() == KSenduiMtmBioUidValue && mEntry.iBioType
        == KMsgBioNokiaServiceMessage.iUid) {
        mPluginInterface = mPluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
        mPluginInterface->setMessageId(msgId);
    }

    mPluginInterface->toRecipientList(mToAddressList);
    mPluginInterface->ccRecipientList(mCcAddressList);
    mPluginInterface->bccRecipientList(mBccAddressList);
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::toAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeederPrivate::toAddressList()
{
    QString alias = QString();
    int count;
    for (int i = 0; i < mToAddressList.count(); ++i) {
        MsgContactHandler::resolveContactDisplayName(
		mToAddressList.at(i)->address(), alias, count);
        mToAddressList.at(i)->setAlias(alias);
        alias.clear();
    }
    return mToAddressList;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::ccAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeederPrivate::ccAddressList()
{
    QString alias = QString();
    int count;
    for (int i = 0; i < mCcAddressList.count(); ++i) {
        MsgContactHandler::resolveContactDisplayName(
        mCcAddressList.at(i)->address(), alias, count);
        mCcAddressList.at(i)->setAlias(alias);
        alias.clear();

    }
    return mCcAddressList;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::bccAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeederPrivate::bccAddressList()
{
    QString alias = QString();
    int count;
    for (int i = 0; i < mBccAddressList.count(); ++i)
    {
        MsgContactHandler::resolveContactDisplayName(
            mBccAddressList.at(i)->address(),
            alias,
            count);
        mBccAddressList.at(i)->setAlias(alias);
        alias.clear();
    }
    return mBccAddressList;
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::recipientCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::recipientCount()
{
    return mToAddressList.count() + mCcAddressList.count() + mBccAddressList.count();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::messageSize
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeederPrivate::messageSize()
{
    return mPluginInterface->messageSize();
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::fromAddressAndAlias
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::fromAddressAndAlias(QString& from, QString& alias)
{
    mPluginInterface->fromAddress(from);
    int count;
    MsgContactHandler::resolveContactDisplayName(from, alias, count);
}

// ---------------------------------------------------------------------------
// UniViewerFeederPrivate::clearContent
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeederPrivate::clearContent()
{
    for (int i = 0; i < mToAddressList.count(); ++i)
    {
        delete mToAddressList.at(i);
    }
    mToAddressList.clear();

    for (int i = 0; i < mCcAddressList.count(); ++i)
    {
        delete mCcAddressList.at(i);
    }
    mCcAddressList.clear();

    for (int i = 0; i < mBccAddressList.count(); ++i)
    {
        delete mBccAddressList.at(i);
    }
    mBccAddressList.clear();
}

// EOF
