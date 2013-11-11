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
* Description: PlayMusic Service interface.
*
*/

#ifndef PLAYMUSICSERVICE_H
#define PLAYMUSICSERVICE_H

#include <xqserviceprovider.h>
#include <xqsharablefile.h>
#include <QStringList>

// Forward declarations
class MusicServices;

/**
 *  PlayMusicService
 *
 */

class PlayMusicService : public XQServiceProvider
{
    Q_OBJECT

public:
    
    PlayMusicService( MusicServices* parent = 0);
    ~PlayMusicService();
    bool isActive();
    void complete( QStringList filesList );
    QString contextTitle() const;
    
public slots:
    
    bool view(const QString& file);
    bool view(const XQSharableFile& file);
    
private:
    int             mRequestIndex;
    MusicServices   *mServiceApp;
    QString         mTitle;
};



#endif // PLAYMUSICSERVICE_H
