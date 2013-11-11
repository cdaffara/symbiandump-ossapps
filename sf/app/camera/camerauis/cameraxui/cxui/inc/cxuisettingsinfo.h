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

#ifndef CXUISETTINGSINFO_H
#define CXUISETTINGSINFO_H

/*!
* @file
* @brief CxUi settings related classes and constants.
*/

#include <QObject>
#include <QMap>
#include <QPair>
#include <QList>
#include <QString>
#include <QStringList>
#include "cxeerror.h"
#include "cxuisettingradiobuttonlist.h"


class CxeEngine;
class CxuiSettingXmlReader;

/*!
* @namespace CxUiSettings
* @brief Namespace for UI settings related constants and classes.
*/
namespace CxUiSettings
{
    /*! Image setting definitions. */
    static const char *IMAGE_SETTING_MAPPING_FILE = ":/xml/image_setting.xml";
    /*! Video setting definitions. */
    static const char *VIDEO_SETTING_MAPPING_FILE = ":/xml/video_setting.xml";

    /*!
    * Character used to separate lines within a single string.
    * E.g. SettingItem::mItem can be split with this character to get multi-line item.
    */
    static const char NEW_LINE_CHAR = '\n';

    struct SettingItem
    {
        QString mItem;
        QVariant mValue;
        QString mIcon;
    };

    /*!
    * Class that holds all the data required for radiobutton list
    */
    class RadioButtonListParams
    {
    public:
        RadioButtonListParams& operator=(const RadioButtonListParams& other);

    public:
        QString mHeading;
        QString mSettingId;
        QString mHeadingIcon;
        QList<SettingItem> mSettingPairList;
        bool mPreview;
        CxuiSettingRadioButtonList::ListBoxType mListboxType;
    };


    /*!
    * Class that holds all the data required for slider
    */
    class SliderParams
    {
    public:
        SliderParams& operator=(const SliderParams& other);

    public:
        QString mHeading;
        QString mSettingId;
        QString mHeadingIcon;
        QStringList mSettingStrings;
        QPair<int, int> mRange;
        qreal mMinorStep;
        qreal mMajorStep;
    };
}

/**
 * Model Class that handles all HbDialog settings content widget info
 */
class CxuiSettingsInfo : public QObject
{
    Q_OBJECT
public:
   CxuiSettingsInfo(CxeEngine *engine);
   ~CxuiSettingsInfo();
   bool getSettingsContent(const QString &key, CxUiSettings::RadioButtonListParams &p);
   bool getSettingsContent(const QString &key, CxUiSettings::SliderParams &p);

private:
    void getImageQualitySettings(CxUiSettings::RadioButtonListParams &settings);
    void getVideoQualitySettings(CxUiSettings::RadioButtonListParams &settings);
    void handleQualitySettings(const QString &key, CxUiSettings::RadioButtonListParams &settings);

private:
    void checkMode();

private: // data members
    int mMode;
    CxeEngine *mEngine;
    CxuiSettingXmlReader *mXmlReader;
};


#endif // CXUISETTINGSINFO_H
