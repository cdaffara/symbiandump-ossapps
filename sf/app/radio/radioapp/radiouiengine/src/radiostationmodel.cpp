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
#include <QStringList>

// User includes
#include "radiostationmodel.h"
#include "radiostationmodel_p.h"
#include "radiopresetstorage.h"
#include "radioenginewrapper.h"
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radiostation.h"
#include "radiostation_p.h"
#include "radiologger.h"

/*!
 *
 */
static QString parseLine( const RadioStation& station )
{
    QString line = "";

    QString name = station.name();
    if ( !name.isEmpty() )
    {
        line.append( name.trimmed() );
    } else {
        const QString parsedFrequency = qtTrId( "txt_rad_dblist_l1_mhz" ).arg( RadioStation::parseFrequency( station.frequency() ) );
        line.append( parsedFrequency );
    }

    LOG_FORMAT( "RadioStationModel: Returning line %s", GETSTRING(line) );
    return line;
}

/*!
 *
 */
RadioStationModel::RadioStationModel( RadioUiEnginePrivate& uiEngine ) :
    QAbstractListModel( &uiEngine.api() ),
    d_ptr( new RadioStationModelPrivate( this, uiEngine ) )
{
}

/*!
 *
 */
RadioStationModel::~RadioStationModel()
{
}

/*!
 *
 */
Qt::ItemFlags RadioStationModel::flags ( const QModelIndex& index ) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags( index );
    flags |= Qt::ItemIsEditable;
    return flags;
}

/*!
 *
 */
int RadioStationModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    Q_D( const RadioStationModel );
    const int count = d->mStations.keys().count();
    return count;
}

/*!
 * Checks the given station and emits signals based on what member variables had been changed
 */
QVariant RadioStationModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    Q_D( const RadioStationModel );
    if ( role == Qt::DisplayRole ) {
        RadioStation station = stationAt( index.row() );
        QString firstLine = parseLine( station );
        QString name = station.name();

        if ( !name.isEmpty() ) {
            if ( currentStation().frequency() == station.frequency() ) {
                if ( d->mDetailLevel.testFlag( RadioStationModel::ShowGenre ) ) {
                    QStringList list;
                    list.append( firstLine );
                    QString genre = " "; // Empty space so that the listbox generates the second row
                    if ( station.genre() != -1 ) {
                        genre = d->mUiEngine.api().genreToString( station.genre(), GenreTarget::StationsList );
                    }
                    list.append( genre );

                    return list;
                }
            } else {
                QStringList list;
                list.append( firstLine );
                list.append( qtTrId( "txt_rad_dblist_l1_mhz2" ).arg( RadioStation::parseFrequency( station.frequency() ) ) );
                return list;
            }
        } else {
            if ( currentStation().frequency() != station.frequency() ) {
                QStringList list;
                list.append( firstLine );
                list.append( " " );
                return list;
            } else {
                QStringList list;
                list.append( firstLine );
                QString genre = " "; // Empty space so that the listbox generates the second row
                if ( station.genre() != -1 ) {
                    genre = d->mUiEngine.api().genreToString( station.genre(), GenreTarget::StationsList );
                }
                list.append( genre );
                return list;
            }
        }

        return firstLine;
    } else if ( role == RadioRole::RadioStationRole ) {
        QVariant variant;
        variant.setValue( stationAt( index.row() ) );
        return variant;
    } else if ( role == Qt::DecorationRole &&
                d->mDetailLevel.testFlag( RadioStationModel::ShowIcons ) ) {
        RadioStation station = stationAt( index.row() );
        QVariantList list;
        if ( station.isFavorite() && !d->mFavoriteIcon.isNull() ) {
            list.append( d->mFavoriteIcon );
        } else if ( !station.isFavorite() && !d->mNonFavoriteIcon.isNull() ) {
            list.append( d->mNonFavoriteIcon );
        }else {
            list.append( QIcon() );
        }
        if ( currentStation().frequency() == station.frequency() && !d->mNowPlayingIcon.isNull() ) {
            list.append( d->mNowPlayingIcon );
        }
        return list;
    } else if ( role == RadioRole::IsFavoriteRole ) {
        QVariant variant;
        variant.setValue( stationAt( index.row() ).isFavorite() );
        return variant;
    }

    return QVariant();
}

