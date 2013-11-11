/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: Language plugin view class
 *
 */

#include "cplanguageview.h"
#include <QDebug>
#include <QStringList>
#include <QLocale>
#include <QDir>
#include <QPluginLoader>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbmessagebox.h>
#include <cpsettingformentryitemdata.h>
#include <ClockSettingsViewInterface>
#include <hbinpututils.h>
#include <hbinputsettingproxy>
#include <hblocaleutil.h>
#include <starterclient.h>
#include <hbdevicemessagebox>
#include <hbaction.h>
#include "cplanguageplugin.h"
#include "cplanguagepluginlog.h"

static int PRIMARY_INPUT_ITEM_INDEX   = 0;
static int SECONDARY_INPUT_ITEM_INDEX = 1;
static int REGION_ITEM_INDEX          = 2;

/*
 * Constructor
 */
CpLanguageView::CpLanguageView(QGraphicsItem *parent) :
    CpBaseSettingView(0,parent),
    mModel(0),
    mCurrentLanguagePage(0), 
    mCurrentRegionItem(0),
    mCurrentPrimaryInputLanguageItem(0),
    mCurrentSecondaryInputLanguageItem(0),
    mClockPluginLoader(0)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::CpLanguageView")
    
    mChineseVariant = CpLanguagePluginUtil::isChineseVariant();
    if (mChineseVariant) {
        PRIMARY_INPUT_ITEM_INDEX = 0;
        SECONDARY_INPUT_ITEM_INDEX = -1; // no secondary writing language in Chinese variant
        REGION_ITEM_INDEX = 1;
    }
    
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    if (form) {        
        mModel = new HbDataFormModel;
        
        form->setHeading(hbTrId("txt_cp_subhead_display_language"));
        form->setDescription(hbTrId("txt_cp_info_after_changign_deivce_language_device"));  
        
        mLanguageRegionMapping = ExtendedLocaleUtil::localeMappings(true);  
         
        mSupportedLanguages = HbLocaleUtil::supportedLanguages();
        QStringList localizedLanguages = CpLanguagePluginUtil::localizedLanguageNames(mSupportedLanguages);
               
        mSupportedRegions = HbLocaleUtil::supportedRegions();
        QStringList localizedRegions = CpLanguagePluginUtil::localizedRegionNames(mSupportedRegions);
        
        CPLANG_LOG ( QString("map count:%1").arg(mLanguageRegionMapping.count()) );
        CPLANG_LOG ( QString("support language count:%1").arg(mSupportedLanguages.count()) );
        CPLANG_LOG ( QString("support region count:%1").arg(mSupportedRegions.count()) );
           
        HbInputUtils::listSupportedInputLanguages(mPrimaryInputLanguages);
        QStringList localizedInputLanguages = CpLanguagePluginUtil::localizedInputLanguageNames(mPrimaryInputLanguages);
        
        QString currentLanguage = HbLocaleUtil::currentLanguage();
        
        //prvious setting
        mPreviousSetting.languageId = currentLanguage;
        mPreviousSetting.regionId = HbLocaleUtil::currentRegion();
        mPreviousSetting.primaryWritingLan = HbInputSettingProxy::instance()->globalInputLanguage();
        mPreviousSetting.secondaryWritingLan = HbInputSettingProxy::instance()->globalSecondaryInputLanguage();
        
        mCurrentSetting = mPreviousSetting;
        
        //update with current setting
        if (LanguageRegionMapping *setting = languageRegionMapping(currentLanguage)) {
            *setting = mCurrentSetting;
        }
              
        for (int index = 0; index < mLanguageRegionMapping.count();index++) {
            const LanguageRegionMapping &setting = mLanguageRegionMapping.at(index);
            HbDataFormModelItem *languageAndRegionPage = mModel->appendDataFormPage( HbLocaleUtil::localisedLanguageName(setting.languageId) );
            mLanguagePages.append(languageAndRegionPage);
                         
            /*
             * Create primary writing language item.
             */
            HbDataFormModelItem *primaryInputLanguageItem = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem, 
                hbTrId("txt_cp_setlabel_primary_writing_language"));
            
                
            int primaryInputLanguageIndex = mPrimaryInputLanguages.indexOf(setting.primaryWritingLan);
            
            CPLANG_LOG(QLatin1String("Primary input Language:") + setting.primaryWritingLan.asString());
            CPLANG_LOG(QString("Index of primary input language: %1").arg(primaryInputLanguageIndex));
            
            fillDataFormComboBoxItem(primaryInputLanguageItem,localizedInputLanguages,primaryInputLanguageIndex);
            
            form->addConnection(primaryInputLanguageItem, SIGNAL(currentIndexChanged (int)),
                this,SLOT(onPrimaryInputLanguageChanged(int)));
            languageAndRegionPage->appendChild(primaryInputLanguageItem);
            
            /*
             * Create secondary writing language item.
             * (No secondary writing language for Chinese variant)
             */
            if (!mChineseVariant) {        
                HbDataFormModelItem *secondaryInputLanguageItem = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
                    hbTrId("txt_cp_setlabel_secondary_writing_language"));
                             
                form->addConnection(secondaryInputLanguageItem, SIGNAL(currentIndexChanged (int)),
                    this,SLOT(onSecondaryInputLanguageChanged(int)));
                languageAndRegionPage->appendChild(secondaryInputLanguageItem);
            }
            
            /*
             * Create region item.
             */
            HbDataFormModelItem *regionItem = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,
                hbTrId("Region"));
 
            int regionIndex = mSupportedRegions.indexOf(setting.regionId);
            
            CPLANG_LOG(QLatin1String("Current region:") + setting.regionId);
            CPLANG_LOG(QString("Index of current region: %1").arg(regionIndex));
            
            fillDataFormComboBoxItem(regionItem,localizedRegions,regionIndex); 
            
            form->addConnection(regionItem, SIGNAL(currentIndexChanged (int)),
                this,SLOT(onRegionChanged(int)));
            languageAndRegionPage->appendChild(regionItem);   
            
            if (mSupportedLanguages.at(index) == currentLanguage) {
                mCurrentLanguagePage = languageAndRegionPage;
                mCurrentRegionItem = mCurrentLanguagePage->childAt(REGION_ITEM_INDEX);
                mCurrentPrimaryInputLanguageItem = mCurrentLanguagePage->childAt(PRIMARY_INPUT_ITEM_INDEX);
                mCurrentSecondaryInputLanguageItem = mCurrentLanguagePage->childAt(SECONDARY_INPUT_ITEM_INDEX);
            }
        }
        
        // update secondary input language item according to primary input language
        updateSecondaryInputLanguageItem();
        
        /*
         * Create region settings item.
         */ 
        HbDataFormModelItem *regionalSettingsItem = new HbDataFormModelItem(
            static_cast<HbDataFormModelItem::DataItemType>(CpSettingFormEntryItemData::ButtonEntryItem));
        regionalSettingsItem->setContentWidgetData("text",hbTrId("txt_cp_button_regional_settings"));
        regionalSettingsItem->setContentWidgetData("textAlignment", 
            QVariant(Qt::AlignHCenter | Qt::AlignVCenter));
        form->addConnection(regionalSettingsItem,SIGNAL(clicked()),
            this,SLOT(launchRegionalSettingsView()));
        
        mModel->appendDataFormItem(regionalSettingsItem);
        
        form->setModel(mModel);
        // active current language page
        form->setExpanded(mModel->indexFromItem(mCurrentLanguagePage),true);
        
        
        /*
         * observe input language changed event
         */
        connect(HbInputSettingProxy::instance(),
            SIGNAL(globalInputLanguageChanged(HbInputLanguage)),
            this,
            SLOT(onPrimaryInputLanguageChanged(HbInputLanguage)));
        
        connect(HbInputSettingProxy::instance(),
            SIGNAL(globalSecondaryInputLanguageChanged(HbInputLanguage)),
            this,
            SLOT(onSecondaryInputLanguageChanged(HbInputLanguage)));    
        
        connect(form,
            SIGNAL(activated(QModelIndex)),
            this,
            SLOT(onDataFormItemActivated(QModelIndex)));
        
    }
}

