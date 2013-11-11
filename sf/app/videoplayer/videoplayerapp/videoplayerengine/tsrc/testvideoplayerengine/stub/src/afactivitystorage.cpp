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
* Description:  stub AfActivityStorage
*
*/

// Version : %version:  2 %

#include "afactivitystorage.h"

QVariant AfActivityStorage::mDataToReturn = QVariant();

int AfActivityStorage::mActivityCount = 0;

QString AfActivityStorage::mLastHandledActivity = QString();

AfActivityStorage::AfActivityStorage()
{
}

AfActivityStorage::~AfActivityStorage()
{
}

bool AfActivityStorage::saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata)
{
    Q_UNUSED(activityData);
    Q_UNUSED(metadata);
    mLastHandledActivity = activityId;
    mActivityCount++;
}
    
QVariant AfActivityStorage::activityData(const QString &activityId) const
{
    Q_UNUSED(activityId);
    return mDataToReturn;
}

bool AfActivityStorage::removeActivity(const QString &activityId)
{
    mLastHandledActivity = activityId;
    mActivityCount--;
    return true;
}
        
    
