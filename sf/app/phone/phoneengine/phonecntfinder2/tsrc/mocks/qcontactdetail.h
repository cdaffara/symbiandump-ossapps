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

#ifndef QCONTACTDETAIL_H
#define QCONTACTDETAIL_H

#include "qtcontactsglobal.h"

#include <QString>


class QContactDetail
{
public:
    QContactDetail();
	QContactDetail(const QString& definitionName);
    virtual ~QContactDetail();
	QContactDetail(const QContactDetail& other);
	QContactDetail& operator=(const QContactDetail& other);
    QString value(const QString& key) const;
	QString definitionName() const;
public: //for testing 
	void setValue(const QString& value);
	


private:
    QString mValue;
	QString mDefinition;

};



#define Q_DECLARE_CUSTOM_CONTACT_DETAIL(className, definitionNameString) \
    className() : QContactDetail(DefinitionName) {}\
    Q_DECLARE_LATIN1_LITERAL(DefinitionName, definitionNameString);

#define Q_IMPLEMENT_CUSTOM_CONTACT_DETAIL(className, definitionNameString) \
    Q_DEFINE_LATIN1_LITERAL(className::DefinitionName, definitionNameString)
	
#endif //qcontactdetail
	


