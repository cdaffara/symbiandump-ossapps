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
* Description: Music Player engine.
*
*/

#ifndef MPENGINE_H
#define MPENGINE_H

#include <QObject>
#include <QStringList>
//#include "mpmpxcollectionviewdefs.h"
//#include "mpmpxcommondefs.h"
//#include "mpcommondefs.h"

//class MpMpxCollectionData;
//class MpPlaybackData;
//class MpSongData;
//class XQSharableFile;
class MpEngine : public QObject
{
    Q_OBJECT

    friend class MpEngineFactory;

public:
    enum EngineMode{
        StandAlone,
        Fetch,
        Embedded,
        MediaBrowsing
    };
    
private:
    explicit MpEngine();

public:

    virtual ~MpEngine();

    // Equalizer related
    void applyPreset( int preset );
    void disableEqualizer();
    int activePreset();
    QStringList presetNames();

signals:

    // Equalizer related
    void errorOccured( int error );
    void equalizerReady();

private:

    Q_DISABLE_COPY( MpEngine )

public:
    int mActivePreset;
    int mApplyPresetCount;
    int mDisableEquqlizerCount;

};

#endif // MPENGINE_H
