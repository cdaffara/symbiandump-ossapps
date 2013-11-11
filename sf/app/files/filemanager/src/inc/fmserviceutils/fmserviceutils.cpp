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
*     The source file of service utils
*/

#include "fmserviceutils.h"

#include "fmserviceutilsprivate.h"

FmServiceUtils::FmServiceUtils( QObject *parent ) : QObject( parent )
{
    d = new FmServiceUtilsPrivate;
}

FmServiceUtils::~FmServiceUtils()
{
    delete d;
}

/*!
    send files that included in \a filePathList via ShareUi
*/
void FmServiceUtils::sendFile( const QStringList &filePathList )
{
    d->sendFile( filePathList );
}

/*!
    call CBaBackupSessionWrapper to close apps, for example, before formant, apps need be closed
    this is synchronous which will take a while, please call this function in thread
*/
void FmServiceUtils::closeApps()
{
    d->closeApps();
}

/*!
    call CBaBackupSessionWrapper to restart apps
    for example, after formant, closed apps need be restarted
    this is synchronous which will return quickly.
*/
void FmServiceUtils::restartApps()
{
    d->restartApps();
}

/*!
    save acitivity for activity wrapper
*/
bool FmServiceUtils::saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata)
{
    return d->saveActivity( activityId, activityData, metadata );
}

/*!
    remove acitivity for activity wrapper
*/
bool FmServiceUtils::removeActivity(const QString &activityId)
{
    return d->removeActivity( activityId );
}
