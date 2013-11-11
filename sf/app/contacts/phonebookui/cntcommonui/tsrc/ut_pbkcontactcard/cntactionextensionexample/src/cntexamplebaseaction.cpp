/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cntexamplebaseaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactonlineaccount.h>
#include <qcontactfilters.h>
#include <QDebug>

MobBaseAction::MobBaseAction(const QString &actionName, const QString& vendorName) :
        m_actionName(actionName),
        m_vendorName(vendorName),
        m_implementationVersion(1),
        m_result(), 
        m_contact(),
        m_detail()
{
}
        
MobBaseAction::~MobBaseAction()
{
}

QContactActionDescriptor MobBaseAction::actionDescriptor() const
{
    QContactActionDescriptor ret;
    ret.setActionName(m_actionName);
    ret.setVendorName(m_vendorName);
    ret.setImplementationVersion(m_implementationVersion);
    return ret;
}

bool MobBaseAction::invokeAction(const QContact& contact, const QContactDetail& detail, const QVariantMap& parameters)
{
	m_contact = contact;
	m_detail  = detail;
}

//Clears the action data, is called after the result has been emitted to contact (emitResult function)
void MobBaseAction::resetAction()
{
	m_contact = QContact(); 
	m_detail  = QContactDetail();
}

//returns whether the actionDescription is supported by this action
bool MobBaseAction::actionDescriptionSupported(const QContactActionDescriptor& descriptor) const
{
    bool supported(false);
    
    if ((descriptor.actionName() == m_actionName) && (descriptor.vendorName() == m_vendorName) && (descriptor.implementationVersion() == m_implementationVersion))
        supported = true;
    else
        supported = false; 
    
    return supported;       
}

//emit the result to the client
void MobBaseAction::emitResult(int errorCode, const QVariant &retValue)
{
//	m_result.clear();
//	m_result.insert("Error", QVariant(errorCode));
//	m_result.insert("ReturnValue", retValue);
//	
//	QContactAction::Status status;
//	
//	if (errorCode == 0){
//		status = QContactAction::Finished;
//	}
//	
//	else{
//		status = QContactAction::FinishedWithError;
//	}
//	
//	emit progress(status, m_result);

	resetAction(); //reset values in the action
}

QVariantMap MobBaseAction::results() const
{
    return m_result;
}
