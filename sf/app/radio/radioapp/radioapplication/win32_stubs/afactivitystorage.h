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

#ifndef AFACTIVITYSTORAGE_STUB_H
#define AFACTIVITYSTORAGE_STUB_H

#include <QString>
#include <QVariant>

class AfActivityStorage
{

public:
    AfActivityStorage(QObject *parent = 0);
    ~AfActivityStorage();

    bool saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata);
    bool removeActivity(const QString &activityId);

};

#endif // AFACTIVITYSTORAGE_STUB_H
