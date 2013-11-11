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

#ifndef RADIOPRESETSTORAGE_H_
#define RADIOPRESETSTORAGE_H_

// System includes
#include <QtGlobal>
#include <QList>
#include <QScopedPointer>

#include "radiopresetstorageexport.h"

// Forward declarations
class RadioStationIf;
class RadioPresetStoragePrivate;

class STORAGE_DLL_EXPORT RadioPresetStorage
{
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioPresetStorage )
    Q_DISABLE_COPY( RadioPresetStorage )

public:

    RadioPresetStorage();
    ~RadioPresetStorage();

    int maxNumberOfPresets() const;
    int presetCount() const;
    int firstPreset() const;
    int nextPreset( int fromIndex ) const;
    bool deletePreset( int presetIndex );
    bool savePreset( const RadioStationIf& station );
    bool readPreset( int index, RadioStationIf& station );
    void readFrequencies( QList<uint>& frequencyList );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    const QScopedPointer<RadioPresetStoragePrivate> d_ptr;

};

#endif // RADIOPRESETSTORAGE_H_
