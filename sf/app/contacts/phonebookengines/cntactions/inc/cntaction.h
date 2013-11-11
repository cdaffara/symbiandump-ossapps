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

#ifndef MOBCNTACTION_H
#define MOBCNTACTION_H

#include <qcontactaction.h>
#include <qcontactactiondescriptor.h>

#include <QSharedData>
#include <QString>
#include <QVariantMap>

#include <xqappmgr.h>

QTM_USE_NAMESPACE

class XQAiwRequest;

class CntAction : public QContactAction
{
    Q_OBJECT

public:   
    CntAction(const QString &actionName);
    virtual ~CntAction();

public: 
	enum ErrorCodes {
		GeneralError = 1,
		DetailNotSupported
	};
    

public: //QContactAction
	QContactActionDescriptor actionDescriptor() const;
    QVariantMap metaData() const;
    QContactFilter contactFilter(const QVariant& value) const;
    
    bool isDetailSupported(const QContactDetail &detail, const QContact &contact = QContact()) const;
    QList<QContactDetail> supportedDetails(const QContact& contact) const;
    
    bool invokeAction(const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& parameters = QVariantMap());
    QContactAction::State state() const;
    QVariantMap results() const;

public:
	virtual void resetAction();
	virtual CntAction* clone() const = 0;
	
private slots:	
	virtual void performAction() = 0;    

public:   
    //returns whether the actionDescription is supported by this action
    bool actionDescriptionSupported(const QContactActionDescriptor& descriptor) const;
    void performNumberAction(const QString &interface, const QString &operation);
    void emitResult(int errorCode, const QVariant &retValue);
    
protected:
    QString m_actionName;
	QString m_vendorName;
	int m_implementationVersion;
	
	QVariantMap m_result; 		//result returned to client
	QContact m_contact;    		//contact passed to invokeAction
	QContactDetail m_detail; 	//detail passed to invokeAction
	QVariantMap m_data; 	//data passed to invokeAction
	QContactAction::State m_state;
	XQAiwRequest* m_request;
    XQApplicationManager    m_AppManager;
};

#endif //MOBCNTACTION_H
