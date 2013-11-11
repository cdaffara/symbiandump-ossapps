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
* Description:
*
*/

#ifndef RADIOPRESETSTORAGE_P_H_
#define RADIOPRESETSTORAGE_P_H_

// User includes
#include <QScopedPointer>
#include <QByteArray>

// Forward declarations
class QSettings;
class QDataStream;

class Preset
{
public:

    Preset();
    ~Preset();

    void externalize( QDataStream& outStream );
    void internalize( QDataStream& inStream );

    uint        mFrequency;
    QString     mName;
    bool        mRenamedByUser;
    int         mGenre;
    QString     mUrl;
    int         mPiCode;
    bool        mFavorite;
    bool        mLocalStation;

};

class RadioPresetStoragePrivate
{
public:

    RadioPresetStoragePrivate();
    ~RadioPresetStoragePrivate();

    bool init();

    void readBookKeeping();
    void saveBookKeeping();

    void removeIndex( int index );
    void addIndex( int index );

    QScopedPointer<QSettings>   mSettings;
    QByteArray                  mBookKeeping;

};

#endif // RADIOPRESETSTORAGE_P_H_
