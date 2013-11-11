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
*               THIS IS DEPRICATED API - DO NOT USE!
*               (use DialService instead)
*
*/

#ifndef DIALSERVICEDEPRICATED_H
#define DIALSERVICEDEPRICATED_H

#include <QObject>
#include <QString>
#include <xqserviceprovider.h>
#include "mpecallcontrolif.h"
#include "mpecallsettersif.h"

// FORWARD DECLARATIONS
class XQSettingsManager;
class DialServiceDepricated : public XQServiceProvider
{
    Q_OBJECT
public:
    DialServiceDepricated(MPECallControlIF &call, MPECallSettersIF &parameters, QObject *parent = 0);
    ~DialServiceDepricated();
    
public slots:
    
    //deprecated
    int dial(const QString& number);
    
    //deprecated
    int dial(const QString& number, int contactId);
    
    //deprecated
    void dialVideo(const QString& number);

    //deprecated
    void dialVideo(const QString& number, int contactId);
    
    //deprecated
    void dialVoip(const QString& address);
    
    //deprecated
    void dialVoip(const QString& address, int contactId);
    
    //deprecated
    void dialVoipService(const QString& address, int serviceId);
    
    //deprecated
    void dialVoipService(const QString& address, int serviceId, int contactId);
    
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


private:
    MPECallControlIF  &m_call;
    MPECallSettersIF  &m_parameters;
    XQSettingsManager *m_settingsManager;
};

#endif // DIALERSERVICEDEPRICATED_H
