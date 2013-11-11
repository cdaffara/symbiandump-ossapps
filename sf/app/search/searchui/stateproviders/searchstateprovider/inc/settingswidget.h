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
 * Description:  Declaration of the settings widget
 *
 */
#ifndef SETTINGSWIDGET_H_
#define SETTINGSWIDGET_H_

#include <hbwidget.h>
#include <qlist.h>
#include <hbgridview.h>
#include <search_global.h>
#include <hbicon.h>
class OnlineHandler;
class HbCheckBox;
class HbDialog;
class QSignalMapper;
class HbDocumentLoader;
class HbDataFormModel;
class HbDataForm;
class HbRadioButtonList;
class HbDataFormModelItem;
class HbComboBox;
class SearchUiLoader;
SEARCH_CLASS(SearchStateProviderTest)

class SettingsWidget : public QObject
    {
Q_OBJECT
public:
    /**
     * Construction
     */
    SettingsWidget();
    /**
     * destructor
     */
    ~SettingsWidget();
public:

    /**
     * Launching the settings widget
     */
    void launchSettingWidget();

    /**
     * returns true is delimeter is on internet mode otherwise false
     */
    bool isInternetSearchOptionSelected();

    /**
     * unchecking subcategories under the main category once selectall unchecked
     */
    void unCheckSubCategories();

    /**
     * checking subcategories under the main category once selectall checked
     */
    void checkSubCategories();

    /**
     * storing settings to application ini file
     */
    void storeSettingsToiniFile();

    /**
     * Load setting from application ini file      
     */
    void loadDeviceSettings();

    /**
     * Load settings whether mode in internet/indevice
     */
    void loadBaseSettings();

    /**
     * make "OK" button visible 
     */
    void setActionVisibility();

    /**
     * enable default settings in the application ini file
     */
    void storeDefaultSettings();

    /**
     * setting up gui for the settings widget
     */
    void createGui();

    /**
     * Filter out categories from the sql database provided by the engine
     * for only those categories that have been successfully harvested
     */
    void preparecategories();

    /**
     * initilize the settings wizard while entering to the setting state
     */
    void initialize();

    /**
     * Load Internet handler and settings 
     */
    void loadIS();

public slots:

    /**
     * will be called when settings OK is clicked
     */
    void slotOkEvent();

    /**
     * will be called when setting cancel is called
     */
    void slotCancelEvent();

    /**
     * will be called when any check box is checked
     */
    void slotstateChanged(int);

    /**
     * slot called while clicking items added to  the combobox
     */
    void slotcurrentIndexChanged(int);

    /**
     * to get the details of intenet service provider
     */
    void slotproviderDetails(QString, HbIcon, int);

    /**
     * to get the details of default intenet service provider
     */
    void slotdefaultProvider(const int);

    /**
     * to get notify about selection of internet provider
     */
    void slotitemSelected(int);

    /**
     * to get notify about contentinfodb changes
     */
    void slotdbChanged();

signals:

    /**
     * Emitted when setting closed
     */
    void settingsEvent(bool);

    /**
     * to notify the indevice search state to display suggestion links
     */
    void ISProvidersIcon(HbIcon, int);

private:

    /**
     * for Device category list
     */
    QList<HbCheckBox*> mDeviceCheckBoxList;

    /**
     * for Internet category list
     */
    QList<HbCheckBox*> mInternetCheckBoxList;

    /**
     * mapper to map checkboxes 
     */
    QSignalMapper* signalMapper;

    /**
     * settings widget popup
     */
    HbDialog *popup;

    /**
     * to indicate internet is selected or not
     */
    bool isInternetSelected;

    /**
     * list holds the localised text id's of category name
     */
    QStringList mDeviceListDisplay;

    /**
     * list holds the localised string of category name
     */
    QStringList mDeviceStoragelist;

    /**
     * lst holds the modes (indevice/internet)
     */
    QStringList mCategoryList;

    /**
     * to load the docml and ui objects
     */
    HbDocumentLoader* mDocumentLoader;

    /**
     * Dataform for the settingwidget
     */
    HbDataForm* dataform;

    /**
     * Dataform model
     */
    HbDataFormModel* mModel;

    /**
     * variable for customizing each item the in the dataform
     */
    QList<HbDataFormModelItem*> mModelItemList;

    /**
     * primary action for delimeter popup
     */
    QAction* primaryAction;

    /**
     * secondary action for delimeter popup
     */
    QAction* secondaryAction;

    /**
     * hold the setting value of category mode 
     */
    int mSelectedScope;

    /**
     * hold the Id of internet service provider
     */
    int mSelectedProvider;

    /**
     * Radio buttion list for internet search categories
     */
    HbRadioButtonList* mradiolist;

    /** 
     * combobox provide an option to select indevice/internet
     */
    HbComboBox *comboBox;

    /** 
     * to execute the ui preparation at once
     */
    bool mInstialize;

    /** 
     * validating the select all
     */
    bool mchangestate;

    /** 
     * handler to get the internet service provider details
     */
    OnlineHandler *mInternetHandler;

    /** 
     * internet service providers and its ID's
     */
    QMap<int, QString> mServiceProviders;

    /** 
     * common ui controller, to get the contentinfodb details
     */
    SearchUiLoader* mUiLoader;

    SEARCH_FRIEND_CLASS (SearchStateProviderTest)
    };
#endif
