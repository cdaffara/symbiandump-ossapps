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
#include <xqconversions.h>

//USER INCLUDES
#include "unibiomessagedataplugin.h"
#include "unibiomessagedataplugin_p.h"

// UniBioMessageDataPlugin::UniBioMessageDataPlugin()
// @see header
//---------------------------------------------------------------
UniBioMessageDataPlugin::UniBioMessageDataPlugin(QObject* parent) :
    QObject(parent)
    {
    d_ptr = q_check_ptr(new UniBioMessageDataPluginPrivate(this));
    }

// UniBioMessageDataPlugin::~UniBioMessageDataPlugin()
// @see header
//---------------------------------------------------------------
UniBioMessageDataPlugin::~UniBioMessageDataPlugin()
{
    delete d_ptr;
}

// UniBioMessageDataPlugin::createInstance()
// @see header
//---------------------------------------------------------------
QObject* UniBioMessageDataPlugin::createInstance()
{
return new UniBioMessageDataPlugin();
}	

// UniBioMessageDataPlugin::reset()
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPlugin::reset()
{
    d_ptr->reset();
}

// UniBioMessageDataPlugin::setMessageId()
// @see header
//---------------------------------------------------------------
int UniBioMessageDataPlugin::setMessageId(int mId)
{
    return d_ptr->setMessageId(mId);
}

// UniBioMessageDataPlugin::body()
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPlugin::body(QString& aBodyText)
{
    d_ptr->body(aBodyText);
}

// UniBioMessageDataPlugin::messageSize()
// @see header
//---------------------------------------------------------------
qint32 UniBioMessageDataPlugin::messageSize()
{
    return d_ptr->messageSize();
}

// UniBioMessageDataPlugin::toRecipientList()
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPlugin::toRecipientList(
    ConvergedMessageAddressList& mAddressList)
{
    d_ptr->toRecipientList(mAddressList);
}

// UniBioMessageDataPlugin::ccRecipientList()
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPlugin::ccRecipientList(ConvergedMessageAddressList& /*mAddressList*/)
{

}

// UniBioMessageDataPlugin::bccRecipientList()
// @see header
//---------------------------------------------------------------

void UniBioMessageDataPlugin::bccRecipientList(ConvergedMessageAddressList& /*mAddressList*/)
{

}

// UniBioMessageDataPlugin::fromAddress()
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPlugin::fromAddress(QString& messageAddress)
{
    d_ptr->fromAddress(messageAddress);
}

// UniBioMessageDataPlugin::attachmentList()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniBioMessageDataPlugin::attachmentList()
{
    RFile file;
    QT_TRAP_THROWING(file = d_ptr->attachmentL());

    if(attachmentCount() == 0)
    {
        file.Close();
        return QList<UniMessageInfo*> ();
    }
     
    UniMessageInfoList attlist;

    QString path;
    QString mimetype;
    int size = 0;

    TFileName fullName;
    qt_symbian_throwIfError(file.FullName(fullName));
    qt_symbian_throwIfError(file.Size(size));

    path = XQConversions::s60DescToQString(*fullName.AllocL());

    UniMessageInfo *msgobj = new UniMessageInfo(path, size, mimetype);
    attlist << msgobj;
    file.Close();
    return attlist;
}

// UniBioMessageDataPlugin::messageType()
// @see header
//---------------------------------------------------------------
QString UniBioMessageDataPlugin::messageType()
{
    QString str("biomessage");
    return str;
}

// UniBioMessageDataPlugin::messagePriority()
// @see header
//---------------------------------------------------------------
MsgPriority UniBioMessageDataPlugin::messagePriority()
{
    return d_ptr->messagePriority();
}

// UniBioMessageDataPlugin::attachmentCount()
// @see header
//---------------------------------------------------------------
int UniBioMessageDataPlugin::attachmentCount()
{
    return d_ptr->attachmentCount();
}

// UniBioMessageDataPlugin::hasAttachment()
// @see header
//---------------------------------------------------------------
bool UniBioMessageDataPlugin::hasAttachment()
{
    if(attachmentCount() > 0)
        return true;
    else
        return false; 
        
}

// UniBioMessageDataPlugin::objectCount()
// @see header
//---------------------------------------------------------------
int UniBioMessageDataPlugin::objectCount()
{
    return 0;
}

// UniBioMessageDataPlugin::objectList()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniBioMessageDataPlugin::objectList()
{
    return QList<UniMessageInfo*> ();
}

// UniBioMessageDataPlugin::slideCount()
// @see header
//---------------------------------------------------------------
int UniBioMessageDataPlugin::slideCount()
{
    return 0;
}

// UniBioMessageDataPlugin::slideContent()
// @see header
//---------------------------------------------------------------
UniMessageInfoList UniBioMessageDataPlugin::slideContent(int /*slidenum*/)
{
    return QList<UniMessageInfo*> ();
}

// UniBioMessageDataPlugin::timeStamp()
// @see header
//---------------------------------------------------------------
QDateTime UniBioMessageDataPlugin::timeStamp()
{
    QDateTime retTimeStamp;
    int tStamp = d_ptr->timeStamp();
    retTimeStamp.setTime_t(tStamp);
    return retTimeStamp;
}

//---------------------------------------------------------------
// UniBioMessageDataPlugin::restore()
// @see header
//---------------------------------------------------------------
void UniBioMessageDataPlugin::restore(CBaseMtm& /*mtm*/)
{

}

//---------------------------------------------------------------
// UniBioMessageDataPlugin::subject()
// @see header
//---------------------------------------------------------------
QString UniBioMessageDataPlugin::subject()
{
  return QString();   
}


//---------------------------------------------------------------
// UniBioMessageDataPlugin::session()
// @see header
//---------------------------------------------------------------
CMsvSession* UniBioMessageDataPlugin::session()
{
    return d_ptr->session();
}



Q_EXPORT_PLUGIN2(UniBioMessageDataPlugin, UniBioMessageDataPlugin)

