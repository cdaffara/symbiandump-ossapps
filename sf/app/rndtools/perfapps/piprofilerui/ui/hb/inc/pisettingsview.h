/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PISETTINGSVIEW_H
#define PISETTINGSVIEW_H

#include <hbview.h>
#include <qlist.h>

#include "generalattributes.h"

class PIProfilerEngine;
class HbMainWindow;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class PluginAttributes;
class SettingItem;

class PISettingsView: public HbView
{
Q_OBJECT

public:

    enum OutputMode
    {
        OUTPUT_MODE_DEBUG, OUTPUT_MODE_FILE
    };

    /**
     * Constructor
     */
    PISettingsView(PIProfilerEngine *engine);

    /**
     * Destructor
     */
    ~PISettingsView();

    /**
     * Reads general settings from data form and returns GeneralAttributes
     */
    GeneralAttributes& getGeneralAttributes();

    /**
     * Reads plug-in specific settings from data form and saves them into mPluginlist
     */
    void readPluginSettings();

    /**
     * Sets plug-in specific settings and creates data form items for them
     */
    void setPluginSettings(QList<PluginAttributes> *pluginList);

    /**
     * Expands one plug-ins settings group. Other groups are collapsed. 
     */
    void expandPluginGroup(int uid);

private:

    /**
     * Creates general settings form items
     */
    void createGeneralSettingForms();

    /**
     * Creates plug-in specific form items
     */
    void createPluginSpecificSettings();

    /**
     * Loads general settings and updates them into form
     */
    void loadGeneralSettings();

    /**
     * Creates one plug-in setting item
     */
    HbDataFormModelItem *createPluginSettingItem(const SettingItem *item,
        HbDataFormModelItem *parentGroup);
    
    /**
     * reads ont plug-in setting value and saves it into qstring value
     */
    void readPluginSettingItemValue(HbDataFormModelItem *dataFormItem, SettingItem *settingItem);

private slots:

    /**
     * Adds text validators into form items
     */
    void addTextValidators(const QModelIndex index);

private:

    // engine pointer
    PIProfilerEngine *mEngine;

    // General settings
    GeneralAttributes mGeneralAttributes;

    // list of plug-ins
    QList<PluginAttributes> *mPluginList;

    // setting form
    HbDataForm *mSettingForm;

    // model class
    HbDataFormModel *mModel;

    // data form groups
    HbDataFormModelItem *mGeneralSettingsGroup;
    HbDataFormModelItem *mPluginSettingsGroup;

    // general setting items
    HbDataFormModelItem *mOutputItem;
    HbDataFormModelItem *mFileLocation;
    HbDataFormModelItem *mPrefix;
    HbDataFormModelItem *mSamplingPeriod;

    HbDataFormModelItem *testItem;//TODO remove!
};

#endif // PISETTINGSVIEW_H
