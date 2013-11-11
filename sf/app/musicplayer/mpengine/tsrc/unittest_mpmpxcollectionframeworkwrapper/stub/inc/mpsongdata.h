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
* Description: mpsongdata stub for testing mpmpxframeworkwrapper.
*
*/

#ifndef MPSONGDATA_H
#define MPSONGDATA_H

#include <QObject>

class CMPXMedia;

class MpSongData : public QObject
{
    Q_OBJECT

public:
    explicit MpSongData( QObject *parent=0 );
    virtual ~MpSongData();

    void setMpxMedia( const CMPXMedia& aMedia );

signals:
    void albumArtReady();
    void playbackInfoChanged();
	void songDetailInfoChanged();

public:

    bool          iSetMedia;

};

#endif // MPSONGDATA_H