/*
 * Destructor
 */
CpLanguageView::~CpLanguageView()
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::~CpLanguageView")
	// Unload RegionalSettingsView plugin
    if (mClockPluginLoader) {
        mClockPluginLoader->unload();
        delete mClockPluginLoader;
    }
    delete mModel;
}

/*!
 * Handle region changed
 */
void CpLanguageView::onRegionChanged(int index)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::onRegionChanged")
    
    CPLANG_LOG(QLatin1String("Before change region:") + HbLocaleUtil::currentRegion());
    
    if (index >= 0 && index < mSupportedRegions.count()) {
		HbLocaleUtil::changeRegion( mSupportedRegions.at(index) );	
		mCurrentSetting.regionId = mSupportedRegions.at(index);
        if (LanguageRegionMapping *setting = languageRegionMapping(mCurrentSetting.languageId)) {
            setting->regionId = mCurrentSetting.regionId;
        }
		
		CPLANG_LOG(QLatin1String("After change region:") + HbLocaleUtil::currentRegion());
    }
}


/*
 * prompt user to restart device.
 */
bool CpLanguageView::promptRestart()
{
    HbDeviceMessageBox messageBox( 
        hbTrId("txt_cp_text_edit_device_will_be_restarted_to_chang"), 
        HbMessageBox::MessageTypeQuestion, this);

    QString okText = hbTrId("txt_cp_button_restart");
    QString cancelText = hbTrId("txt_cp_button_discard_changes");
    messageBox.setAction(new QAction(okText,&messageBox), 
        HbDeviceMessageBox::AcceptButtonRole);
    messageBox.setAction(new QAction(cancelText,&messageBox), 
        HbDeviceMessageBox::RejectButtonRole);
    messageBox.setIconVisible(false);
    
    return (messageBox.exec() 
            == messageBox.action(HbDeviceMessageBox::AcceptButtonRole) );
}

