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
#include <QString>
#include <QSettings>
#include <QDataStream>

// User includes
#include "radiopresetstorage.h"
#include "radiopresetstorage_win32_p.h"
#include "radiostationif.h"

const QString KKeyBookKeeping = "BookKeeping";
const QString KKeyBase = "Preset-";

/*!
 *
 */
static QString makeKey( int index ) {
    QString key = KKeyBase;
    key.append( index );
    return key;
}

/*!
 *
 */
Preset::Preset() :
    mFrequency( 0 ),
    mRenamedByUser( false ),
    mGenre( -1 ),
    mPiCode( -1 ),
    mFavorite( false ),
    mLocalStation( false )
{
}

/*!
 *
 */
Preset::~Preset()
{
}

/*!
 *
 */
void Preset::externalize( QDataStream& outStream )
{
    outStream << mFrequency;
    outStream << mName;
    outStream << mRenamedByUser;
    outStream << mGenre;
    outStream << mUrl;
    outStream << mPiCode;
    outStream << mFavorite;
    outStream << mLocalStation;
}

/*!
 *
 */
void Preset::internalize( QDataStream& inStream )
{
    inStream >> mFrequency;
    inStream >> mName;
    inStream >> mRenamedByUser;
    inStream >> mGenre;
    inStream >> mUrl;
    inStream >> mPiCode;
    inStream >> mFavorite;
    inStream >> mLocalStation;
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
    return 100;
}

/*!
 *
 */
int RadioPresetStorage::presetCount() const
{
    Q_D( const RadioPresetStorage );
    return d->mBookKeeping.count();
}

/*!
 *
 */
int RadioPresetStorage::firstPreset() const
{
    Q_D( const RadioPresetStorage );
    if ( d->mBookKeeping.count() > 0 ) {
        return d->mBookKeeping.at( 0 );
    }
    return -1;
}

/*!
 *
 */
int RadioPresetStorage::nextPreset( int fromIndex ) const
{
    Q_D( const RadioPresetStorage );
    int index = d->mBookKeeping.indexOf( fromIndex ) + 1;
    if ( index > 0 && index < d->mBookKeeping.count() ) {
        return d->mBookKeeping.at( index );
    }
    return -1;
}

/*!
 *
 */
bool RadioPresetStorage::deletePreset( int presetIndex )
{
    Q_D( RadioPresetStorage );
    if ( presetIndex > -1 ) {
        int index = d->mBookKeeping.indexOf( presetIndex );
        if ( index > -1 ) {
            d->mBookKeeping.remove( index, 1 );
            d->mSettings->remove( makeKey( presetIndex ) );
            d->removeIndex( presetIndex );
            return true;
        }
    } else if ( presetIndex == -1 ) {
        for ( int i = d->mBookKeeping.count() - 1; i >= 0; --i ) {
            d->mSettings->remove( makeKey( d->mBookKeeping.at( i ) ) );
        }
        d->mBookKeeping.clear();
        return true;
    }
    return false;
}

/*!
 *
 */
bool RadioPresetStorage::savePreset( const RadioStationIf& station )
{
    Preset preset;
    preset.mFrequency = station.frequency();
    preset.mName = station.name();
    preset.mRenamedByUser = station.isRenamedByUser()   ;
    preset.mGenre = station.genre();
    preset.mUrl = station.url();
    preset.mPiCode = station.piCode();
    preset.mFavorite = station.isFavorite();
    preset.mLocalStation = station.isLocalStation();

    QByteArray array;
    QDataStream outStream( &array, QIODevice::WriteOnly );
    preset.externalize( outStream );

    Q_D( RadioPresetStorage );
    d->mSettings->setValue( makeKey( station.presetIndex() ), array );

    d->addIndex( station.presetIndex() );

    return true;
}

/*!
 *
 */
bool RadioPresetStorage::readPreset( int index, RadioStationIf& station )
{
    Q_D( RadioPresetStorage );
    QByteArray array = d->mSettings->value( makeKey( index ) ).toByteArray();
    if ( !array.isEmpty() ) {
        Preset preset;

        QDataStream inStream( array );
        preset.internalize( inStream );

        station.setPresetIndex( index );
        station.setFrequency( preset.mFrequency );
        station.setName( preset.mName );
        station.setRenamedByUser( preset.mRenamedByUser );
        station.setGenre( preset.mGenre );
        station.setUrl( preset.mUrl );
        station.setPiCode( preset.mPiCode );
        station.setFavorite( preset.mFavorite );
        station.setLocalStation( preset.mLocalStation );

        return true;
    }
    return false;
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
    mSettings.reset( new QSettings( "Nokia", "QtFmRadio" ) );
    readBookKeeping();
    return true;
}

/*!
 *
 */
void RadioPresetStoragePrivate::readBookKeeping()
{
    QVariant value = mSettings->value( KKeyBookKeeping );
    if ( value.isValid() ) {
        mBookKeeping = value.toByteArray();
    }
}

/*!
 *
 */
void RadioPresetStoragePrivate::saveBookKeeping()
{
    mSettings->setValue( KKeyBookKeeping, mBookKeeping );
}

/*!
 *
 */
void RadioPresetStoragePrivate::removeIndex( int index )
{
    int pos = mBookKeeping.indexOf( index );
    if ( pos > -1 ) {
        mBookKeeping.remove( pos, 1 );
        saveBookKeeping();
    }
}

/*!
 *
 */
void RadioPresetStoragePrivate::addIndex( int index )
{
    int pos = mBookKeeping.indexOf( index );
    if ( pos == -1 ) {
        mBookKeeping.append( index );
        saveBookKeeping();
    }
}