/*!
 * Checks the given station and emits signals based on what member variables had been changed
 */
bool RadioStationModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    Q_UNUSED( index );

    if ( role == RadioRole::ToggleFavoriteRole ) {
        const uint frequency = value.toUInt();
        RadioStation station;
        if ( findFrequency( frequency, station ) ) {
            setFavoriteByPreset( station.presetIndex(), !station.isFavorite() );
        } else {
            setFavoriteByFrequency( frequency, true );
        }

        return true;
    }

    return false;
}

/*!
 * Called by the engine to initialize the list with given amount of presets
 */
void RadioStationModel::initialize( RadioPresetStorage* storage, RadioEngineWrapper* wrapper )
{
    Q_D( RadioStationModel );
    d->mPresetStorage = storage;
    d->mWrapper = wrapper;

    int index = d->mPresetStorage->firstPreset();
    LOG_FORMAT( "RadioStationModel::initialize: presetCount: %d, firstIndex: %d",
                                            d->mPresetStorage->presetCount(), index );

    while ( index >= 0 ) {
        RadioStation station;

        RadioStationIf* stationInterface = static_cast<RadioStationIf*>( station.data_ptr() );
        if ( d->mPresetStorage->readPreset( index, *stationInterface ) ) {
            if ( station.isValid() && d->mWrapper->isFrequencyValid( station.frequency() ) ) {

#ifdef INIT_STATIONS_WITH_DUMMY_RT
                station.setGenre( GenreEurope::RdsChildrensProgrammes );
                if ( index % 3 == 0 ) {
                    station.setName( "Radio Rock" );
                    station.setRadioText( "Now playing: <font color='cyan'>The Presidents of the United States of America</font> - <font color='cyan'>Dune Buggy and diipa daapa jhkjhui erjlkej rewjtl</font>" );
                } else if ( index % 2 == 0 ) {
                    station.setName( "Radio Rock" );
                } else {
                    station.setDynamicPsText( "DYN PS" );
                }
#endif // INIT_STATIONS_WITH_DUMMY_RT

                // Check if the station seems to send RDS or not.
                // Note that radiotext is not checked because it is not saved to cenrep
                // TODO: Consider saving this state flag to cenrep
                if ( ( station.hasName() && !station.isRenamed() ) || station.hasUrl() ) {
                    static_cast<RadioStationIf*>( station.data_ptr() )->setStationHasSentRds( true );
                }

                d->mStations.insert( station.frequency(), station );
            } else {
                LOG( "RadioStationModel::initialize: Invalid station!" );
                LOG_FORMAT( "Invalid station freq: %d", station.frequency() );
            }
        }

        index = d->mPresetStorage->nextPreset( index );
    }

    d->setCurrentStation( d->mWrapper->currentFrequency() );

    wrapper->addObserver( d );
}

/*!
 * Sets the icons to be used in the lists
 */
void RadioStationModel::setIcons( const QIcon& favoriteIcon, const QIcon& nonFavoriteIcon, const QIcon& nowPlayingIcon )
{
    Q_D( RadioStationModel );
    d->mFavoriteIcon = favoriteIcon;
    d->mNonFavoriteIcon = nonFavoriteIcon;
    d->mNowPlayingIcon = nowPlayingIcon;
}

/*!
 * Returns a reference to the station handler interface
 */
RadioStationHandlerIf& RadioStationModel::stationHandlerIf()
{
    Q_D( RadioStationModel );
    return *d;
}

/*!
 * Returns a reference to the underlying QList so that it can be easily looped
 */
const Stations& RadioStationModel::list() const
{
    Q_D( const RadioStationModel );
    return d->mStations;
}

/*!
 * Returns the station at the given index.
 */
