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

#ifndef AFACTIVITYSTORAGE_H
#define AFACTIVITYSTORAGE_H


#include <qstring.h>
#include <qstringlist.h>
#include <qvariant.h>

class AfActivityStorage
{

public:
    AfActivityStorage();
    virtual ~AfActivityStorage();

    bool saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata);
    
    QVariant activityData(const QString &activityId) const;
    
    bool removeActivity (const QString &activityId);
    
    static QVariant mDataToReturn;
   
    static int mActivityCount;
   
    static QString mLastHandledActivity;
    
};

#endif // AFACTIVITYSTORAGE_H

