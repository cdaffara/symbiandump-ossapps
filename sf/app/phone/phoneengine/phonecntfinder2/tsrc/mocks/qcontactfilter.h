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


#ifndef QCONTACTFILTER_H
#define QCONTACTFILTER_H

#include <QVariant>
#include <QList>
#include <QDateTime>

#include "qtcontactsglobal.h"

class QContactFilter
{
public:
    QContactFilter() {};


    enum FilterType {
        InvalidFilter,
        ContactDetailFilter,
        ContactDetailRangeFilter,
        ChangeLogFilter,
        ActionFilter,
        GroupMembershipFilter,
        IntersectionFilter,
        UnionFilter,
        IdListFilter,
        DefaultFilter
    };

 };
#endif