/*!
 * Handle primary language changed.
 */
void CpLanguageView::onPrimaryInputLanguageChanged(int index)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::onPrimaryInputLanguageChanged(int index)")
    
    CPLANG_LOG(QLatin1String("Before change primary input language:") + HbInputSettingProxy::instance()->globalInputLanguage().asString());
    
    if (index >= 0 && index < mPrimaryInputLanguages.count()) {
        HbInputLanguage inputLanguage = mPrimaryInputLanguages.at(index);
        if (inputLanguage != HbInputSettingProxy::instance()->globalInputLanguage()) {
            HbInputSettingProxy::instance()->setGlobalInputLanguage(inputLanguage);
            mCurrentSetting.primaryWritingLan = inputLanguage;
            if (LanguageRegionMapping *setting = languageRegionMapping(mCurrentSetting.languageId)) {
                setting->primaryWritingLan = mCurrentSetting.primaryWritingLan;
            }
            
            updateSecondaryInputLanguageItem();
            CPLANG_LOG(QLatin1String("After change primary input language:") + HbInputSettingProxy::instance()->globalInputLanguage().asString());
        }
    }
}

/*!
 * Handle secondary language changed.
 */
void CpLanguageView::onSecondaryInputLanguageChanged(int index)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::onSecondaryInputLanguageChanged(int index)")
    
    CPLANG_LOG(QLatin1String("Before change secondary input language:") + HbInputSettingProxy::instance()->globalSecondaryInputLanguage().asString());
    
    if (index >= 0 && index < mSecondaryInputLanguages.count()) {
        HbInputLanguage inputLanguage = mSecondaryInputLanguages.at(index);
        if (inputLanguage != HbInputSettingProxy::instance()->globalSecondaryInputLanguage() ) {
            HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(inputLanguage);
            mCurrentSetting.secondaryWritingLan = inputLanguage;
            if (LanguageRegionMapping *setting = languageRegionMapping(mCurrentSetting.languageId)) {
                setting->secondaryWritingLan = mCurrentSetting.secondaryWritingLan;
            }
            
            CPLANG_LOG(QLatin1String("After change secondary input language:") + HbInputSettingProxy::instance()->globalSecondaryInputLanguage().asString());
        }
    }
}

/*!
 * Observe primary input language changed event and update the ui, if it is being changed from outside of this plugin
 */
void CpLanguageView::onPrimaryInputLanguageChanged(const HbInputLanguage &inputLanguage)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::onPrimaryInputLanguageChanged(const HbInputLanguage &inputLanguage)")
    if (mCurrentPrimaryInputLanguageItem) {
        int index = mPrimaryInputLanguages.indexOf(inputLanguage);
        if (index != mCurrentPrimaryInputLanguageItem->contentWidgetData("currentIndex").toInt() ) {
            CPLANG_LOG( QString("New Index: %1").arg(index) )
                
            mCurrentSetting.primaryWritingLan = inputLanguage;
            mCurrentPrimaryInputLanguageItem->setContentWidgetData("currentIndex",index);
            updateSecondaryInputLanguageItem();
        }
    }
}

/*!
 * Observe secondary input language changed event and update the ui, if it is being changed from outside of this plugin
 */
