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
 * Description: Feeder class for unified viewer. Fetches data from the
 *              message store for the given message store id.
 *
 */

#include "univiewerfeeder.h"
#include "univiewerfeeder_p.h"

//---------------------------------------------------------------------------
// UniViewerFeeder::UniViewerFeeder
// Default Constructor.
//---------------------------------------------------------------------------
UniViewerFeeder::UniViewerFeeder(qint32 msgId,
    QObject *parent) :
    QObject(parent)
{
    d_ptr = new UniViewerFeederPrivate(msgId, this);
}

//---------------------------------------------------------------------------
// UniViewerFeeder::~UniViewerFeeder()
// Desctuctor
//---------------------------------------------------------------------------
UniViewerFeeder::~UniViewerFeeder()
{
    if (d_ptr)
    {
        delete d_ptr;
        d_ptr = NULL;
    }
}

//---------------------------------------------------------------------------
// UniViewerFeeder::updateContent()
//
//---------------------------------------------------------------------------
void UniViewerFeeder::updateContent(qint32 msgId)
{
    d_ptr->updateContent(msgId);
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::emitMsgBody
// Emits msgBody signal with msgBody as argument
// ---------------------------------------------------------------------------
void UniViewerFeeder::emitMsgBody(QString body)
{
    emit msgBody(body);
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::fetchDetails
// Fetches message details from the store.
// ---------------------------------------------------------------------------
void UniViewerFeeder::fetchDetails()
{
    d_ptr->fetchDetails();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::msgType
// Returns the message type.
// ---------------------------------------------------------------------------
qint32 UniViewerFeeder::msgType()
{
    return d_ptr->msgType();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::timeStamp
// Returns the time stamp.
// ---------------------------------------------------------------------------
QDateTime UniViewerFeeder::timeStamp()
{
    return d_ptr->timeStamp();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::hasAttachments
// @see header file
// ---------------------------------------------------------------------------
bool UniViewerFeeder::hasAttachments()
{
    return d_ptr->hasAttachments();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::attachmentsList
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeeder::attachmentsList()
{
    return d_ptr->attachmentsList();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::attachmentCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::attachmentCount()
{
    return d_ptr->attachmentCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::objectsList
// @see header file
// ---------------------------------------------------------------------------

UniMessageInfoList UniViewerFeeder::objectsList()
{
    return d_ptr->objectsList();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::objectCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::objectCount()
{
    return d_ptr->objectCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::slideCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::slideCount()
{
    return d_ptr->slideCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::slideContent
// @see header file
// ---------------------------------------------------------------------------
UniMessageInfoList UniViewerFeeder::slideContent(int slidenum)
{
    return d_ptr->slideContent(slidenum);
}

ConvergedMessageAddressList UniViewerFeeder::toAddressList()
{
    return d_ptr->toAddressList();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::ccAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeeder::ccAddressList()
{
    return d_ptr->ccAddressList();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::bccAddressList
// @see header file
// ---------------------------------------------------------------------------
ConvergedMessageAddressList UniViewerFeeder::bccAddressList()
{
    return d_ptr->bccAddressList();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::recipientCount
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::recipientCount()
{
    return d_ptr->recipientCount();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::messageSize
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::messageSize()
{
    return d_ptr->messageSize();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::clearContent
// @see header file
//
void UniViewerFeeder::clearContent()
{
    return d_ptr->clearContent();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::fromAddressAndAlias
// @see header file
// ---------------------------------------------------------------------------
void UniViewerFeeder::fromAddressAndAlias(QString& from, QString& alias)
{
    return d_ptr->fromAddressAndAlias(from, alias);
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::isIncoming
// @see header file
// ---------------------------------------------------------------------------
bool UniViewerFeeder::isIncoming()
{
    return d_ptr->isIncoming();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::priority
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::priority()
{
    return d_ptr->priority();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::subject
// @see header file
// ---------------------------------------------------------------------------
QString UniViewerFeeder::subject()
{
    return d_ptr->subject();
}

// ---------------------------------------------------------------------------
// UniViewerFeeder::sendingState
// @see header file
// ---------------------------------------------------------------------------
int UniViewerFeeder::sendingState()
{
    return d_ptr->sendingState();
}
// EOF
