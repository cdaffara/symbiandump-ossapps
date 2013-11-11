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
* Description: GetMusic Service interface.
*
*/

#ifndef GETMUSICSERVICE_H
#define GETMUSICSERVICE_H

#include <xqserviceprovider.h>
#include <QStringList>

// Forward declarations
class MusicServices;

/**
 *  GetMusicService
 *
 */
class GetMusicService : public XQServiceProvider
{
    Q_OBJECT
public:
    GetMusicService( MusicServices *parent = 0 );
    ~GetMusicService();
    bool isActive();
    void complete( QStringList filesList );
    QString contextTitle() const;

public slots:
    void fetch();

private:
    int             mRequestIndex;
    MusicServices   *mServiceApp;
    QString         mTitle;
};



#endif // GETMUSICSERVICE_H