void CpLanguageView::onSecondaryInputLanguageChanged(const HbInputLanguage &inputLanguage)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::onSecondaryInputLanguageChanged(const HbInputLanguage &inputLanguage)")
        
    if (mCurrentSecondaryInputLanguageItem) {
        int index = mSecondaryInputLanguages.indexOf(inputLanguage);
        if (index != mCurrentSecondaryInputLanguageItem->contentWidgetData("currentIndex").toInt()) {
            CPLANG_LOG( QString("New Index: %1").arg(index) )
            mCurrentSetting.secondaryWritingLan = inputLanguage;
            mCurrentSecondaryInputLanguageItem->setContentWidgetData("currentIndex",index);
        }
    }
}


/*!
 * Fill contents in the data form model items.
 * \param comboBoxItem a data form model item whose type is HbDataFormModelItem::ComboBoxItem
 * \param items items showing in the combobox
 * \param currentIndex index of the selected item
 */
void CpLanguageView::fillDataFormComboBoxItem(HbDataFormModelItem *comboBoxItem,
                                      const QStringList &items,
                                      int currentIndex /*= -1*/)
{
    Q_ASSERT(comboBoxItem);
    Q_ASSERT(comboBoxItem->type() == HbDataFormModelItem::ComboBoxItem);
    
    comboBoxItem->setContentWidgetData("items", items);
    comboBoxItem->setContentWidgetData("currentIndex",currentIndex);
}

/*!
 *  Update secondary input language item according to primary input language
 */
void CpLanguageView::updateSecondaryInputLanguageItem()
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::updateSecondaryInputLanguageItem")
           
    // no secondary writing language in Chinese variant.
    if (mChineseVariant) {
        return;
    }
    
    HbInputLanguage primaryInputLanguage = HbInputSettingProxy::instance()->globalInputLanguage();
 
    mSecondaryInputLanguages.clear();
    // append a "none set" item
    mSecondaryInputLanguages.append(HbInputLanguage());
    
    // Chinese input doesn't have secondary input language
    if (primaryInputLanguage.language() == QLocale::Chinese) {
        HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(HbInputLanguage());
        CPLANG_LOG("Primary input language: Chinese");
    }
    else {
        foreach(const HbInputLanguage &inputLanguage,mPrimaryInputLanguages) {
            if ( inputLanguage != primaryInputLanguage   // Secondary input language can't be set the same as primary input language
                && inputLanguage.language() != QLocale::Chinese) {  // Chinese input can't be set as secondary input language
                mSecondaryInputLanguages.append(inputLanguage);
            }
        }
    }
       
    QStringList localisedNames = CpLanguagePluginUtil::localizedInputLanguageNames(mSecondaryInputLanguages);
    
    // if the secondary input language is the same as primary input language, set it as "none set"
    HbInputLanguage secondaryInputLanguage = HbInputSettingProxy::instance()->globalSecondaryInputLanguage();
    
    if (secondaryInputLanguage == primaryInputLanguage) {
        secondaryInputLanguage = HbInputLanguage();
        HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(HbInputLanguage());  
        
        CPLANG_LOG("secondary == primary, set secondary input as \"None\"");
    }
    
    mCurrentSetting.secondaryWritingLan = secondaryInputLanguage;
    if (LanguageRegionMapping *setting = languageRegionMapping(mCurrentSetting.languageId)) {
        setting->secondaryWritingLan = mCurrentSetting.secondaryWritingLan;
    }
    
    fillDataFormComboBoxItem(mCurrentSecondaryInputLanguageItem,localisedNames,mSecondaryInputLanguages.indexOf(secondaryInputLanguage)); 
}

/*!
 * Restart the device.
 */
int CpLanguageView::restartDevice() 
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::restartDevice")
#ifdef Q_OS_SYMBIAN
    RStarterSession session;
    TInt error = session.Connect();

    if (error == KErrNone) {
        session.Reset( RStarterSession::ELanguageSwitchReset );
        session.Close();
    }
    
    return error;
#else
    return 0;
#endif
}

/*!
    Launch the regional settings view.
 */
void CpLanguageView::launchRegionalSettingsView()
{    
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::launchRegionalSettingsView")
    
    //Load the clock settings view plugin if it is not loaded
    if (!mClockPluginLoader) {
        QDir dir(CLOCK_SETTINGSVIEW_PLUGIN_PATH);
        QString pluginName = dir.absoluteFilePath(CLOCK_SETTINGSVIEW_PLUGIN_NAME);
        mClockPluginLoader = new QPluginLoader(pluginName);
        mClockPluginLoader->load();
    }
    
    //Launch the settings view
    ClockSettingsViewInterface *clockSettingsViewInterface 
        = qobject_cast<ClockSettingsViewInterface*>(mClockPluginLoader->instance());
    if (clockSettingsViewInterface) {
        clockSettingsViewInterface->launchRegionalSettingsView();
    }
    else {
        CPLANG_LOG("Load clocksettingsviewplugin failed.");
    }
}