RadioStation RadioStationModel::stationAt( int index ) const
{
    // Get the value from the keys list instead of directly accessing the values list
    // because QMap may have added a default-constructed value to the values list
    Q_D( const RadioStationModel );
    if ( index >= 0 && index < d->mStations.keys().count() ) {
        uint frequency = d->mStations.keys().at( index );
        return d->mStations.value( frequency );
    }
    return RadioStation();
}

/*!
 * Finds a station by frequency
 */
bool RadioStationModel::findFrequency( uint frequency, RadioStation& station, FindCriteria::Criteria criteria ) const
{
    Q_D( const RadioStationModel );

    if ( criteria == FindCriteria::IncludeManualStation && d->mCurrentStation->frequency() == frequency ) {
        station = *d->mCurrentStation;
        return true;
    }

    if ( d->mStations.contains( frequency ) ) {
        station = d->mStations.value( frequency );
        return true;
    }
    return false;
}

/*!
 * Convenience function to find a radio station.
 */
RadioStation RadioStationModel::findStation( uint frequency, FindCriteria::Criteria criteria ) const
{
    RadioStation station;
    findFrequency( frequency, station, criteria ); // Return value ignored
    return station;
}

/*!
 * Finds a station by preset index
 */
int RadioStationModel::findPresetIndex( int presetIndex )
{
    Q_D( RadioStationModel );
    int index = 0;
    foreach( const RadioStation& tempStation, d->mStations ) {
        if ( tempStation.presetIndex() == presetIndex ) {
            return index;
        }
        ++index;
    }

    return RadioStation::NotFound;
}

/*!
 * Finds a station by preset index
 */
int RadioStationModel::findPresetIndex( int presetIndex, RadioStation& station )
{
    Q_D( RadioStationModel );
    const int index = findPresetIndex( presetIndex );
    if ( index != RadioStation::NotFound ) {
        station = d->mStations.values().at( index );
    }
    return index;
}

/*!
 * Finds the closest station from the given frequency
 */
RadioStation RadioStationModel::findClosest( const uint frequency, StationSkip::Mode mode )
{
    Q_D( RadioStationModel );
    const bool findFavorite = mode == StationSkip::PreviousFavorite || mode == StationSkip::NextFavorite;
    const bool findNext = mode == StationSkip::Next || mode == StationSkip::NextFavorite;
    QList<RadioStation> list = findFavorite ? d->favorites() : d->mStations.values();

    if ( list.isEmpty() ) {
        return RadioStation();
    }

    // Find the previous and next station from current frequency
    RadioStation previous;
    RadioStation next;
    foreach( const RadioStation& station, list ) {
        const uint testFreq = station.frequency();
        if ( testFreq == frequency ) {
            continue;
        }

        if ( testFreq > frequency ) {
            next = station;
            break;
        }
        previous = station;
    }

    // Check if we need to loop around
    if ( findNext && !next.isValid() ) {
        next = list.first();
    } else if ( !findNext && !previous.isValid() ) {
        previous = list.last();
    }

    return findNext ? next : previous;
}

/*!
 * Checks if the model contains the given frequency
 */
bool RadioStationModel::contains( const uint frequency ) const
{
    RadioStation unused;
    return findFrequency( frequency, unused );
}

/*!
 * Removes a station by frequency
 */
void RadioStationModel::removeByFrequency( uint frequency )
{
    RadioStation station;
    if ( findFrequency( frequency, station ) ) {
        removeStation( station );
    }
}

/*!
 * Removes a station by preset index
 */
void RadioStationModel::removeByPresetIndex( int presetIndex )
{
    RadioStation station;
    const int index = findPresetIndex( presetIndex, station );
    if ( index >= 0 ) {
        removeStation( station );
    }
}

/*!
 * Removes the given station
 */
