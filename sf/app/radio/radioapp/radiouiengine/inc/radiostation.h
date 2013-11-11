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

#ifndef RADIOSTATION_H_
#define RADIOSTATION_H_

// System includes
#include <QObject>
#include <QSharedDataPointer>
#include <QString>
#include <QMetaType>

// User includes
#include "radiouiengineexport.h"
#include "radio_global.h"

// Constants

// Forward declarations
class RadioStationPrivate;

// Class declaration

/*!
 * RadioStation is a data container that holds all information of a radio station.
 * It is implicitly shared to make it cheap to copy and pass around as a signal parameter.
 * It uses the "shared null" idiom to initialize empty instances quickly and efficiently.
 *
 * Note! It is derived from QObject even though architecturally it shouldn't be.
 * It is done only to satisfy the WINSCW build which doesn't export the destructor in UREL
 * build. This causes mismatching def files and annoying warnings. Deriving from QObject fixes this
 */
class UI_ENGINE_DLL_EXPORT RadioStation : public QObject
{
    friend class RadioStationModel;
    friend class RadioStationModelPrivate;
    friend class TestRadioUiEngine;
    friend class TestRadioPresetStorage;

public:

    /**
     * Flags to indicate how the RadioStation has changed since last save or reset.
     * Declared to use QFlags<> to ease flag usage and to enforce type safety
     */
    enum ChangeFlag
    {
         NoChange               = 0
        ,PersistentDataChanged  = 1 << 0
        ,NameChanged            = 1 << 1
        ,FavoriteChanged        = 1 << 2
        ,GenreChanged           = 1 << 3
        ,UrlChanged             = 1 << 4
        ,TypeChanged            = 1 << 5
        ,PiCodeChanged          = 1 << 6
        ,PsTypeChanged          = 1 << 7
        ,RadioTextChanged       = 1 << 8
        ,DynamicPsChanged       = 1 << 9
    };
    Q_DECLARE_FLAGS( Change, ChangeFlag )

    /**
     * Flags ot indicate station type.
     * Declared to use QFlags<> to ease flag usage and to enforce type safety
     */
    enum TypeFlag
    {
       Favorite         = 1 << 0,
       LocalStation     = 1 << 1,
       PreDefined       = 1 << 2,
       ManualStation    = 1 << 3
    };
    Q_DECLARE_FLAGS( Type, TypeFlag )

    /**
     * Flag to indiate whether or not station uses dynamic PS and if the check has been performed
     * Declared to use QFlags<> to ease flag usage and to enforce type safety
     */
    enum PsTypeFlag
    {
        Unknown,
        Dynamic,
        Static
    };
    Q_DECLARE_FLAGS( PsType, PsTypeFlag )

    /**
     * Magical values used as preset indexes to signify certain conditions.
     * NotFound means that a find function could not find a station
     * Invalid means that the station instance has not been initialized
     * SharedNull identifies the empty "null" station that every newly created station points to
     */
    enum PresetFlag { NotFound = -1, Invalid = -100, SharedNull = -200 };

    /**
     * Static convenience function to parse a frequency
     */
    static QString parseFrequency( uint frequency );

    RadioStation();
    RadioStation( const RadioStation& other );

    ~RadioStation();

    RadioStation& operator=( const RadioStation& other );

private:

    explicit RadioStation( int presetIndex, uint frequency );

    void reset();
    void setChangeFlags( Change flags );

    // Setters for persistent data
    void setPresetIndex( int presetIndex );
    void setFrequency( uint frequency );
    void setName( const QString& name );
    void setGenre( const int genre );
    void setUrl( const QString& url );
    bool setPiCode( int piCode, RadioRegion::Region region );

    // Setters for non-persistent data
    void setPsType( PsType psType );
    void setRadioText( const QString& radioText );
    void setRadioTextPlus( const int rtPlusClass, const QString& rtPlusItem );
    void setDynamicPsText( const QString& dynamicPsText );

public: // Getters and setters

    // Setters & Getters for persistent data

    bool isValid() const;

    QString name() const;
    void setUserDefinedName( const QString& name );
    bool isRenamed() const;

    int genre() const;

    QString frequencyString() const;
    uint frequency() const;
    int presetIndex() const;

    void setFavorite( bool favorite );
    bool isFavorite() const;

    QString url() const;

    int piCode() const;

    void setType( RadioStation::Type type );
    void unsetType( RadioStation::Type type );
    bool isType( RadioStation::Type type ) const;

    // Convenience checkers

    inline bool hasPiCode() const       { return piCode() != -1; }
    inline bool hasName() const         { return !name().isEmpty(); }
    inline bool hasUrl() const          { return !url().isEmpty(); }
    inline bool hasRadiotext() const    { return !radioText().isEmpty(); }
    inline bool hasDynamicPs() const    { return !dynamicPsText().isEmpty(); }
    inline bool hasGenre() const        { return genre() != -1; }

    // Getters for non-persistent data

    PsType psType() const;
    QString radioText() const;
    QString dynamicPsText() const;
    Change changeFlags() const;
    bool hasDataChanged( Change flags ) const;
    bool hasChanged() const;
    void resetChangeFlags();
    bool hasSentRds() const;

private:

    // Methods for converting PI code into call sign
    QString piCodeToCallSign( uint programmeIdentification );
    QString iterateCallSign( int piBase, int programmeIdentification );
    QString callSignString( uint programmeIdentification );
    char callSignChar( uint decimalValue );

private: // data

    /**
     * Pointer to the implicitly shared private implementation
     * Own.
     */
    QSharedDataPointer<RadioStationPrivate> mData;

public:

    /**
     * Checks if the class is detached from implicitly shared data
     * Required by many QT convenience functions for implicitly shared classes
     */
    bool isDetached() const;

    typedef QSharedDataPointer<RadioStationPrivate> DataPtr;
    inline DataPtr& data_ptr() { return mData; }

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStation::Change )
Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStation::Type )

Q_DECLARE_TYPEINFO( RadioStation, Q_MOVABLE_TYPE );  // Can be moved around in memory by containers if necessary
Q_DECLARE_SHARED( RadioStation )                     // Uses implicit sharing
Q_DECLARE_METATYPE( RadioStation )                   // To be usable in a QVariant

#endif // RADIOSTATION_H_
