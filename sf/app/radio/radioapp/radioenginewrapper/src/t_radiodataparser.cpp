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

#include "t_radiodataparser.h"

const int KThousand                     = 1000;
const int KMillion                      = KThousand * KThousand;

const QString KDataFile                 = "./settings.xml";

const QString KXmlMilliseconds          = "ms";
const QString KXmlSeconds               = "s";
const QString KXmlMinutes               = "m";
const QString KXmlRadioSettings         = "RadioSettings";
const QString KXmlMaxVolume             = "maxvolume";
const QString KXmlFrequencyStepSize     = "FrequencyStepSize";
const QString KXmlRegion                = "region";
const QString KXmlMinFrequency          = "MinFrequency";
const QString KXmlMaxFrequency          = "MaxFrequency";

const QString KXmlRadioStations         = "RadioStations";
const QString KXmlSetting               = "setting";
const QString KXmlStation               = "Station";
const QString KXmlName                  = "name";
const QString KXmlValue                 = "value";
const QString KXmlFrequency             = "frequency";
const QString KXmlBooleanTrue           = "TRUE";
const QString KXmlRds                   = "Rds";
const QString KXmlRdsItem               = "RdsItem";
const QString KXmlInterval              = "SendInterval";
const QString KXmlType                  = "type";
const QString KXmlCount                 = "count";
const QString KXmlDelay                 = "delay";

const QString KXmlRdsGroup              = "RdsGroup";
const QString KXmlRdsPsName             = "PsName";
const QString KXmlRdsPiCode             = "PiCode";
const QString KXmlRdsRt                 = "RT";
const QString KXmlRdsRtPlus             = "RtPlus";
const QString KXmlRdsRtPlusItem         = "RtPlusItem";
const QString KXmlRdsRtPlusTag          = "RtPlusTag";
const QString KXmlRdsRtPlusTagDelay     = "delay";
const QString KXmlRdsRtPlusTagArtist    = "artist";
const QString KXmlRdsRtPlusTagTitle     = "title";
const QString KXmlRdsPty                = "Pty";

/*!
 *
 */
static bool isEqual( const QString& first, const QString& second )
{
    return first.compare( second, Qt::CaseInsensitive ) == 0;
}


/*!
 *
 */
T_RadioDataParser::T_RadioDataParser() :
    mReader( new QXmlSimpleReader() )
{
}

/*!
 *
 */
T_RadioDataParser::~T_RadioDataParser()
{
}

/*!
 *
 */
