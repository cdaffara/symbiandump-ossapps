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

#ifndef RADIOSTATIONINFO_P_H_
#define RADIOSTATIONINFO_P_H_

// System includes
#include <QSharedData>
#include <QTime>

// User includes
#include "radiostation.h"
#include "radiostationif.h"

// Class declaration
class RadioStationPrivate : public QSharedData
                          , public RadioStationIf
{
public:

    /**
     * Flags to indicate various on/off type information of the radio station
     */
    enum StationInfoFlag
    {
       RenamedByUser        = 1 << 0,
       StationSendsRds      = 1 << 1,
       CallSignCheckDone    = 1 << 2
    };
    Q_DECLARE_FLAGS( StationInfo, StationInfoFlag )

    explicit RadioStationPrivate( int presetIndex = RadioStation::Invalid, uint frequency = 0 );
    explicit RadioStationPrivate( const RadioStationPrivate& other );

    virtual ~RadioStationPrivate();

    void init( int presetIndex, uint frequency = 0 );

// from base class RadioStationIf

    int presetIndex() const;
    void setPresetIndex( int presetIndex );
    uint frequency() const;
    void setFrequency( uint frequency );
    QString name() const;
    void setName( QString name );
    bool isRenamedByUser() const;
    void setRenamedByUser( bool renamed );
    int genre() const;
    void setGenre( int genre );
    QString url() const;
    void setUrl( QString url );
    int piCode() const;
    void setPiCode( int piCode );
    bool isFavorite() const;
    void setFavorite( bool favorite );
    bool isLocalStation() const;
    void setLocalStation( bool localStation );
    bool hasStationSentRds() const;
    void setStationHasSentRds( bool hasSentRds );

// New functions

    bool isCallSignCheckDone() const;
    void setCallSignCheckDone( bool checkDone );

public: // data

    // ========================================================================
    // Persistent data. Saved to Central repository
    // ========================================================================

    /**
     * Station index in the Preset Utility.
     * Negative values signify an invalid RadioStation that has not and will not be saved.
     * Initialized to -100 by default to indicate and invalid station.
     */
    int                         mPresetIndex;

    /**
     * Station frequency. The most important bit of information about a radio station.
     * Is checked to be non-null in the isValid() function
     */
    uint                        mFrequency;

    /**
     * Station name. Contains a name that is set in one of the following ways:
     *  1) Received through RDS as the stations PS name and station does not use dynamic PS
     *  2) Entered by the user. In this case the mRenamedByUser flag will be set and the name
     *     cannot be changed until the user removes the renaming.
     *  3) Callsign calculated from the PI code can be set as the station name if the station
     *     does not send RDS information and the current region is America
     */
    QString                     mName;

    /**
     * Station genre received through RDS
     */
    int                         mGenre;

    /**
     * Station URL. Can be e.g. a web address to the radio stations homepage
     */
    QString                     mUrl;

    /**
     * Stations PI code. Only used in the America region to calculate the Callsign if station
     * does not use RDS to broadcast PS name
     */
    int                         mPiCode;

    /**
     * Station type. Used by the application mainly to filter out certain stations depending
     * on the users preference to see all locally audible stations or all favorite stations.
     * Can have any combination of the following flags:
     *  - Favorite      The station is a favorite of the user.
     *  - LocalStation  The station was found in the last scan and should be audible in the current location
     *  - Temporary     Special flag to mark one RadioStation instance that is used to store
     *                  information of a manually tuned frequency that has not been saved by the user.
     */
    RadioStation::Type          mType;    // Station type, favorite or local station or neither

    // ========================================================================
    // Non-persistent data. Only kept in memory
    // ========================================================================

    /**
     * Radio station PS name type. Some radio stations use the PS name broadcasting against the
     * official RDS standard rules to broadcast advertisements. The ads are sent by changing the
     * PS name in relatively rapid succession. The RDS standard specifies that a station should
     * only use the PS name to broadcast a static station name.
     * The FM Radio application attempts to determine if the station uses dynamic PS or not and
     * stores the information here. Stations with a dynamic PS are treated differently in the UI and the
     * name is never saved to Central Repository because it would only contain a part of an advertisement.
     * Can have one of the following values:
     *  - Unknown   Dynamic PS check has not been done so it is unknown if the station uses dynamic PS
     *  - Dynamic   The check has been done and station has been found to use dynamic PS
     *  - Static    The check has been done and the station has been found not to use dynamic PS
     */
    RadioStation::PsType        mPsType;

    /**
     * Stores the Radio Text received through RDS. Can be either normal Radio Text or Radio Text Plus
     * where the tags have been parsed. Parsed Radio Text Plus is stored in HTML form, the normal Radio Text
     * is stored as plain text.
     */
    QString                     mRadioText;

    /**
     * Holder for the ads sent by using dynamic PS. Holds one part of the ad at a time.
     */
    QString                     mDynamicPsText;

    /**
     * Internal book keeping used to determine which member variables have been changed since last save
     * or reset. Used to decide which signals should be sent by the RadioStationModel when the station is saved.
     */
    RadioStation::Change        mChangeFlags;

    /**
     * Time of the last PS name change. Used to check if the station changes its PS name too often.
     * In bad RDS coverage a station using dynamic PS might be incorrectly determined to use static PS.
     * This is a corrective effort to change that decision if the name changes too often.
     */
    QTime                       mLastPsNameChangeTime;

    /**
     * Flags to indicate various on/off type information of the radio station
     */
    StationInfo                 mStationInfo;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStationPrivate::StationInfo )

#endif // RADIOSTATIONINFO_P_H_
