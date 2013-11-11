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

#ifndef RADIO_GLOBAL_H_
#define RADIO_GLOBAL_H_

// System includes
#include <QtGlobal>
#include <qnamespace.h>

// Constants

/**
 * Radio frequency multiplier
 */
const int FREQUENCY_MULTIPLIER = 1000000;

/**
 * Desired amount of steps in the volume control in the UI
 */
const int MAXIMUM_VOLUME_LEVEL = 20;

/**
 * Default volume level that is used when no previous setting exists
 */
const int DEFAULT_VOLUME_LEVEL = 4;

const int VOLUME_STEP_DIVIDER = 500;

/**
 * UIDs for checking the offline mode
 * Values copied from CoreApplicationUIsSDKCRKeys.h
 */
const long int CENREP_CORE_APPLICATION_UIS = 0x101F876C;
const unsigned long int ID_NETWORK_CONNECTION_ALLOWED = 0x00000001;
enum NetworkConnectionAllowed { NetworkNotAllowed = 0, NetworkAllowed };

/**
 * Roles used when interacting with the radio station model
 */
namespace RadioRole
{
    enum Role
    {
        RadioStationRole = Qt::UserRole + 1,
        ToggleFavoriteRole,
        IsFavoriteRole
    };
}

/**
 * Radio Text Plus tag ids
 */
namespace RtPlus
{
    enum Tag
    {
        Dummy = 0,
        Title = 1,
        Artist = 4,
        Band = 9,
        Homepage = 39
    };
}

namespace GenreTarget
{
    enum Target
    {
        Carousel,
        StationsList,
        HomeScreen
    };
}

namespace Seek
{
    enum Direction
    {
        Down,
        Up
    };

    enum State
    {
        NotSeeking,
        SeekingUp,
        SeekingDown
    };
}

namespace Scan
{
    enum Status
    {
        NotScanning,
        ScanningInMainView,
        ScanningInStationsView
    };
}

namespace StationSkip
{
    enum Mode
    {
        Previous,
        Next,
        PreviousFavorite,
        NextFavorite
    };
}

namespace TuneReason
{
    enum Reason
    {
        Unspecified,
        FrequencyStrip,
        ManualSeekUpdate,
        ManualSeekTune,
        StationCarousel,
        StationsList,
        Skip,
        Seek,
        StationScanInitialization,
        StationScan,
        StationScanFinalize,
        StationScanNoStationsFound,
        SkipFromEngine,
        SkipFromWidget
    };
}

namespace RadioRegion
{
    enum Region
    {
        None = -1,
        Default,
        Japan,
        America,
        Poland
    };
}

namespace GenreEurope
{
    enum Europe
    {
        RdsNone,
        RdsNews,
        RdsCurrentAffairs,
        RdsInformation,
        RdsSport,
        RdsEducation,
        RdsDrama,
        RdsCulture,
        RdsScience,
        RdsVariedSpeech,
        RdsPopMusic,
        RdsRockMusic,
        RdsEasyListening,
        RdsLightClassical,
        RdsSeriousClassical,
        RdsOtherMusic,
        RdsWeather,
        RdsFinance,
        RdsChildrensProgrammes,
        RdsSocialAffairs,
        RdsReligion,
        RdsPhoneIn,
        RdsTravel,
        RdsLeisure,
        RdsJazzMusic,
        RdsCountryMusic,
        RdsNationalMusic,
        RdsOldiesMusic,
        RdsFolkMusic,
        RdsDocumentary,
        RdsAlarmTest,
        RdsAlarm
    };
}

namespace GenreAmerica
{
    enum America
    {
        RbdsNone,
        RbdsNews,
        RbdsInformation,
        RbdsSports,
        RbdsTalk,
        RbdsRock,
        RbdsClassicRock,
        RbdsAdultHits,
        RbdsSoftRock,
        RbdsTop40,
        RbdsCountry,
        RbdsOldies,
        RbdsSoft,
        RbdsNostalgia,
        RbdsJazz,
        RbdsClassical,
        RbdsRhythmAndBlues,
        RbdsSoftRhythmAndBlues,
        RbdsLanguage,
        RbdsReligiousMusic,
        RbdsReligiousTalk,
        RbdsPersonality,
        RbdsPublic,
        RbdsCollege,
        RbdsUnassigned1,
        RbdsUnassigned2,
        RbdsUnassigned3,
        RbdsUnassigned4,
        RbdsUnassigned5,
        RbdsWeather,
        RbdsEmergencyTest,
        RbdsEmergency
    };
}

#endif // RADIO_GLOBAL_H_
