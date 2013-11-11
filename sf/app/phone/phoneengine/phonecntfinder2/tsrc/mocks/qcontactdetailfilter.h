/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTDETAILFILTER_H
#define QCONTACTDETAILFILTER_H

#include <QString>
#include <QVariant>

#include "qcontactfilter.h"

class QContactDetailFilter: public QContactFilter
{
public:
    QContactDetailFilter()
	{
	};
   // QContactDetailFilter(const QContactFilter& other);

    /* Mutators */
    void setDetailDefinitionName(const QString& definition, const QString& fieldName = QString())
	{
	Q_UNUSED(definition)
	Q_UNUSED(fieldName)
	};
    //void setMatchFlags(Qt::MatchFlags flags);

    /* Filter Criterion */
    void setValue(const QVariant& value)
	{
	Q_UNUSED(value)
	};

};

#endif
