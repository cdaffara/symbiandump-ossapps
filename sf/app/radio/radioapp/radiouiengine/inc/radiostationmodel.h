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

#ifndef RADIOSTATIONMODEL_H_
#define RADIOSTATIONMODEL_H_

// System includes
#include <QAbstractListModel>
#include <QScopedPointer>
#include <QMap>

// User includes
#include "radio_global.h"
#include "radiouiengineexport.h"
#include "radiostation.h"

// Forward declarations
class RadioStationModelPrivate;
class RadioPresetStorage;
class RadioStationHandlerIf;
class RadioEngineWrapper;
class RadioStation;
class RadioUiEnginePrivate;
class QIcon;

// Constants
typedef QMap<uint,RadioStation> Stations;

namespace FindCriteria
{
    enum Criteria
    {
        OnlySavedStations,
        IncludeManualStation
    };
}

// Class declaration
class UI_ENGINE_DLL_EXPORT RadioStationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr.data(), RadioStationModel )
    Q_DISABLE_COPY( RadioStationModel )

public:

    enum DetailFlag
    {
        Minimal     = 1 << 0,
        ShowIcons   = 1 << 1,
        ShowGenre   = 1 << 2
    };
    Q_DECLARE_FLAGS( Detail, DetailFlag )

    explicit RadioStationModel( RadioUiEnginePrivate& uiEngine );

    ~RadioStationModel();

// from base class QAbstractListModel

    Qt::ItemFlags flags ( const QModelIndex& index ) const;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

// New functions

    /*!
     * Called by the engine to initialize the list with given amount of presets
     */
    void initialize( RadioPresetStorage* storage, RadioEngineWrapper* wrapper );

    /*!
     * Sets the icons to be used in the lists
     */
    void setIcons( const QIcon& favoriteIcon, const QIcon& nonFavoriteIcon, const QIcon& nowPlayingIcon );

    /*!
     * Returns a reference to the station handler interface
     */
    RadioStationHandlerIf& stationHandlerIf();

    /*!
     * Returns a reference to the underlying QList so that it can be easily looped
     */
    const Stations& list() const;

    /*!
     * Returns the station at the given index.
     * The station is returned by value, but it is cheap because it is implicitly shared
     */
    RadioStation stationAt( int index ) const;

    /*!
     * Finds a station by frequency
     */
    bool findFrequency( uint frequency, RadioStation& station, FindCriteria::Criteria criteria = FindCriteria::OnlySavedStations ) const;

    /*!
     * Convenience function to find a radio station.
     * Internally uses findFrequency() and returns an invalid station if the given frequency is not found
     */
    RadioStation findStation( uint frequency, FindCriteria::Criteria criteria = FindCriteria::OnlySavedStations ) const;

    /*!
     * Functions to find stations by preset index
     */
    int findPresetIndex( int presetIndex );
    int findPresetIndex( int presetIndex, RadioStation& station );

    /*!
     * Finds the closest station from the given frequency
     */
    RadioStation findClosest( const uint frequency, StationSkip::Mode mode );

    /*!
     * Checks if the model contains the given frequency
     */
    bool contains( const uint frequency ) const;

    /*!
     * Functions to remove stations
     */
    void removeByFrequency( uint frequency );
    void removeByPresetIndex( int presetIndex );
    void removeStation( const RadioStation& station );
    void removeByModelIndices( QModelIndexList& indices, bool removeFavorites = false );

    /**
     * Removes all stations
     */
    enum RemoveMode{ RemoveAll, RemoveLocalStations, RemoveFavorites  };
    void removeAll( RemoveMode mode = RemoveAll );

    /*!
     * Functions to add and save stations
     */
    void addStation( const RadioStation& station );
    void saveStation( RadioStation& station );

    /*!
     * Function to check the number of favorite stations
     */
    int favoriteCount();

    /*!
    * Function to check the number of local stations
    */
    int localCount();

    /*!
     * Convenience functions to change common settings
     */
    void setFavoriteByFrequency( uint frequency, bool favorite );
    void setFavoriteByPreset( int presetIndex, bool favorite );
    void renameStation( int presetIndex, const QString& name );
    void setFavorites( const QModelIndexList& favorites );

    /*!
     * Functions to init and access the currently tuned station
     */
    RadioStation& currentStation();
    const RadioStation& currentStation() const;

    /*!
     * Sets the model detail level
     */
    void setDetail( Detail level );

    /*!
     * Returns a list of radio stations in the given frequency range
     */
    QList<RadioStation> stationsInRange( uint minFrequency, uint maxFrequency );

    /*!
     * Returns the model index corresponding to the given frequency
     */
    int indexFromFrequency( uint frequency );

signals:

    void stationDataChanged( const RadioStation& station );
    void radioTextReceived( const RadioStation& station );
    void dynamicPsChanged( const RadioStation& station );
    void favoriteChanged( const RadioStation& station );

private slots:

    /*!
     * Timer timeout slot to indicate that the dynamic PS check has ended
     */
    void dynamicPsCheckEnded();

private:

// New functions

    /*!
     * Checks the given station and emits signals based on what member variables had been changed
     * since the last save or reset.
     */
    void emitChangeSignals( const RadioStation& station, RadioStation::Change flags );

    void emitDataChanged( const RadioStation& station );

    /*!
     * Finds an unused preset index
     */
    int findUnusedPresetIndex();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    const QScopedPointer<RadioStationModelPrivate> d_ptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStationModel::Detail )

#endif // RADIOSTATIONMODEL_H_
