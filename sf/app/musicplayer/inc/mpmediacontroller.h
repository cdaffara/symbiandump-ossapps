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
* Description: Music Player Media Controller.
*
*/

#ifndef MPMEDIACONTROLLER_H
#define MPMEDIACONTROLLER_H

#include <QObject>
#include <QtGlobal>

class MpMediaKeyHandler;
class MpVolumeSlider;

#ifdef BUILD_MPMEDIACONTROLLER
#define BUILD_MPMEDIACONTROLLER_EXPORT Q_DECL_EXPORT
#else
#define BUILD_MPMEDIACONTROLLER_EXPORT Q_DECL_IMPORT
#endif
class  BUILD_MPMEDIACONTROLLER_EXPORT MpMediaController: public QObject
{

Q_OBJECT

public:

    explicit MpMediaController( QObject *parent = 0 );
    virtual ~MpMediaController();

private:
    Q_DISABLE_COPY(MpMediaController)
    MpMediaKeyHandler           *mMpMediaKeyHandler;    // own
    MpVolumeSlider              *mMpVolumeSlider;       // own
};

#endif    // MPMEDIACONTROLLER_H