bool T_RadioDataParser::parse()
{
    QFile file( KDataFile );
    if ( file.exists() ) {
        mSource.reset( new QXmlInputSource( &file ) );
        mReader->setContentHandler( this );;
        mReader->setErrorHandler( this );

        return mReader->parse( mSource.data() );
    } else {
        mErrorString = QString( "Data file %1 not found!" ).arg( file.fileName() );
    }
    return false;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::startDocument()
{
    return true;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::endDocument()
{
    return true;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::startElement( const QString& namespaceURI,
                                      const QString& localName,
                                      const QString& qName,
                                      const QXmlAttributes& atts )
{
    if ( isEqual( localName, KXmlRadioSettings ) )
    {
        handleStartRadioSettings( atts );
    }
    else if ( isEqual( localName, KXmlSetting ) )
    {
        handleStartSetting( atts );
    }
    else if ( isEqual( localName, KXmlRadioStations ) )
    {
//        handleStartRdsData( atts );
    }
    else if ( isEqual( localName, KXmlStation ) )
    {
        handleStartStation( atts );
    }
    else if ( isEqual( localName, KXmlRdsGroup ) )
    {
        handleStartRdsGroup( atts );
    }
    else if ( isEqual( localName, KXmlRds ) )
    {
        handleStartRdsData( atts );
    }

    return true;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::endElement( const QString& namespaceURI, const QString& localName, const QString& qName )
{
    if ( isEqual( localName, KXmlRadioSettings ) )
        {
        handleEndRadioSettings();
        }
    else if ( isEqual( localName, KXmlSetting ) )
        {
        handleEndSetting();
        }
    else if ( isEqual( localName, KXmlStation ) )
        {
        handleEndStation();
        }
    else if ( isEqual( localName, KXmlRds ) )
        {
        handleEndRdsData();
        }
    return true;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::characters( const QString& ch )
{
    return true;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::error( const QXmlParseException& exception )
{
    mErrorString = QString( "Line: %1, Column: %2, Msg: %3" )
                   .arg( exception.lineNumber() )
                   .arg( exception.columnNumber() )
                   .arg( exception.message() );
    return true;
}

/*!
 * \reimp
 */
bool T_RadioDataParser::fatalError( const QXmlParseException& exception )
{
    mErrorString = QString( "Error in data XML. Line: %1, Column: %2, Msg: %3" )
                   .arg( exception.lineNumber() )
                   .arg( exception.columnNumber() )
                   .arg( exception.message() );
    return false;
}

/*!
 * \reimp
 */
QString T_RadioDataParser::errorString() const
{
    return mErrorString;
}

/*!
 *
 */
void T_RadioDataParser::handleStartRadioSettings( const QXmlAttributes& atts )
{
    Q_UNUSED( atts );
    RadioData::EngineSettings* engineSettings = new RadioData::EngineSettings;
    pushToSettingStack( engineSettings, mSettingStack );
}

/*!
 *
 */
void T_RadioDataParser::handleEndRadioSettings()
{
    RadioData::EngineSettings* engineSettings
            = static_cast<RadioData::EngineSettings*>( popFromSettingStack( mSettingStack ) );
    if ( engineSettings )
    {
        mEngineSettings = *engineSettings;
        delete engineSettings;
        engineSettings = 0;
    }
}

/*!
 *
 */
void T_RadioDataParser::handleStartSetting( const QXmlAttributes& atts )
{
    RadioData::SettingHolder* holder = new RadioData::SettingHolder;

    RadioData::Setting* parentHolder = topOfSettingStack( mSettingHolderStack );
    if ( parentHolder ) {
        static_cast<RadioData::SettingHolder*>( parentHolder )->mChildren.append( holder );
    }

    pushToSettingStack( holder, mSettingHolderStack );

    for ( int i = 0; i < atts.count(); ++i )
    {
        QString attrName = atts.localName( i );

        if ( isEqual( attrName, KXmlName ) )
        {
            holder->mName = atts.value( i );
        }
        else if ( isEqual( attrName, KXmlValue ) == 0 )
        {
            holder->mValue = atts.value( i );
        }
    }
}

/*!
 *
 */
void T_RadioDataParser::handleEndSetting()
{
    QScopedPointer<RadioData::SettingHolder> holder(
            static_cast<RadioData::SettingHolder*>( popFromSettingStack( mSettingHolderStack ) ) );
    if ( mSettingHolderStack.count() == 0 )
    {
        if ( RadioData::Setting* setting = topOfSettingStack( mSettingStack ) )
        {
            setting->setValue( *holder, *this );
        }
    }
}

/*!
 *
 */
void T_RadioDataParser::handleStartStation( const QXmlAttributes& atts )
{
    RadioData::Station* station = new RadioData::Station;
    pushToSettingStack( station, mSettingStack );
}

/*!
 *
 */
void T_RadioDataParser::handleEndStation()
{
    RadioData::Station* station = static_cast<RadioData::Station*>( popFromSettingStack( mSettingStack ) );
    mStations.append( station );
}

/*!
 *
 */
void T_RadioDataParser::handleStartRdsGroup( const QXmlAttributes& atts )
{

}

/*!
 *
 */
void T_RadioDataParser::handleEndRdsGroup()
{

}

/*!
 *
 */
void T_RadioDataParser::handleStartRdsData( const QXmlAttributes& atts )
{
    RadioData::RdsItem* rds = new RadioData::RdsItem;
    pushToSettingStack( rds, mSettingStack );
}

/*!
 *
 */
void T_RadioDataParser::handleEndRdsData()
{
    QScopedPointer<RadioData::RdsItem> rds(
            static_cast<RadioData::RdsItem*>( popFromSettingStack( mSettingStack ) ) );
    RadioData::Station* station = dynamic_cast<RadioData::Station*>( topOfSettingStack( mSettingStack ) );
    if ( rds && station )
    {
        station->mRdsArray.append( rds.take() );
    }
}

/*!
 *
 */
void T_RadioDataParser::pushToSettingStack( RadioData::Setting* setting, RadioData::SettingArray& array )
{
    array.append( setting );
}

/*!
 *
 */
RadioData::Setting* T_RadioDataParser::topOfSettingStack( RadioData::SettingArray& array )
{
    if ( array.count() > 0 )
        {
        return array[ array.count() - 1 ];
        }

    return 0;
}

/*!
 *
 */
RadioData::Setting* T_RadioDataParser::popFromSettingStack( RadioData::SettingArray& array )
{
    const int lastIndex = array.count() - 1;
    RadioData::Setting* setting = array[lastIndex];
    array.removeAt( lastIndex );
    return setting;
}

/*!
 *
 */
int T_RadioDataParser::parseInt( const QString& string, int defaultVal )
{
    bool ok = false;
    int ret = string.toInt( &ok );
    if ( ok ) {
        return ret;
    }
    return defaultVal;
}

/*!
 *
 */
int T_RadioDataParser::parseTime( const QString& string, int defaultVal )
{
    if ( isEqual( string.right( 2 ), KXmlMilliseconds ) ) {
        QString temp = string.mid( 0, string.length() - 2 );
        return parseInt( temp, defaultVal );
    } else if ( isEqual( string.right( 1 ), KXmlSeconds ) ) {
        QString temp = string.mid( 0, string.length() - 1 );
        return parseInt( temp, defaultVal ) * KThousand;
    } else if ( isEqual( string.right( 1 ), KXmlMinutes ) ) {
        QString temp = string.mid( 0, string.length() - 1 );
        return parseInt( temp, defaultVal ) * KMillion;
    } else {
        // Default time is seconds
        return parseInt( string, defaultVal ) * KThousand;
    }
}


RadioData::RdsGroup::RdsGroup( const QXmlAttributes& atts, T_RadioDataParser& parser ) :
    mInterval( 0 ),
    mCount( 0 )
{
    for ( int i = 0; i < atts.count(); ++i ) {
        QString attrName = atts.localName( i );

        if ( isEqual( attrName, KXmlInterval ) )
        {
            mInterval = parser.parseTime( atts.value( i ), 0 );
        }
        else if ( isEqual( attrName, KXmlCount ) == 0 )
        {
            mCount = parser.parseInt( atts.value( i ), 0 );
        }
    }

}

RadioData::RdsGroup::~RdsGroup()
{

}

RadioData::Station::Station() :
    mFrequency( 0 )
{
}

RadioData::Station::~Station()
{
    qDeleteAll( mRdsArray );
    mRdsArray.clear();
}

void RadioData::Station::setValue( SettingHolder& holder, T_RadioDataParser& parser )
{
    if ( isEqual( holder.mName, KXmlFrequency ) )
    {
        int valueInt = 0;
        parser.parseInt( holder.mValue, valueInt );
        mFrequency = static_cast<uint>( valueInt );
    }
}


RadioData::RdsItem::RdsItem() :
    mCurrentRtPlusIndex( 0 ),
    mType( RadioData::RdsItem::Unknown )
{
}

RadioData::RdsItem::RtPlusHolder::RtPlusHolder() :
    mDelay( 0 ),
    mRtClass( 0 )
{
}

RadioData::RdsItem::~RdsItem()
{
    qDeleteAll( mRtPlusItems );
    mRtPlusItems.clear();
}

void RadioData::RdsItem::setValue( SettingHolder& holder, T_RadioDataParser& parser )
{
    if ( holder.mChildren.count() == 0 )
    {
        if ( isEqual( holder.mName, KXmlInterval ) )
        {
            mInterval = parser.parseInt( holder.mValue, 0 );
        }
        else if ( isEqual( holder.mName, KXmlType ) )
        {
            if ( isEqual( holder.mValue, KXmlRdsPsName ) )
            {
                mType = RadioData::RdsItem::RdsPsName;
            }
            else if ( isEqual( holder.mValue, KXmlRdsRt ) )
            {
                mType = RadioData::RdsItem::RdsRadioText;
            }
            else if ( isEqual( holder.mValue, KXmlRdsRt ) )
            {
                mType = RadioData::RdsItem::RdsRadioText;
            }
            else if ( isEqual( holder.mValue, KXmlRdsRtPlus ) )
            {
                mType = RadioData::RdsItem::RdsRadioTextPlus;
            }
            else if ( isEqual( holder.mValue, KXmlRdsPty ) )
            {
                mType = RadioData::RdsItem::RdsPty;
            }
            if ( isEqual( holder.mValue, KXmlRdsPiCode ) )
            {
                mType = RadioData::RdsItem::RdsPiCode;
            }
        }
        else if ( isEqual( holder.mName, KXmlRdsItem ) )
        {
            mRdsItems.append( holder.mValue );
        }
    }
    else
    {
        initChildren( holder, parser );
    }
}

void RadioData::RdsItem::initChildren( SettingHolder& holder, T_RadioDataParser& parser )
{
    mRdsItems.append( holder.mValue );

    for ( int i = 0; i < holder.mChildren.count(); ++i )
    {
        RtPlusHolder* rtPlusHolder = new RtPlusHolder;
        mRtPlusItems.append( rtPlusHolder );

        SettingHolder* child = holder.mChildren[i];
        if ( isEqual( child->mName, KXmlRdsRtPlusTag ) )
        {
            rtPlusHolder->mRtClass = RadioData::RdsItem::Title;
            if ( isEqual( child->mValue, KXmlRdsRtPlusTagArtist ) )
            {
                rtPlusHolder->mRtClass = RadioData::RdsItem::Artist;
            }
        }

        for ( int j = 0; j < child->mChildren.count(); ++j )
        {
            SettingHolder* grandChild = child->mChildren[j];
            if ( isEqual( grandChild->mName, KXmlRdsRtPlusTagDelay ) )
            {
                parser.parseInt( grandChild->mValue, rtPlusHolder->mDelay );
            }
            else if ( isEqual( grandChild->mName, KXmlRdsItem ) )
            {
                rtPlusHolder->mRtItem = grandChild->mValue;
            }
        }
    }
}

RadioData::SettingHolder::SettingHolder()
{
}

RadioData::SettingHolder::~SettingHolder()
{
    qDeleteAll( mChildren );
    mChildren.clear();
}

RadioData::EngineSettings::EngineSettings() :
    mMaxVolume( 0 ),
    mFrequencyStepSize( 0 ),
    mRegionId( 0 ),
    mMinFrequency( 0 ),
    mMaxFrequency( 0 )
{
}

void RadioData::EngineSettings::setValue( SettingHolder& holder, T_RadioDataParser& parser )
{
    if ( isEqual( holder.mName, KXmlMaxVolume ) )
    {
        parser.parseInt( holder.mValue, mMaxVolume );
    }
    else if ( isEqual( holder.mName, KXmlFrequencyStepSize ) )
    {
        parser.parseInt( holder.mValue, mFrequencyStepSize );
    }
    else if ( isEqual( holder.mName, KXmlRegion ) )
    {
        parser.parseInt( holder.mValue, mRegionId );
    }
    else if ( isEqual( holder.mName, KXmlMinFrequency ) )
    {
        int valueInt = 0;
        parser.parseInt( holder.mValue, valueInt );
        mMinFrequency = static_cast<uint>( valueInt );
    }
    else if ( isEqual( holder.mName, KXmlMaxFrequency ) )
    {
        int valueInt = 0;
        parser.parseInt( holder.mValue, valueInt );
        mMaxFrequency = static_cast<uint>( valueInt );
    }
}
