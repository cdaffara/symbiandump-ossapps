/*!
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
* Description:  Dial API to be used through Qt Highway.
*
*
*            Example usage:
*            QString service("phoneui");
*            QString interface("com.nokia.symbian.ICallDial");
*            QString operation("dial(QString)"); // choose appropriate operation
*            XQApplicationManager appManager;
*            QScopedPointer<XQAiwRequest> request(appManager.create(service, interface, operation, false));
*            if (request == NULL) {
*                //Service not found 
*            }
*            QList<QVariant> args;
*            args << QString("0501234567"); // number or address
*            // add contactId and serviceId when needed by operation
*            request->setArguments(args);
*            QVariant retValue;
*            if (request->send(retValue)) {
*               //error
*            }
*            
*/

#ifndef DIALSERVICE_H
#define DIALSERVICE_H

#include <QObject>
#include <QString>
#include <xqserviceprovider.h>
#include "mpecallcontrolif.h"
#include "mpecallsettersif.h"

// FORWARD DECLARATIONS
class XQSettingsManager;

class DialService : public XQServiceProvider
{
    Q_OBJECT
public:
    DialService(MPECallControlIF &call, MPECallSettersIF &parameters, QObject *parent = 0);
    ~DialService();
    
public slots:
    /*!
        \fn dial(const QString& number)
        
        This method makes a cellular switched dial command to
        Phone Application. It is intended to be used via Qt Highway.
    */
    int dial(const QString& number);
    
    /*!
        \fn dial(const QString& number)
        
        This method makes a cellular switched dial command to
        Phone Application. It is intended to be used via Qt Highway.
        Caller's name is shown according to the given Phonebook contact
        identifier.
     
    */
    int dial(const QString& number, int contactId);
    
    /*!
        \fn dialVideo(const QString& number)
        
        This method makes a video call dial command to
        Phone Application. It is intended to be used via Qt Highway.

    */
    int dialVideo(const QString& number);

    /*!
        \fn dial(const QString& number)
        
        This method makes a video call dial command to
        Phone Application. It is intended to be used via Qt Highway.
        Caller's name is shown according to the given Phonebook contact
        identifier.
    */
    int dialVideo(const QString& number, int contactId);
    
    /*!
        \fn dialVoip(const QString& address)
        
        This method makes a voip call dial command to
        Phone Application. It is intended to be used via Qt Highway.
    */
    int dialVoip(const QString& address);
    
    /*!
        \fn dialVoip(const QString& address, int contactId)
        
        This method makes a voip call dial command to
        Phone Application. It is intended to be used via Qt Highway.
        Caller's name is shown according to the given Phonebook contact
        identifier.

    */
    int dialVoip(const QString& address, int contactId);
    
    /*!
        \fn dialVoipService(const QString& address, int contactId)
        
        This method makes a voip call dial command to
        Phone Application. It is intended to be used via Qt Highway.
        Call is made by given service (id).
    */
    int dialVoipService(const QString& address, int serviceId);
    
    /*!
        \fn dialVoipService(const QString& address, int serviceId, int contactId)
        
        This method makes a voip call dial command to
        Phone Application. It is intended to be used via Qt Highway.
        Call is made by given service (id).
        Caller's name is shown according to the given Phonebook contact
        identifier.
    */
    int dialVoipService(const QString& address, int serviceId, int contactId);
    
private:
	
    /*!
        \fn QString modifyPhoneNumber(QString& number)
        
        Returns a modified phone number string. Strips white spaces, makes prefix changes etc. 
    */
    QString modifyPhoneNumber(const QString &number) Q_REQUIRED_RESULT;
	
    /*!
        \fn QString simplified(QString& number)
        
        Returns a string that has whitespaces, '(', ')', '-', '[', and ']' chars removed 
    */
    static QString simplified(const QString &number) Q_REQUIRED_RESULT;
    
    /*!
        \fn QString japanPrefixModifications(QString& number)
        
        Returns a string that has japan specific configuration modifications made 
    */
    QString japanPrefixModifications(const QString &number) Q_REQUIRED_RESULT;
	
	
    /*!
        \fn bool hasCapability()
        
        Checks if the client has the required capabilities
    */
    bool hasCapability();


private:
    MPECallControlIF  &m_call;
    MPECallSettersIF  &m_parameters;
    XQSettingsManager *m_settingsManager;
};

#endif // DIALERSERVICE_H
