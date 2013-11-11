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

#ifndef T_RADIODATAPARSER_H
#define T_RADIODATAPARSER_H

// System includes
#include <QScopedPointer>
#include <QXxml>
#include <QString>
#include <QList>

// Forward declarations
class T_RadioDataParser;

namespace RadioData
{
    class SettingHolder;

    /**
     * Base class for all settings
     */
    class Setting
    {
    public:

        virtual ~Setting() {}
        virtual void setValue( SettingHolder& holder,
                               T_RadioDataParser& parser ) {};

    };

    /**
     * Holder to store a setting during parsing
     */
    class SettingHolder : public Setting
    {
    public:

        SettingHolder();
        ~SettingHolder();

        QString                 mName;
        QString                 mValue;
        QList<SettingHolder*>   mChildren;

    };

    class RdsItem : public Setting
    {
    public:
        enum Type
            {
             Unknown
            ,RdsPsName
            ,RdsRadioText
            ,RdsRadioTextPlus
            ,RdsPty
            ,RdsPiCode
            };

        RdsItem();
        ~RdsItem();

        void setValue( SettingHolder& holder,
                       T_RadioDataParser& parser );

        void initChildren( SettingHolder& holder,
                           T_RadioDataParser& parser );

        enum rtClass{ Title = 1, Artist = 4, Homepage = 39 };

        class RtPlusHolder
        {
        public:

            RtPlusHolder();

            int     mDelay;
            int     mRtClass;
            QString mRtItem;
        };

        Type                    mType;
        QStringList             mRdsItems;
        QList<RtPlusHolder*>    mRtPlusItems;
        int                     mCurrentRtPlusIndex;

    };

    class RdsGroup : public Setting
    {
    public:

        RdsGroup( const QXmlAttributes& atts, T_RadioDataParser& parser );
        ~RdsGroup();

        int mInterval;
        int mCount;
        QList<RdsItem*> mRdsItems;

    };

    class Station : public Setting
    {
    public:

        Station();
        ~Station();

        void setValue( SettingHolder& holder,
                       T_RadioDataParser& parser );

        uint                mFrequency;
        QList<RdsGroup*>    mRdsArray;

    };

    class EngineSettings : public Setting
    {
    public:

        EngineSettings();

        void setValue( SettingHolder& holder,
                       T_RadioDataParser& parser );

        int     mMaxVolume;
        int     mFrequencyStepSize;
        int     mRegionId;
        uint    mMinFrequency;
        uint    mMaxFrequency;

    };

    typedef QList<Setting*> SettingArray;

}

class T_RadioDataParser : public QXmlDefaultHandler
{
public:

    T_RadioDataParser();
    virtual ~T_RadioDataParser();

    bool parse();

    QString errorString() const;

    int parseInt( const QString& string, int defaultVal );
    int parseTime( const QString& string, int defaultVal );

private:

// from base class QXmlDefaultHandler

    bool startDocument();
    bool endDocument();
    bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
    bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName );
    bool characters( const QString& ch );
    bool error( const QXmlParseException& exception );
    bool fatalError( const QXmlParseException& exception );

// New functions

    void handleStartRadioSettings( const QXmlAttributes& atts );
    void handleEndRadioSettings();
    void handleStartSetting( const QXmlAttributes& atts );
    void handleEndSetting();
    void handleStartStation( const QXmlAttributes& atts );
    void handleEndStation();
    void handleStartRdsGroup( const QXmlAttributes& atts );
    void handleEndRdsGroup();
    void handleStartRdsData( const QXmlAttributes& atts );
    void handleEndRdsData();

    void pushToSettingStack( RadioData::Setting* setting, RadioData::SettingArray& array );
    RadioData::Setting* topOfSettingStack( RadioData::SettingArray& array );
    RadioData::Setting* popFromSettingStack( RadioData::SettingArray& array );

private: // data

    QScopedPointer<QXmlSimpleReader>    mReader;
    QScopedPointer<QXmlInputSource>     mSource;

    QString                             mErrorString;

    int                                 mCurrentPresetIndex;

    RadioData::SettingArray             mSettingStack;

    RadioData::SettingArray             mSettingHolderStack;

public:

    RadioData::EngineSettings           mEngineSettings;

    QList<RadioData::Station*>          mStations;

};

#endif // T_RADIODATAPARSER_H
