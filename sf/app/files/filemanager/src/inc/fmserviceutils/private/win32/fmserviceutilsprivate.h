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
* 
* Description:
*     The header file of service utils private for win32 wrapper
*/

#ifndef FMSERVICEUTILSPRIVATE_H
#define FMSERVICEUTILSPRIVATE_H

#include <QStringList>
#include <QVariant>
#include <QVariantHash>

class FmServiceUtilsPrivate
{
public:
    FmServiceUtilsPrivate();
    virtual ~FmServiceUtilsPrivate();

    void sendFile( const QStringList &filePathList );
    void closeApps();
    void restartApps();
    
    // activity implement
    bool saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata);
    bool removeActivity(const QString &activityId);
};

#endif
