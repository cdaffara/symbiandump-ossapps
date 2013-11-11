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

//USER
#include "logscontact.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logslogger.h"
#include "logseventdata.h"
#include "logsdbconnector.h"
#include "logscommondata.h"

//SYSTEM
#include <QVariant>
#include <qcontactmanager.h>
#include <qcontactonlineaccount.h>
#include <qcontactphonenumber.h>
#include <xqappmgr.h>
#include <cntservicescontact.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::LogsContact(LogsEvent& event, LogsDbConnector& dbConnector)
  :  QObject(), 
     mDbConnector(dbConnector),
     mAiwRequest(0),
     mCurrentRequest(TypeLogsContactSave),
     mContactId(0),
     mSaveAsOnlineAccount(false)
{
    mNumber = event.getNumberForCalling();
    mContactId = event.contactLocalId();
    mContact = contact();
    if ( event.eventType() == LogsEvent::TypeVoIPCall &&
         event.logsEventData() && 
         !event.logsEventData()->remoteUrl().isEmpty() ) {
         mSaveAsOnlineAccount = true;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::LogsContact(const QString& number, 
                         LogsDbConnector& dbConnector,
                         unsigned int contactId)
  :  QObject(), 
     mDbConnector(dbConnector),
     mAiwRequest(0),
     mCurrentRequest(TypeLogsContactSave),
     mNumber(number),
     mContactId(contactId),
     mSaveAsOnlineAccount(false)
{
    mContact = contact();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::~LogsContact()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsContact::~LogsContact()" )
    delete mAiwRequest;
}
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsContact::RequestType LogsContact::allowedRequestType()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::allowedRequestType()" )
    LogsContact::RequestType type = TypeLogsContactSave;
    
    if ( isContactInPhonebook() ) {
        if (isContactGroup()) {
            type = TypeLogsContactOpenGroup;
            LOGS_QDEBUG( "logs [ENG] -> TypeLogsContactOpenGroup" )
        } else {
            type = TypeLogsContactOpen;
            LOGS_QDEBUG( "logs [ENG] -> TypeLogsContactOpenGroup" )
        }
    }
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsContact::allowedRequestType(): ", type )
    return type;
}

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
bool LogsContact::isContactRequestAllowed()
{
    return ( isContactInPhonebook() || !mNumber.isEmpty() );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::open()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::open()")
    bool ret = false;
    LogsContact::RequestType type = allowedRequestType();
    if ( type == TypeLogsContactOpen || type == TypeLogsContactOpenGroup) {
        mCurrentRequest = type;
        QString interface("com.nokia.symbian.IContactsView");
        QString operation("openContactCard(int)");
        if (type == TypeLogsContactOpenGroup) {
            operation = "openGroup(int)";
        }
        QList<QVariant> arguments;
        arguments.append( QVariant(mContactId) );
        ret = requestPhonebookService( interface, operation, arguments );
    }
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsContact::open(): ", ret )
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::addNew()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::save()" )
            
    bool ret = save("editCreateNew(QString,QString)");
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsContact::save(): ", ret )
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::updateExisting()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::updateExisting()" )
    
    bool ret = save("editUpdateExisting(QString,QString)");
    
    LOGS_QDEBUG( "logs [ENG] <- LogsContact::updateExisting()" )
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsContact::cancelServiceRequest()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsContact::cancelServiceRequest()" )
    delete mAiwRequest;
    mAiwRequest = 0;
    LOGS_QDEBUG( "logs [ENG] <- LogsContact::cancelServiceRequest()" )
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::isContactGroup()
{
    bool isGroup = (mContact.type() == QContactType::TypeGroup);    
    LOGS_QDEBUG_2( "logs [ENG] <-> LogsContact::isContactGroup(): ", isGroup )
    return isGroup;
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::save(const QString& operation)
{
    QList<QVariant> arguments;
 
    if ( !mNumber.isEmpty() ) {
        if ( mSaveAsOnlineAccount ){
            QString type = QContactOnlineAccount::DefinitionName;
            arguments.append( QVariant(type) );
        } else {
            QString type = QContactPhoneNumber::DefinitionName;
            arguments.append( QVariant(type) );
        }     
        arguments.append( QVariant(mNumber) );
    }
    
    bool ret(false);
    
    if ( arguments.count() == 2 ) {
        mCurrentRequest = TypeLogsContactSave;
        QString interface("com.nokia.symbian.IContactsEdit");
        ret = requestPhonebookService( interface, operation, arguments );        
    } else {
        LOGS_QDEBUG( "logs [ENG]  !No Caller ID, not saving the contact..")
    }
    
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::requestPhonebookService(const QString& interface, 
                             const QString& operation,
                             const QList<QVariant>& arguments)
{
    bool ret = false;
    cancelServiceRequest();
    XQApplicationManager appMng;
    mAiwRequest = appMng.create( interface, operation, true); // embedded 
    if (mAiwRequest) {
        connect(mAiwRequest, SIGNAL(requestOk(const QVariant&)), 
                this, SLOT(handleRequestCompleted(const QVariant&)));
        connect(mAiwRequest, SIGNAL(requestError(int,const QString&)), 
                this, SLOT(handleError(int,const QString&)));

        mAiwRequest->setArguments(arguments);
        mAiwRequest->setSynchronous(false);
        ret = mAiwRequest->send();
    }
    return ret;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsContact::handleError(int errorCode, const QString& errorMessage)
{
    LOGS_QDEBUG_4( "logs [ENG] <-> LogsContact::handleError(): ", errorCode,
            " ,msg: ", errorMessage)
    cancelServiceRequest();
}

// ----------------------------------------------------------------------------
// Phonebookservices define following return values:
// KCntServicesReturnValueContactNotModified = 0;
// KCntServicesReturnValueContactDeleted     = -1;
// KCntServicesReturnValueContactSaved       = 1;
// KCntServicesTerminated = -2;
// ----------------------------------------------------------------------------
//
void LogsContact::handleRequestCompleted(const QVariant& result)
{
    delete mAiwRequest;
    mAiwRequest = 0;
    bool retValOk = false;
    int serviceRetVal = result.toInt(&retValOk);
    LOGS_QDEBUG_3( "logs [ENG] -> LogsContact::handleRequestCompleted(), (retval, is_ok)", 
                   serviceRetVal, retValOk )
    
    bool modified = ( retValOk 
                      && (serviceRetVal == KCntServicesReturnValueContactSaved 
                      || serviceRetVal == KCntServicesReturnValueContactDeleted) );
    
    //If existing contact was modified, cached match for the contact should be 
    //cleaned up, since remote party info might have been changed.
    //However, if remote party info is taken from symbian DB, it won't be 
    //updated
    bool clearCached = ( mCurrentRequest == TypeLogsContactOpen
                         || mCurrentRequest == TypeLogsContactOpenGroup ); 
    if ( modified ) {
        mContact = contact();
        mDbConnector.updateDetails(clearCached);
    }
    
    if ( mCurrentRequest == TypeLogsContactOpen ) {
        emit openCompleted(modified);
    } else if ( mCurrentRequest == TypeLogsContactSave ) {
        emit saveCompleted(modified);
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
QContact LogsContact::contact()
{
    if ( mContactId ) {     
        return LogsCommonData::getInstance().contactManager().contact( mContactId ); 
    } else {
        return QContact();
    }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::isContactInPhonebook()
{
    return ( mContactId && mContactId == mContact.localId() );
}

// End of file

