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

// System includes
#include <presetutility.h>
#include <preset.h>
#include <QString>

// User includes
#include "radiopresetstorage.h"
#include "radiopresetstorage_p.h"
#include "radiostationif.h"

/*!
 * Converts a symbian descriptor to Qt string
 */
static QString convertString( const TDesC& aDesc )
{
    return QString::fromUtf16( aDesc.Ptr(), aDesc.Length() );
}

/*!
 *
 */
RadioPresetStorage::RadioPresetStorage() :
    d_ptr( new RadioPresetStoragePrivate() )
{
    Q_D( RadioPresetStorage );
    d->init();
}

/*!
 *
 */
RadioPresetStorage::~RadioPresetStorage()
{
}

/*!
 *
 */
int RadioPresetStorage::maxNumberOfPresets() const
{
    Q_D( const RadioPresetStorage );
    return d->mPresetUtility->MaxNumberOfPresets();
}

/*!
 *
 */
int RadioPresetStorage::presetCount() const
{
    Q_D( const RadioPresetStorage );
    return d->mPresetUtility->PresetCount();
}

/*!
 *
 */
int RadioPresetStorage::firstPreset() const
{
    Q_D( const RadioPresetStorage );
    return d->mPresetUtility->FirstPreset();
}

/*!
 *
 */
int RadioPresetStorage::nextPreset( int fromIndex ) const
{
    Q_D( const RadioPresetStorage );
    return d->mPresetUtility->NextPreset( fromIndex );
}

/*!
 *
 */
bool RadioPresetStorage::deletePreset( int presetIndex )
{
    Q_D( RadioPresetStorage );
    return d->mPresetUtility->DeletePreset( presetIndex ) == KErrNone;
}

/*!
 *
 */
bool RadioPresetStorage::savePreset( const RadioStationIf& station )
{
    Q_D( RadioPresetStorage );
    TPreset preset;
    preset.SetFrequency( station.frequency() );
    TPresetName name( station.name().left( KPresetNameLength ).utf16() );
    preset.SetName( name );
    preset.SetRenamedByUser( station.isRenamedByUser() );
    preset.SetGenre( station.genre() );
    TRadioUrl url( station.url().left( KUrlMaxLength ).utf16() );
    preset.SetUrl( url );
    preset.SetPiCode( station.piCode() );
    preset.SetFavorite( station.isFavorite() );
    preset.SetLocalStation( station.isLocalStation() );

    TRAPD( err, d->mPresetUtility->SavePresetL( preset, station.presetIndex() ) );
    return err == KErrNone;
}

/*!
 *
 */
bool RadioPresetStorage::readPreset( int index, RadioStationIf& station )
{
    Q_D( RadioPresetStorage );
    TPreset preset;
    TRAPD( err, d->mPresetUtility->ReadPresetL( index, preset ) );
    if ( !err ) {

        station.setPresetIndex( index );
        station.setFrequency( preset.Frequency() );
        station.setName( convertString( preset.Name() ) );
        station.setRenamedByUser( preset.RenamedByUser() );
        station.setGenre( preset.Genre() );
        station.setUrl( convertString( preset.Url() ) );
        station.setPiCode( preset.PiCode() );
        station.setFavorite( preset.Favorite() );
        station.setLocalStation( preset.LocalStation() );

        return true;
    }
    return false;
}

/*!
 *
 */
void RadioPresetStorage::readFrequencies( QList<uint>& frequencyList )
{
    Q_D( RadioPresetStorage );

    TPreset preset;
    int index = firstPreset();
    while ( index >= 0 ) {
        TRAPD( err, d->mPresetUtility->ReadPresetL( index, preset ) );
        if ( !err ) {
            frequencyList.append( preset.Frequency() );
        }

        index = nextPreset( index );
    }
}

/*!
 *
 */
RadioPresetStoragePrivate::RadioPresetStoragePrivate()
{
}

/*!
 *
 */
RadioPresetStoragePrivate::~RadioPresetStoragePrivate()
{
}

/*!
 *
 */
bool RadioPresetStoragePrivate::init()
{
    TRAPD( err, mPresetUtility.reset( CPresetUtility::NewL() ) );
    return err == KErrNone;
}
