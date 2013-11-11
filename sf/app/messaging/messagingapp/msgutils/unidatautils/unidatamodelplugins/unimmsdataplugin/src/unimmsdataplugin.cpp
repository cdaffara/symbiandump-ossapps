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

#include "debugtraces.h"

#include "unimmsdataplugin.h"
#include "unimmsdataplugin_p.h"

UniMMSDataPlugin::UniMMSDataPlugin(QObject* parent) :
    QObject(parent)
{
    d_ptr = q_check_ptr (new UniMMSDataPluginPrivate());
}

UniMMSDataPlugin::~UniMMSDataPlugin()
{
    delete d_ptr;
}

QObject* UniMMSDataPlugin::createInstance()
{
return new UniMMSDataPlugin();
}

int UniMMSDataPlugin::setMessageId(int messageId)
{
    return d_ptr->setMessageId(messageId);
}

void UniMMSDataPlugin::reset()
{
    d_ptr->reset();
}

void UniMMSDataPlugin::body(QString& aBodyText)
{
    d_ptr->body(aBodyText);
}

int UniMMSDataPlugin::messageSize()
{
    return d_ptr->messageSize();
}

void UniMMSDataPlugin::toRecipientList(
                                       ConvergedMessageAddressList& mAddressList)
{
    d_ptr->toRecipientList(mAddressList);
}

void UniMMSDataPlugin::ccRecipientList(
                                       ConvergedMessageAddressList& mAddressList)
{
    d_ptr->ccRecipientList(mAddressList);
}

void UniMMSDataPlugin::bccRecipientList(
                                        ConvergedMessageAddressList& mAddressList)
{
    d_ptr->bccRecipientList(mAddressList);
}

void UniMMSDataPlugin::fromAddress(QString& messageAddress)
{
    d_ptr->fromAddress(messageAddress);
}

UniMessageInfoList UniMMSDataPlugin::attachmentList()
{
    return d_ptr->attachmentList();
}

QString UniMMSDataPlugin::messageType()
{
    return d_ptr->messageType();
}

MsgPriority UniMMSDataPlugin::messagePriority()
{
    return d_ptr->messagePriority();
}

int UniMMSDataPlugin::attachmentCount()
{
    return d_ptr->attachmentCount();
}

bool UniMMSDataPlugin::hasAttachment()
{
    return d_ptr->hasAttachment();
}

int UniMMSDataPlugin::objectCount()
{
    return d_ptr->objectCount();
}

UniMessageInfoList UniMMSDataPlugin::objectList()
{
    return d_ptr->objectList();
}

int UniMMSDataPlugin::slideCount()
{
    return d_ptr->slideCount();
}

UniMessageInfoList UniMMSDataPlugin::slideContent(int slidenum)
{
    return d_ptr->slideContent(slidenum);
}

QDateTime UniMMSDataPlugin::timeStamp()
{
    QDateTime retTimeStamp;
    int tStamp = d_ptr->timeStamp();
    retTimeStamp.setTime_t(tStamp);
    return retTimeStamp;
}

void UniMMSDataPlugin::restore(CBaseMtm& mtm)
{
    QDEBUG_WRITE("Inside restore");
    int error;    
    TRAP(error,d_ptr->restoreL(mtm));    
    QDEBUG_WRITE_FORMAT("Exiting restore with error= ",error);
    qt_symbian_throwIfError(error);
}

QString UniMMSDataPlugin::subject()
{
    QString subject;
    TRAPD(err, subject =d_ptr->subjectL());
    return  subject;
}

CMsvSession* UniMMSDataPlugin::session()
{
    return d_ptr->session();
    
}


Q_EXPORT_PLUGIN2(unimmsdataplugin, UniMMSDataPlugin)

