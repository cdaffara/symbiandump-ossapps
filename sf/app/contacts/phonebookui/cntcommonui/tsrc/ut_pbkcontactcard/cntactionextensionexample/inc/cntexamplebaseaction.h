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

#ifndef MOBBASEACTION_H
#define MOBBASEACTION_H

#include <qcontactaction.h>
#include <qcontactactiondescriptor.h>

#include <QSharedData>
#include <QString>
#include <QVariantMap>

QTM_USE_NAMESPACE

class MobBaseAction : public QContactAction
{
    Q_OBJECT

public:   
    MobBaseAction(const QString &actionName, const QString& vendorName);
    virtual ~MobBaseAction();

public: 
	enum ErrorCodes {
		GeneralError = 1,
		DetailNotSupported
	};
    

public: //QContactAction
	QContactActionDescriptor actionDescriptor() const;
    bool invokeAction(const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& parameters = QVariantMap());
    QVariantMap results() const;

public:
	virtual void resetAction();
	virtual MobBaseAction* clone() const = 0;

public:   
    //returns whether the actionDescription is supported by this action
    bool actionDescriptionSupported(const QContactActionDescriptor& descriptor) const;
    void emitResult(int errorCode, const QVariant &retValue);
    
    
protected:
    QString m_actionName;
	QString m_vendorName;
	int m_implementationVersion;
	
	QVariantMap m_result; 		//result returned to client
	QContact m_contact;    		//contact passed to invokeAction
	QContactDetail m_detail; 	//detail passed to invokeAction
};

#endif //MOBBASEACTION_H
