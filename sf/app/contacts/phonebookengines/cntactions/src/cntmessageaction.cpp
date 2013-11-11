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

#include "cntmessageaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
CntMessageAction::CntMessageAction() : 
    CntAction("message")
{
}

CntMessageAction::~CntMessageAction()
{
}

CntMessageAction* CntMessageAction::clone() const
{
	 return new CntMessageAction();
}

bool CntMessageAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    if (detail.definitionName() == QContactPhoneNumber::DefinitionName 
        && !static_cast<QContactPhoneNumber>(detail).subTypes().isEmpty())
    {
        return (static_cast<QContactPhoneNumber>(detail).subTypes().first() == QContactPhoneNumber::SubTypeMobile);
    }
    else
    {
        return false;
    }
}

QList<QContactDetail> CntMessageAction::supportedDetails(const QContact& contact) const
{
    QList<QContactDetail> details = contact.details(QContactPhoneNumber::DefinitionName);
    QList<QContactDetail> supportedDetails;
    for (int i = 0; i < details.count(); i++)
    {
        if (!static_cast<QContactPhoneNumber>(details[i]).subTypes().isEmpty() 
            && static_cast<QContactPhoneNumber>(details[i]).subTypes().first() == QContactPhoneNumber::SubTypeMobile)
        {
            supportedDetails.append(details[i]);
        }
    } 
    return supportedDetails;
}

void CntMessageAction::performAction()
{
    QList<QVariant> data;
    QVariant retValue;
    
    delete m_request;
    m_request = NULL;
        
    //QContactType == TypeGroup
    if (QContactType::TypeGroup == m_contact.type()) {
        QString interface("com.nokia.symbian.IMessageSend");
        QString operation("send(QVariantMap,QString)");
        m_request = m_AppManager.create(interface, operation, true); // embedded
        if (m_request==NULL) {
            emitResult(GeneralError, retValue);
            return;
        }
        
        QVariantMap recipientMap;
        QVariant value = m_data.value("message");
        if (value.canConvert<QVariantMap>()) {
            recipientMap = value.toMap();
        }
        if (!recipientMap.isEmpty()) {
            data.append(recipientMap); //recipients
            data.append(QString()); //body text
            
            m_request->setArguments(data);
            m_request->setSynchronous(true);
            m_request->send(retValue);
            emitResult(m_request->lastError(), retValue);
        }
        else {
            emitResult(GeneralError, retValue);
        }
    }
    //QContactType == TypeContact
    //detail exist use it
    else if (m_detail.definitionName() == QContactPhoneNumber::DefinitionName) {
        QString interface("com.nokia.symbian.IMessageSend");
        QString operation("send(QString,qint32,QString)");
        m_request = m_AppManager.create(interface, operation, true); // embedded
        if (m_request==NULL) {
            emitResult(GeneralError, retValue);
            return;
        }
        
        const QContactPhoneNumber &phoneNumber = static_cast<const QContactPhoneNumber &>(m_detail);
        data << phoneNumber.number() << m_contact.localId() << m_contact.displayLabel();
        
        m_request->setArguments(data);
        m_request->setSynchronous(true);
        m_request->send(retValue);
        emitResult(m_request->lastError(), retValue);
    }
    else {
        emitResult(GeneralError, retValue);
    }
}