/*!
 *  handle data form page changed event
 */
void CpLanguageView::onDataFormItemActivated(const QModelIndex &modelIndex)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::onDataFormItemActivated")
        
    mCurrentLanguagePage = mModel->itemFromIndex(modelIndex);
    
    mCurrentRegionItem = mCurrentLanguagePage->childAt(REGION_ITEM_INDEX);
    mCurrentPrimaryInputLanguageItem = mCurrentLanguagePage->childAt(PRIMARY_INPUT_ITEM_INDEX);
    mCurrentSecondaryInputLanguageItem = mCurrentLanguagePage->childAt(SECONDARY_INPUT_ITEM_INDEX);
       
    int index = mLanguagePages.indexOf(mCurrentLanguagePage);
    
    if (index >= 0) {
        QString newLanguage = mSupportedLanguages.at(index);
        LanguageRegionMapping *setting = languageRegionMapping(newLanguage);
        if (setting) {
            setting->secondaryWritingLan = HbInputSettingProxy::instance()->globalSecondaryInputLanguage();
            mCurrentSetting = *setting;  
            
            HbLocaleUtil::changeRegion(mCurrentSetting.regionId);
            HbInputSettingProxy::instance()->setGlobalInputLanguage(mCurrentSetting.primaryWritingLan);
            
            //update combobox
            mCurrentPrimaryInputLanguageItem->setContentWidgetData("currentIndex",mPrimaryInputLanguages.indexOf(mCurrentSetting.primaryWritingLan));
            mCurrentRegionItem->setContentWidgetData("currentIndex",mSupportedRegions.indexOf(mCurrentSetting.regionId)); 
            
            CPLANG_LOG(QString("Primary writing:") + HbInputSettingProxy::instance()->globalInputLanguage().asString());
            CPLANG_LOG(QString("Secondary writing:") + HbInputSettingProxy::instance()->globalSecondaryInputLanguage().asString());
            
            updateSecondaryInputLanguageItem();
        }
    }  
 
}

/*!
 * From CpBaseSettingView, prompt user to restart device before exiting the view
 */
void CpLanguageView::close()
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::close()")
        
    CPLANG_LOG( QString("mCurrentSetting.languageId :") + mCurrentSetting.languageId )
    CPLANG_LOG( QString("HbLocaleUtil::currentLanguage():") + HbLocaleUtil::currentLanguage() )
    
    if (mCurrentSetting.languageId != HbLocaleUtil::currentLanguage()) {      
        // apply new settings
        if (promptRestart()) {
            HbLocaleUtil::changeLocale(mCurrentSetting.languageId);
            
            /*
             * change other settings.
             */
            if (mSupportedRegions.contains(mCurrentSetting.regionId,Qt::CaseInsensitive)) {
                HbLocaleUtil::changeRegion(mCurrentSetting.regionId);
            }
            CpLanguagePlugin::updateEntryItem();
            HbInputSettingProxy::instance()->setGlobalInputLanguage(mCurrentSetting.primaryWritingLan);
            HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(mCurrentSetting.secondaryWritingLan);
            
            restartDevice();
        }
        // restore previous values
        else {
            if (mCurrentSetting.regionId != mPreviousSetting.regionId) {
                HbLocaleUtil::changeRegion(mPreviousSetting.regionId);
            }
            if (mCurrentSetting.primaryWritingLan != mPreviousSetting.primaryWritingLan) {
                HbInputSettingProxy::instance()->setGlobalInputLanguage(mPreviousSetting.primaryWritingLan);
            }
            if (mCurrentSetting.secondaryWritingLan != mPreviousSetting.secondaryWritingLan) {
                HbInputSettingProxy::instance()->setGlobalSecondaryInputLanguage(mPreviousSetting.secondaryWritingLan);
            }
        }
    }
    
    CpBaseSettingView::close();
}

/*!
 * get the language-region mapping table
 */
LanguageRegionMapping *CpLanguageView::languageRegionMapping(const QString &language)
{
    CPLANG_LOG_FUNC_ENTRY("CpLanguageView::languageRegionMapping(const QString &language)")
    
    foreach(const LanguageRegionMapping &setting,mLanguageRegionMapping) {
        if (setting.languageId.compare(language,Qt::CaseInsensitive) == 0) {
            return const_cast<LanguageRegionMapping*>(&setting);
        }
    }
    
    CPLANG_LOG("Not found.")
    
    return 0;
}

//End of File
