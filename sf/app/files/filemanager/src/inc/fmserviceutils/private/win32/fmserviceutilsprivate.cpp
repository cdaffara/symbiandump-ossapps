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
*     The source file of service utils private for win32 wrapper
*/

#include "fmserviceutilsprivate.h"

/*!
    constructor
*/
FmServiceUtilsPrivate::FmServiceUtilsPrivate()
{
}

/*!
    destructor
*/
FmServiceUtilsPrivate::~FmServiceUtilsPrivate()
{
}

/*!
    dummy function for win32 build
*/
void FmServiceUtilsPrivate::sendFile( const QStringList &filePathList )
{
    Q_UNUSED( filePathList );
}

/*!
    dummy function for win32 build
*/
void FmServiceUtilsPrivate::closeApps()
{

}

/*!
    dummy function for win32 build
*/
void FmServiceUtilsPrivate::restartApps()
{

}

/*!
    dummy function for win32 build
*/
bool FmServiceUtilsPrivate::saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata)
{
    Q_UNUSED( activityId );
    Q_UNUSED( activityData );
    Q_UNUSED( metadata );
    return false;
}

/*!
    dummy function
*/
bool FmServiceUtilsPrivate::removeActivity(const QString &activityId)
{
    Q_UNUSED( activityId );
    return false;
}