void RadioStationModel::removeStation( const RadioStation& station )
{
    Q_D( RadioStationModel );
    const uint frequency = station.frequency();
    if ( d->mStations.contains( frequency ) ) {

        // If we are removing the current station, copy its data to the current station pointer
        // to keep all of the received RDS data still available. They will be discarded when
        // the user tunes to another frequency, but they are available if the user decides to add it back.
        if ( d->mCurrentStation->frequency() == frequency ) {
            *d->mCurrentStation = station;
        }

        // Copy the station to a temporary variable that can be used as signal parameter
        RadioStation tempStation = station;

        const int row = indexFromFrequency( tempStation.frequency() );
        beginRemoveRows( QModelIndex(), row, row );

        d->mPresetStorage->deletePreset( tempStation.presetIndex() );
        d->mStations.remove( frequency );

        d->mCurrentStation = NULL;
        d->setCurrentStation( d->mWrapper->currentFrequency() );

        endRemoveRows();
    }
}
/*!
 * Removes stations based on model indices
 */
void RadioStationModel::removeByModelIndices( QModelIndexList& indices, bool removefavorite )
{
    // List needs to be sorted and indices needs to go throught from largest to smallest.
    // This is for keeping QmodelIndexing in sync after begin- and endremoverows, which
    // are needed for each item separately
    qSort( indices );
    QModelIndexList::const_iterator iter = indices.constEnd();
    QModelIndexList::const_iterator begin = indices.constBegin();
    RadioStation station;  
    while( iter != begin ) {
       iter--;
       station = stationAt( (*iter).row() );
       if( removefavorite ) {
          setFavoriteByPreset( station.presetIndex(), false );   
      } else {                     
          removeStation( station );
      }                     
    }
}

/*!
 * Public slot
 * Removes all stations
 */
void RadioStationModel::removeAll( RemoveMode mode )
{
    Q_D( RadioStationModel );
    if ( d->mStations.count() == 0 ) {
        return;
    }

    if ( mode == RemoveAll ) {
        beginRemoveRows( QModelIndex(), 0, rowCount() - 1 );

        // Preset utility deletes all presets with index -1
        bool success = d->mPresetStorage->deletePreset( -1 );
        Q_UNUSED( success );
        RADIO_ASSERT( success, "FMRadio", "Failed to remove station" );

        d->mStations.clear();
        d->mCurrentStation = NULL;
        d->setCurrentStation( d->mWrapper->currentFrequency() );

        endRemoveRows();
    } else {
        foreach( const RadioStation& station, d->mStations ) {

            if ( mode == RemoveLocalStations ) {
                if ( station.isType( RadioStation::LocalStation ) && !station.isFavorite() ) {
                    removeStation( station );
                }
            } else {
                if ( station.isFavorite() ) {
                    RadioStation newStation( station );
                    newStation.setFavorite( false );
                    saveStation( newStation );
                }
            }
        }
    }
}

/*!
 * Adds a new station to the list
 */
void RadioStationModel::addStation( const RadioStation& station )
{
    Q_D( RadioStationModel );
    const int newIndex = findUnusedPresetIndex();
    LOG_FORMAT( "RadioStationModel::addStation: Adding station to index %d", newIndex );

    RadioStation newStation = station;
    newStation.setPresetIndex( newIndex );
    newStation.unsetType( RadioStation::ManualStation );

    // We have to call beginInsertRows() BEFORE the addition is actually done so we must figure out where
    // the new station will go in the sorted frequency order
    int row = 0;
    const int count = rowCount();
    if ( count > 1 ) {
        Stations::const_iterator iter = d->mStations.upperBound( newStation.frequency() );
        if ( d->mStations.contains( iter.key() ) ) {
            row = d->mStations.keys().indexOf( iter.key() );
        } else {
            row = count;
        }
    } else if ( count == 1 ) {
        uint existingFreq = d->mStations.keys().first();
        if ( station.frequency() > existingFreq ) {
            row = 1;
        }
    }

    beginInsertRows( QModelIndex(), row, row );

    d->doSaveStation( newStation );

    d->setCurrentStation( d->mWrapper->currentFrequency() );

    endInsertRows();

    // Not all UI components listen to rowsInserted() signal so emit the favorite signal
    if ( newStation.isFavorite() ) {
        emit favoriteChanged( *d->mCurrentStation );
    }
}

