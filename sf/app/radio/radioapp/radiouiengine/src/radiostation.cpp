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
#include <QTime>

// User includes
#include "radiostation.h"
#include "radiostation_p.h"
#include "radiologger.h"
#include "radio_global.h"

// Constants
const QLatin1String HTML_TAG_START( "<font color='cyan'><u>" );
const QLatin1String HTML_TAG_END( "</u></font>" );

const uint LAST_CALLSIGN_CHAR_CODE = 25;
const uint THREE_LETTER_CALLSIGN_COUNT = 72;
const uint KXXX_CALLSIGN_PI_FIRST = 0x1000;
const uint WXXX_CALLSIGN_PI_FIRST = 0x54A8;
const uint WXXX_CALLSIGN_PI_LAST = 0x994F;
const uint XXX_CALLSIGN_PI_FIRST = 0x9950;
const uint XXX_CALLSIGN_PI_LAST = 0x99B9;

const char* CALLSIGN_TABLE[THREE_LETTER_CALLSIGN_COUNT] =
   {"KBW", "KCY", "KDB", "KDF", "KEX", "KFH","KFI","KGA","KGB",
    "KGO", "KGU", "KGW", "KGY", "KHQ", "KID", "KIT", "KJR", "KLO",
    "KLZ", "KMA", "KMJ", "KNX", "KOA", "KOB", "KOY", "KPQ", "KQV",
    "KSD", "KSL", "KUJ", "KUT", "KVI", "KWG", "KXL", "KXO", "KYW",
    "WBT", "WBZ", "WDZ", "WEW", "WGH", "WGL", "WGN", "WGR", "WGY",
    "WHA", "WHB", "WHK", "WHO", "WHP", "WIL", "WIP", "WIS", "WJR",
    "WJW", "WJZ", "WKY", "WLS", "WLW", "WMC", "WMT", "WOC", "WOI",
    "WOL", "WOR", "WOW", "WRC", "WRR", "WSB", "WSM", "WWJ", "WWL"};

const uint PI_CODE_TABLE[THREE_LETTER_CALLSIGN_COUNT] =
   {0x99A5, 0x99A6, 0x9990, 0x99A7, 0x9950, 0x9951, 0x9952, 0x9953,
    0x9991, 0x9954, 0x9955, 0x9956, 0x9957, 0x99AA, 0x9958, 0x9959,
    0x995A, 0x995B, 0x995C, 0x995D, 0x995E, 0x995F, 0x9960, 0x99AB,
    0x9992, 0x9993, 0x9964, 0x9994, 0x9965, 0x9966, 0x9995, 0x9967,
    0x9968, 0x9996, 0x9997, 0x996B, 0x9999, 0x996D, 0x996E, 0x996F,
    0x999A, 0x9971, 0x9972, 0x9973, 0x999B, 0x9975, 0x9976, 0x9977,
    0x9978, 0x999C, 0x999D, 0x997A, 0x99B3, 0x997B, 0x99B4, 0x99B5,
    0x997C, 0x997D, 0x997E, 0x999E, 0x999F, 0x9981, 0x99A0, 0x9983,
    0x9984, 0x99A1, 0x99B9, 0x99A2, 0x99A3, 0x99A4, 0x9988, 0x9989};

const uint DISABLE_LOCAL_AREA_COVERAGE_MASK = 0x0800;

const int PS_NAME_CHANGE_THRESHOLD_SECONDS = 10;

// Macros to help protect shared null and manual station instances to help debugging
// Enabled in emulator or win32 builds by default
#if defined __WINS__ || defined BUILD_WIN32
#   define PROTECT_SHAREDNULL_AND_MANUALSTATION
#endif

#ifdef PROTECT_SHAREDNULL_AND_MANUALSTATION
#   define ASSERT_SHARED_NULL_IS_INTACT \
        Q_ASSERT_X( mData->mPresetIndex != SharedNull, "RadioStation", "Shared null modified illegally!" );
#   define ASSERT_MANUAL_STATION \
        Q_ASSERT_X( mData->mPresetIndex != ManualStation, "RadioStation", "Illegally modifying manual station" );
#else
#   define ASSERT_SHARED_NULL_IS_INTACT
#   define ASSERT_MANUAL_STATION
#endif // PROTECT_SHAREDNULL_AND_MANUALSTATION

/**
 * Static shared data instance that is used by all default-constructed RadioStation instances
 */
static RadioStationPrivate shared_null( RadioStation::SharedNull, 0 );

/*!
 *
 */
QString RadioStation::parseFrequency( uint frequency )
{
    QString freqString;
    freqString.sprintf( "%.1f", qreal( frequency ) / FREQUENCY_MULTIPLIER );
    return freqString;
}

