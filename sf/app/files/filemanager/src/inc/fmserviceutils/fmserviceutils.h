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
*     The header file of service utils
*/

#ifndef FMSERVICEUTILS_H
#define FMSERVICEUTILS_H

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QVariantHash>

class FmServiceUtilsPrivate;

/*!
    \class FmServiceUtils 
    \brief The class FmServiceUtils provide some service utils APIs
    As FmUtils provide static APIs, FmServiceUtils provide non-static APIs
*/
class FmServiceUtils : public QObject
{
Q_OBJECT
public:
    FmServiceUtils( QObject *parent = 0 );
    virtual ~FmServiceUtils();
    void sendFile( const QStringList &filePathList );
    
    void closeApps();
    void restartApps();
    
    // activity wrapper
    bool saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata);
    bool removeActivity(const QString &activityId);

private:
    FmServiceUtilsPrivate *d;
};


#endif