/*!
 * Saves the given station. It is expected to already exist in the list
 */
void RadioStationModel::saveStation( RadioStation& station )
{
    Q_D( RadioStationModel );
    const bool stationHasChanged = station.hasChanged();
    RadioStation::Change changeFlags = station.changeFlags();
    station.resetChangeFlags();

    if ( station.isType( RadioStation::ManualStation ) ) {

        d->mManualStation = station;
        emitChangeSignals( station, changeFlags );

    } else if ( station.isValid() && stationHasChanged && d->mStations.contains( station.frequency() )) {

        d->doSaveStation( station, changeFlags.testFlag( RadioStation::PersistentDataChanged ) );
        d->setCurrentStation( d->mWrapper->currentFrequency() );

        emitChangeSignals( station, changeFlags );
    }
}

/*!
 * Finds number of favorite stations
 */
int RadioStationModel::favoriteCount()
{
    Q_D( const RadioStationModel );
    return d->favorites().count();
}

/*!
 * Finds number of local stations
 */
int RadioStationModel::localCount()
{
    Q_D( const RadioStationModel );
    return d->locals().count();
}

/*!
 * Changes the favorite status of a station by its frequency. If the station does
 * not yet exist, it is added.
 */
void RadioStationModel::setFavoriteByFrequency( uint frequency, bool favorite )
{
    Q_D( RadioStationModel );
    if ( d->mWrapper->isFrequencyValid( frequency ) ) {
        LOG_FORMAT( "RadioStationModel::setFavoriteByFrequency, frequency: %d", frequency );
        RadioStation station;
        if ( findFrequency( frequency, station ) ) { // Update existing preset
            if ( station.isFavorite() != favorite ) {
                station.setFavorite( favorite );
                saveStation( station );
            }
        } else if ( favorite ) {                    // Add new preset if setting as favorite
            RadioStation newStation;
            if ( d->mCurrentStation->frequency() == frequency ) {
                newStation = *d->mCurrentStation;
            } else {
                LOG( "CurrentStation frequency mismatch!" );
                newStation.setFrequency( frequency );
            }

            newStation.setType( RadioStation::LocalStation | RadioStation::Favorite );

            // Emit the signals only after adding the preset and reinitializing the current station
            // because the UI will probably query the current station in its slots that get called.
            addStation( newStation );
        }
    }
}

/*!
 * Changes the favorite status of a station by its preset index
 */
void RadioStationModel::setFavoriteByPreset( int presetIndex, bool favorite )
{
    LOG_FORMAT( "RadioStationModel::setFavoriteByPreset, presetIndex: %d", presetIndex );
    RadioStation station;
    if ( findPresetIndex( presetIndex, station ) != RadioStation::NotFound ) {
        station.setFavorite( favorite );
        saveStation( station );
    }
}

/*!
 * Renames a station by its preset index
 */
void RadioStationModel::renameStation( int presetIndex, const QString& name )
{
    LOG_FORMAT( "RadioStationModel::renameStation, presetIndex: %d, name: %s", presetIndex, GETSTRING(name) );
    RadioStation station;
    if ( findPresetIndex( presetIndex, station ) != RadioStation::NotFound ) {
        station.setUserDefinedName( name.left(15) ); // Only 15 characters allowed
        saveStation( station );
    }
}

/*!
 *
 */
void RadioStationModel::setFavorites( const QModelIndexList& favorites )
{
    foreach ( const QModelIndex& index, favorites ) {
        RadioStation station = stationAt( index.row() );
        RADIO_ASSERT( station.isValid() , "RadioStationModel::setFavorites", "invalid RadioStation");
        setFavoriteByPreset( station.presetIndex(), true );
    }
}

/*!
 * Returns the currently tuned station
 */
RadioStation& RadioStationModel::currentStation()
{
    Q_D( RadioStationModel );
    return *d->mCurrentStation;
}

/*!
 * Returns the currently tuned station
 */
