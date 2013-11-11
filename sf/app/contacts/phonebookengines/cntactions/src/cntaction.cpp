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
#include "cntaction.h"

#include <xqservicerequest.h>
#include <xqappmgr.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>
#include <QTimer>
#include <QDebug>

CntAction::CntAction(const QString &actionName) :
        m_actionName(actionName),
        m_vendorName("symbian"),
        m_implementationVersion(1),
        m_result(), 
        m_contact(),
        m_detail(),
        m_request(NULL)
{
}
        
CntAction::~CntAction()
{
    delete m_request;
    m_request = NULL;
}

QVariantMap CntAction::metaData() const
{
    return QVariantMap();
}

QContactFilter CntAction::contactFilter(const QVariant& value) const
{
    Q_UNUSED(value);
    
    QContactUnionFilter unionFilter;
   
    QContactDetailFilter landlineFilter;
    landlineFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    landlineFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeLandline));
    unionFilter << landlineFilter;
    
    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    unionFilter << mobileFilter;
        
    QContactDetailFilter pagerFilter;
    pagerFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    pagerFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypePager));
    unionFilter << pagerFilter;
        
    QContactDetailFilter voiceFilter;
    voiceFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    voiceFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeVoice));
    unionFilter << voiceFilter;
        
    QContactDetailFilter modemFilter;
    modemFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    modemFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeModem));
    unionFilter << modemFilter;
        
    QContactDetailFilter videoFilter;
    videoFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    videoFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeVideo));
    unionFilter << videoFilter;
        
    QContactDetailFilter carFilter;
    carFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    carFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeCar));
    unionFilter << carFilter;
        
    QContactDetailFilter bulletinBoardFilter;
    bulletinBoardFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    bulletinBoardFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    unionFilter << bulletinBoardFilter;
	
    return unionFilter;
}

//virtual function, common code for call, videocall and message actions
bool CntAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    return (detail.definitionName() == QContactPhoneNumber::DefinitionName);
}

//virtual function, common code for call, videocall and message actions
QList<QContactDetail> CntAction::supportedDetails(const QContact& contact) const
{
    return contact.details(QContactPhoneNumber::DefinitionName);
}

QContactActionDescriptor CntAction::actionDescriptor() const
{
    QContactActionDescriptor ret;
    ret.setActionName(m_actionName);
    ret.setVendorName(m_vendorName);
    ret.setImplementationVersion(m_implementationVersion);
    return ret;
}


bool CntAction::invokeAction(const QContact& contact, const QContactDetail& detail, const QVariantMap& data)
{
	m_contact = contact;
	m_detail  = detail;
	m_data = data;
	
	QTimer::singleShot(1, this, SLOT(performAction()));
	m_state = QContactAction::ActiveState;
	return true;
}

QContactAction::State CntAction::state() const
{
    return m_state;
}

QVariantMap CntAction::results() const
{
    return m_result;
}

//Clears the action data, is called after the result has been emitted to contact (emitResult function)
void CntAction::resetAction()
{
	m_contact = QContact(); 
	m_detail  = QContactDetail();
}

//returns whether the actionDescription is supported by this action
bool CntAction::actionDescriptionSupported(const QContactActionDescriptor& descriptor) const
{
    bool supported(false);
    
    if ((descriptor.actionName() == m_actionName) && (descriptor.vendorName() == m_vendorName) && (descriptor.implementationVersion() == m_implementationVersion))
        supported = true;
    else
        supported = false; 
    
    return supported;       
}

//common code to perform a call, videocall action
void CntAction::performNumberAction(const QString &interface, const QString &operation)
{
    QVariantList args;
    QVariant retValue;
    
    delete m_request;
    m_request = NULL;
    m_request = m_AppManager.create(interface, operation, false); // not embedded
    if (!m_request) {
        emitResult(GeneralError, retValue);
        return;
    }

    //QContactType == TypeGroup
    if (QContactType::TypeGroup == m_contact.type()) {
        QContactPhoneNumber conferenceCall = m_contact.detail<QContactPhoneNumber>();
        args << conferenceCall.number() << m_contact.localId() << m_contact.displayLabel();
        m_request->setArguments(args);
        m_request->setSynchronous(true);
        m_request->send(retValue);
        emitResult(m_request->lastError(), retValue);
    }
    //QContactType == TypeContact
    //detail exist use it
    else if (m_detail.definitionName() == QContactPhoneNumber::DefinitionName) {
		const QContactPhoneNumber &phoneNumber = static_cast<const QContactPhoneNumber &>(m_detail);
		args << phoneNumber.number() << m_contact.localId() << m_contact.displayLabel();
		m_request->setArguments(args);
		m_request->setSynchronous(true);
        m_request->send(retValue);
        emitResult(m_request->lastError(), retValue);
	}
    //QContactType == TypeContact
    //if no detail, pick preferred
	else if (m_detail.isEmpty())
	{
		QContactDetail detail = m_contact.preferredDetail(m_actionName);
		QContactPhoneNumber phoneNumber;
	
		//if preferred is empty pick first phonenumber
		if (detail.isEmpty()) {
			phoneNumber = m_contact.detail<QContactPhoneNumber>();
		}
		
		//if not empty, cast detail to phonenumber
		else {
			phoneNumber = static_cast<QContactPhoneNumber>(detail);
		}
		
	    args << phoneNumber.number() << m_contact.localId() << m_contact.displayLabel();
	    m_request->setArguments(args);
	    m_request->setSynchronous(true);
	    m_request->send(retValue);
	    emitResult(m_request->lastError(), retValue);
	}
	//else return an error
	else {
		emitResult(GeneralError, retValue);
	}
}

//emit the result to the client
void CntAction::emitResult(int errorCode, const QVariant &retValue)
{
	m_result.clear();
	m_result.insert("Error", QVariant(errorCode));
	m_result.insert("ReturnValue", retValue);
	if (m_request) {
        m_result.insert("XQAiwRequest Error", m_request->lastError());
	}
	
	if (errorCode == 0) {
		m_state = QContactAction::FinishedState;
	}
	
	else {
		m_state = QContactAction::FinishedWithErrorState;
	}
	
	//emit progress(state, m_result);
	emit stateChanged(m_state);
	emit resultsAvailable();

	resetAction(); //reset values in the action
}