/*!
 *
 */
RadioStation::RadioStation() :
    mData( &shared_null )
{
}

/*!
 *
 */
RadioStation::RadioStation( const RadioStation& other ) :
    mData( other.mData )
{
}

/*!
 *
 */
RadioStation::RadioStation( int presetIndex, uint frequency ) :
    mData( new RadioStationPrivate( presetIndex, frequency ) )
{
}

/*!
 *
 */
RadioStation::~RadioStation()
{
}

/*!
 *
 */
RadioStation& RadioStation::operator=( const RadioStation& other )
{
    mData = other.mData;
    return *this;
}

/*!
 *
 */
void RadioStation::reset()
{
    mData = &shared_null;
    ASSERT_SHARED_NULL_IS_INTACT
}

/*!
 *
 */
void RadioStation::setChangeFlags( RadioStation::Change flags )
{
    if ( mData->mChangeFlags != flags ) {
        mData->mChangeFlags = flags;
    }
}

/*!
 *
 */
void RadioStation::setPresetIndex( int presetIndex )
{
    if ( mData->mPresetIndex != presetIndex ) {
        ASSERT_MANUAL_STATION
        mData->mPresetIndex = presetIndex;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged;
    }
}

/*!
 *
 */
void RadioStation::setFrequency( uint frequency )
{
    if ( mData->mFrequency != frequency ) {
        mData->mFrequency = frequency;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 * Sets the preset name
 */
void RadioStation::setName( const QString& name )
{
    // Name emptiness is checked because this name setter is used by incoming RDS PS name
    // and empty names should be ignored
    if ( !name.isEmpty() && !mData->isRenamedByUser() && mData->mName.compare( name.trimmed() ) != 0 ) {
        mData->mName = name.trimmed();
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::NameChanged;
        mData->setStationHasSentRds( true );

        // Save the time when PS name changed and if the last change was too close to the current time
        // change the PS type to dynamic if it has already been incorrectly determined to be static.
        QTime previousChange = mData->mLastPsNameChangeTime;
        mData->mLastPsNameChangeTime = QTime::currentTime();
        if ( previousChange.isValid() && mData->mPsType == RadioStation::Static &&
             previousChange.secsTo( mData->mLastPsNameChangeTime ) < PS_NAME_CHANGE_THRESHOLD_SECONDS ) {
            LOG( "Station changed PS name too often. PS type changed to Dynamic" );
            mData->mPsType = RadioStation::Dynamic;
            mData->mDynamicPsText = mData->mName;
            mData->mName = "";
            mData->mChangeFlags |= RadioStation::PsTypeChanged | RadioStation::DynamicPsChanged;
            mData->setCallSignCheckDone( false );
        }

        //TODO: This is a temporary thing to see some URL. Remove this
        if ( !mData->mName.isEmpty() ) {
            QString url = mData->mName.toLower().remove( " " );
            mData->mUrl = "http://www." + url + ".fi";
        } else {
            mData->mUrl = "";
        }
        mData->mChangeFlags |= RadioStation::UrlChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
void RadioStation::setGenre( const int genre )
{
    if ( mData->mGenre != genre ) {
        mData->mGenre = genre;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::GenreChanged;

        // Note that setStationHasSentRds() is not set when genre or pi code arrives.

        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
void RadioStation::setUrl( const QString& url )
{
    if ( mData->mUrl.compare( url ) != 0 ) {
        mData->mUrl = url;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::UrlChanged;
        mData->setStationHasSentRds( true );
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 * Sets the PI code
 * @return true if code was changed, false if not
 */
bool RadioStation::setPiCode( int piCode, RadioRegion::Region region )
{
    LOG_FORMAT( "RadioStation::setPiCode new PI: %d", piCode );
    // toggling local area coverage bit code must not be interpreted as new PI code
    if ( region != RadioRegion::America ) {
        piCode &= ~DISABLE_LOCAL_AREA_COVERAGE_MASK;
    }

    LOG_FORMAT( "stored PI: %d", mData->mPiCode );
    LOG_FORMAT( "call sign check done: %d", mData->isCallSignCheckDone() );
    //prevent executing the below code when unnessesary
    if ( mData->mPiCode != piCode || !mData->isCallSignCheckDone() ) {
        mData->mPiCode = piCode;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::PiCodeChanged;
        // Note that setStationHasSentRds() is not set when genre or pi code arrives.

        // call sign not calculated for clear channel stations
		//TODO: Remove magic numbers
        if ( ( (mData->mPiCode & 0xF000 ) >> 12 ) == 0x1 ) {
            LOG( "Clear channel station" );
            mData->setCallSignCheckDone( true );
        } else if ( region == RadioRegion::America && mData->mName.isEmpty() && !isRenamed() ) {
            LOG( "Calculate call sign" );
            mData->mName = piCodeToCallSign( mData->mPiCode );
            mData->mChangeFlags |= RadioStation::NameChanged;
        }

        if ( mData->mChangeFlags.testFlag( RadioStation::PsTypeChanged ) ) {
            LOG( "Call sign check done" );
            mData->setCallSignCheckDone( true );
        }

        ASSERT_SHARED_NULL_IS_INTACT
        return true;
    }
    return false;
}

/*!
 *
 */
void RadioStation::setPsType( PsType psType )
{
    if ( mData->mPsType != psType ) {
        mData->mPsType = psType;
        mData->mChangeFlags |= RadioStation::PsTypeChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
void RadioStation::setRadioText( const QString& radioText )
{
    if ( mData->mRadioText.compare( radioText ) != 0 ) {
        mData->mRadioText = radioText.isEmpty() ? "" : radioText.trimmed();
        mData->mChangeFlags |= RadioStation::RadioTextChanged;
        mData->setStationHasSentRds( true );
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
void RadioStation::setRadioTextPlus( const int rtPlusClass, const QString& rtPlusItem )
{
    if ( !mData->mRadioText.isEmpty() &&
         !rtPlusItem.isEmpty() &&
         ( rtPlusClass == RtPlus::Artist || rtPlusClass == RtPlus::Title || rtPlusClass == RtPlus::Homepage) )
    {
        // Url is saved to its own variable and it is not highlighted from the radiotext
        if ( rtPlusClass == RtPlus::Homepage ) {
            setUrl( rtPlusItem );
            return;
        }

        const QString replacement = HTML_TAG_START + rtPlusItem + HTML_TAG_END;

        mData->mRadioText.replace( rtPlusItem, replacement );
        mData->mChangeFlags |= RadioStation::RadioTextChanged;
    }
}

/*!
 *
 */
void RadioStation::setDynamicPsText( const QString& dynamicPsText )
{
    if ( mData->mDynamicPsText.compare( dynamicPsText ) != 0 ) {
        mData->mDynamicPsText = dynamicPsText;
        mData->mChangeFlags |= RadioStation::DynamicPsChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
bool RadioStation::isValid() const
{
    return mData->mFrequency > 0 && ( isType( ManualStation ) || mData->mPresetIndex >= 0 );
}

/*!
 *
 */
QString RadioStation::name() const
{
    return mData->mName.isEmpty() ? "" : mData->mName;
}

/*!
 *
 */
void RadioStation::setUserDefinedName( const QString& name )
{
    // We don't check for name emptiness because this setter is used also to remove the renaming
    // of a station by setting an empty name
    if ( mData->mName.compare( name ) != 0 ) {
        mData->mName = name;
        mData->setRenamedByUser( !name.isEmpty() );
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::NameChanged;
    }
}

/*!
 *
 */
bool RadioStation::isRenamed() const
{
    return mData->isRenamedByUser();
}

/*!
 *
 */
int RadioStation::genre() const
{
    return mData->mGenre;
}

/*!
 *
 */
QString RadioStation::frequencyString() const
{
    return parseFrequency( mData->mFrequency );
}

/*!
 *
 */
uint RadioStation::frequency() const
{
    return mData->mFrequency;
}

/*!
 *
 */
int RadioStation::presetIndex() const
{
    return mData->mPresetIndex;
}

/*!
 *
 */
void RadioStation::setFavorite( bool favorite )
{
    if ( isFavorite() != favorite ) {
        favorite ? setType( RadioStation::Favorite ) : unsetType( RadioStation::Favorite );
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::FavoriteChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
bool RadioStation::isFavorite() const
{
    return mData->mType.testFlag( RadioStation::Favorite );
}

/*!
 *
 */
QString RadioStation::url() const
{
    return mData->mUrl;
}

/*!
 *
 */
int RadioStation::piCode() const
{
    return mData->mPiCode;
}

/*!
 *
 */
void RadioStation::setType( RadioStation::Type type )
{
    if ( !isType( type ) ) {

        // Check if favorite-status changed
        if ( mData->mType.testFlag( RadioStation::Favorite ) != type.testFlag( RadioStation::Favorite ) ) {
            mData->mChangeFlags |= RadioStation::FavoriteChanged;
        }

        mData->mType |= type;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::TypeChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
void RadioStation::unsetType( RadioStation::Type type )
{
    if ( isType( type ) ) {

        // Check if favorite-status changed
        if ( mData->mType.testFlag( RadioStation::Favorite ) != type.testFlag( RadioStation::Favorite ) ) {
            mData->mChangeFlags |= RadioStation::FavoriteChanged;
        }

        mData->mType &= ~type;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::TypeChanged;
        ASSERT_SHARED_NULL_IS_INTACT
    }
}

/*!
 *
 */
bool RadioStation::isType( RadioStation::Type type ) const
{
    return ( mData->mType & type ) == type;
}

/*!
 *
 */
RadioStation::PsType RadioStation::psType() const
{
    return mData->mPsType;
}

/*!
 *
 */
QString RadioStation::radioText() const
{
    return mData->mRadioText;
}

/*!
 *
 */
QString RadioStation::dynamicPsText() const
{
    return mData->mDynamicPsText;
}

/*!
 *
 */
RadioStation::Change RadioStation::changeFlags() const
{
    return mData->mChangeFlags;
}

/*!
 *
 */
bool RadioStation::hasDataChanged( RadioStation::Change flags ) const
{
    return ( mData->mChangeFlags & flags ) == flags;
}

/*!
 *
 */
bool RadioStation::hasChanged() const
{
    return mData->mChangeFlags != RadioStation::NoChange;
}

/*!
 *
 */
void RadioStation::resetChangeFlags()
{
    if ( mData->mChangeFlags != RadioStation::NoChange ) {
        mData->mChangeFlags = RadioStation::NoChange;
    }
}

/*!
 *
 */
bool RadioStation::hasSentRds() const
{
    return mData->hasStationSentRds();
}

/*!
 *
 */
 QString RadioStation::piCodeToCallSign( uint programmeIdentification )
 {
    QString callSign;

    LOG_FORMAT( "RadioStation::piCodeToCallSign PI: %d", programmeIdentification );
    // call signs beginning with 'K'
    if ( ( programmeIdentification>=KXXX_CALLSIGN_PI_FIRST ) && ( programmeIdentification < WXXX_CALLSIGN_PI_FIRST ) ) {
        callSign += "K";
        callSign += iterateCallSign( KXXX_CALLSIGN_PI_FIRST, programmeIdentification );
    }
    // call signs beginning with 'W'
    else if ( ( programmeIdentification >= WXXX_CALLSIGN_PI_FIRST ) && ( programmeIdentification <= WXXX_CALLSIGN_PI_LAST ) ) {
        callSign += "W";
        callSign += iterateCallSign( WXXX_CALLSIGN_PI_FIRST, programmeIdentification );
    }
    // 3 letter only call signs
    else if ( ( programmeIdentification >= XXX_CALLSIGN_PI_FIRST ) && ( programmeIdentification <= XXX_CALLSIGN_PI_LAST ) ) {
        callSign += callSignString( programmeIdentification );
    } else {
        LOG( "RadioStation::piCodeToCallSign - Unhandled else" );
    }

    LOG_FORMAT( "RadioStation::piCodeToCallSign, call sign: %s", GETSTRING(callSign) );

    return callSign;
}

/*!
 *
 */
QString RadioStation::iterateCallSign( int piBase, int programmeIdentification )
{
    QString callSign;
    LOG_FORMAT( "RadioStation::iterateCallSign base: %d", piBase );

    int sum = 0;
    int i = 0;
    while( sum < programmeIdentification ) {
        ++i;
        sum = piBase + i * 676 + 0 + 0;
    }
    callSign += callSignChar( i - 1 );

    int tmpSum( sum - 676 );
    sum -= 676;
    i = 0;
    while( sum <= programmeIdentification ) {
        ++i;
        sum = tmpSum + 0 + i * 26 + 0;
    }
    callSign += callSignChar( i - 1 );

    tmpSum = sum - 26;
    sum -= 26;
    i = 0;
    while( sum <= programmeIdentification ) {
        ++i;
        sum = tmpSum + 0 + 0 + i;
    }
    callSign += callSignChar( i - 1 );

    return callSign;
    }

/*!
 *
 */
QString RadioStation::callSignString( uint programmeIdentification )
{
    for ( uint i = 0; i < THREE_LETTER_CALLSIGN_COUNT; ++i ) {
        if( PI_CODE_TABLE[i] == programmeIdentification ) {
            return CALLSIGN_TABLE[i];
        }
    }

    LOG_FORMAT( "RadioStation::callSignString, Not found PI: %d", programmeIdentification );

    return QString( "????" );
}

/*!
 *
 */
char RadioStation::callSignChar( uint decimalValue )
{
    LOG_FORMAT( "RadioStation::callSignChar A+: %d", decimalValue );
    if ( decimalValue <= LAST_CALLSIGN_CHAR_CODE ) {
        return static_cast<char>( 'A' + decimalValue );
    }
    return '?';
}

/**
 * Checks if the class is detached from implicitly shared data
 */
bool RadioStation::isDetached() const
{
    return mData->ref == 1;
}
