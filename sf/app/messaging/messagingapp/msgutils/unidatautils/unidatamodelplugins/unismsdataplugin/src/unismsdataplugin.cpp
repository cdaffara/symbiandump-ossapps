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
 
//SYSTEM INCLUDES
#include <mtclreg.h>
#include <mtclbase.h>

//USER INCLUDES
#include "unismsdataplugin.h"
#include "unismsdataplugin_p.h"

//---------------------------------------------------------------
// UniSMSDataPlugin::UniSMSDataPlugin()
// @see header
//---------------------------------------------------------------
UniSMSDataPlugin::UniSMSDataPlugin(QObject* parent) :
    QObject(parent)
    {
    d_ptr = q_check_ptr(new UniSMSDataPluginPrivate(this));    
    }

//---------------------------------------------------------------
// UniSMSDataPlugin::~UniSMSDataPlugin()
// @see header
//---------------------------------------------------------------
UniSMSDataPlugin::~UniSMSDataPlugin()
{
    delete d_ptr;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::createInstance()
// @see header
//---------------------------------------------------------------
QObject* UniSMSDataPlugin::createInstance()
{
return new UniSMSDataPlugin();
}	

//---------------------------------------------------------------
// UniSMSDataPlugin::reset()
// @see header
//---------------------------------------------------------------
void UniSMSDataPlugin::reset()
{
    d_ptr->reset();
}

//---------------------------------------------------------------
// UniSMSDataPlugin::setMessageId()
// @see header
//---------------------------------------------------------------
int UniSMSDataPlugin::setMessageId(int mId)
{
    return d_ptr->setMessageId(mId);
}

//---------------------------------------------------------------
// UniSMSDataPlugin::body()
// @see header
//---------------------------------------------------------------
void UniSMSDataPlugin::body(QString& aBodyText)
{
    d_ptr->body(aBodyText);
}

//---------------------------------------------------------------
// UniSMSDataPlugin::messageSize()
// @see header
//---------------------------------------------------------------
qint32 UniSMSDataPlugin::messageSize()
{
    return d_ptr->messageSize();
}

//---------------------------------------------------------------
// UniSMSDataPlugin::toRecipientList()
// @see header
//---------------------------------------------------------------
void UniSMSDataPlugin::toRecipientList(
    ConvergedMessageAddressList& mAddressList)
{
    d_ptr->toRecipientList(mAddressList);
}

//---------------------------------------------------------------
// UniSMSDataPlugin::ccRecipientList()
// @see header
//---------------------------------------------------------------
void UniSMSDataPlugin::ccRecipientList(
    ConvergedMessageAddressList& /*mAddressList*/)
{

}

//---------------------------------------------------------------
// UniSMSDataPlugin::bccRecipientList()
// @see header
//---------------------------------------------------------------

void UniSMSDataPlugin::bccRecipientList(
    ConvergedMessageAddressList& /*mAddressList*/)
{

}

//---------------------------------------------------------------
// UniSMSDataPlugin::fromAddress()
// @see header
//---------------------------------------------------------------
void UniSMSDataPlugin::fromAddress(QString& messageAddress)
{
    d_ptr->fromAddress(messageAddress);
}

//---------------------------------------------------------------
// UniSMSDataPlugin::attachmentList()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniSMSDataPlugin::attachmentList()
{
    return QList<UniMessageInfo*> ();
}

//---------------------------------------------------------------
// UniSMSDataPlugin::messageType()
// @see header
//---------------------------------------------------------------
QString UniSMSDataPlugin::messageType()
{
    QString str("sms");
    return str;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::messagePriority()
// @see header
//---------------------------------------------------------------
MsgPriority UniSMSDataPlugin::messagePriority()
{
    return Normal;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::attachmentCount()
// @see header
//---------------------------------------------------------------
int UniSMSDataPlugin::attachmentCount()
{
    return KErrNone;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::hasAttachment()
// @see header
//---------------------------------------------------------------
bool UniSMSDataPlugin::hasAttachment()
{
    return false;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::objectCount()
// @see header
//---------------------------------------------------------------
int UniSMSDataPlugin::objectCount()
{
    return 0;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::objectList()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniSMSDataPlugin::objectList()
{
    return QList<UniMessageInfo*> ();
}

//---------------------------------------------------------------
// UniSMSDataPlugin::slideCount()
// @see header
//---------------------------------------------------------------
int UniSMSDataPlugin::slideCount()
{
    return 0;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::slideContent()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniSMSDataPlugin::slideContent(int /*slidenum */)
{
    return QList<UniMessageInfo*> ();
}

//---------------------------------------------------------------
// UniSMSDataPlugin::timeStamp()
// @see header
//---------------------------------------------------------------
QDateTime UniSMSDataPlugin::timeStamp()
{
    QDateTime retTimeStamp;
    int tStamp = d_ptr->timeStamp();
    retTimeStamp.setTime_t(tStamp);
    return retTimeStamp;
}

//---------------------------------------------------------------
// UniSMSDataPlugin::restore()
// @see header
//---------------------------------------------------------------
void UniSMSDataPlugin::restore(CBaseMtm& /*mtm*/)
{

}

//---------------------------------------------------------------
// UniSMSDataPlugin::subject()
// @see header
//---------------------------------------------------------------
QString UniSMSDataPlugin::subject()
{
  return QString();   
}

//---------------------------------------------------------------
// UniSMSDataPlugin::session()
// @see header
//---------------------------------------------------------------
CMsvSession* UniSMSDataPlugin::session()
{
    return d_ptr->session();
}


Q_EXPORT_PLUGIN2(unismsdataplugin, UniSMSDataPlugin)