const RadioStation& RadioStationModel::currentStation() const
{
    Q_D( const RadioStationModel );
    return *d->mCurrentStation;
}

/*!
 * Sets the model detail level
 */
void RadioStationModel::setDetail( Detail level )
{
    Q_D( RadioStationModel );
    d->mDetailLevel = level;
}

/*!
 * Returns a list of radio stations in the given frequency range
 */
QList<RadioStation> RadioStationModel::stationsInRange( uint minFrequency, uint maxFrequency )
{
    Q_D( RadioStationModel );
    QList<RadioStation> stations;
    foreach( const RadioStation& station, d->mStations ) {
        if ( station.frequency() >= minFrequency && station.frequency() <= maxFrequency ) {
            stations.append( station );
        }
    }

    return stations;
}

/*!
 * Returns the model index corresponding to the given frequency
 */
int RadioStationModel::indexFromFrequency( uint frequency )
{
    RadioStation station;
    if ( findFrequency( frequency, station ) ) {
        return findPresetIndex( station.presetIndex() );
    }
    return -1;
}

/*!
 * Private slot
 * Timer timeout slot to indicate that the dynamic PS check has ended
 */
void RadioStationModel::dynamicPsCheckEnded()
{
    Q_D( RadioStationModel );
    LOG_TIMESTAMP( "Finished dynamic PS check." );
    LOG("RadioStationModel::dynamicPsCheckEnded");
    if ( d->mCurrentStation->psType() != RadioStation::Dynamic && !d->mCurrentStation->dynamicPsText().isEmpty() )
    {
        d->mCurrentStation->setPsType( RadioStation::Static );
        d->mCurrentStation->setName( d->mCurrentStation->dynamicPsText() );
        d->mCurrentStation->setDynamicPsText( "" );
        saveStation( *d->mCurrentStation );
    }
}

/*!
 * Checks the given station and emits signals based on what member variables had been changed
 */
void RadioStationModel::emitChangeSignals( const RadioStation& station, RadioStation::Change flags )
{
    if ( flags.testFlag( RadioStation::NameChanged ) ||
         flags.testFlag( RadioStation::GenreChanged ) ||
         flags.testFlag( RadioStation::UrlChanged ) ||
         flags.testFlag( RadioStation::TypeChanged ) ||
         flags.testFlag( RadioStation::PiCodeChanged ) ) {

        // Create a temporary RadioStation for the duration of the signal-slot processing
        // The receivers can ask the station what data has changed and update accordingly
        RadioStation tempStation( station );
        tempStation.setChangeFlags( flags );
        emit stationDataChanged( tempStation );

        emitDataChanged( tempStation );
    }

    if ( flags.testFlag( RadioStation::RadioTextChanged ) ) {
        emit radioTextReceived( station );
        emitDataChanged( station );
    }

    if ( flags.testFlag( RadioStation::DynamicPsChanged ) ) {
        emit dynamicPsChanged( station );
        emitDataChanged( station );
    }

    if ( flags.testFlag( RadioStation::FavoriteChanged ) && station.isValid() ) {
        emit favoriteChanged( station );
        emitDataChanged( station );
    }
}

/*!
 *
 */
void RadioStationModel::emitDataChanged( const RadioStation& station )
{
    const int row = findPresetIndex( station.presetIndex() );
    QModelIndex top = index( row, 0, QModelIndex() );
    QModelIndex bottom = index( row, 0, QModelIndex() );
    emit dataChanged( top, bottom );
}

/*!
 * Finds an unused preset index
 */
int RadioStationModel::findUnusedPresetIndex()
{
    Q_D( RadioStationModel );
    QList<int> indexes;
    foreach( const RadioStation& station, d->mStations ) {
        if ( station.isValid() ) {
            indexes.append( station.presetIndex() );
        }
    }

    int index = 0;
    for ( ; indexes.contains( index ); ++index ) {
        // Nothing to do here
    }

    LOG_FORMAT( "RadioStationModel::findUnusedPresetIndex, index: %d", index );
    return index;
}
