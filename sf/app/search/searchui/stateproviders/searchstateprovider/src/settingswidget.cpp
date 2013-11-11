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
 * Description:  Implementation of the settings widget
 *
 */
#include "settingswidget.h"
#include "onlinehandler.h"
#include "searchuiloader.h"
#include <hbdialog.h>
#include <hbcheckbox.h>
#include <qgraphicslinearlayout.h>
#include <hbaction.h>
#include <hblabel.h>
#include <qsignalmapper.h>
#include <qsettings.h>
#include <hbinputdialog.h>
#include <hbscrollarea.h>
#include <qgraphicswidget.h>
#include <hbglobal.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdocumentloader.h>
#include <hbcombobox.h>
#include <hbdataformviewitem.h>
#include <hbradiobuttonlist.h>
#include <qdir.h>

const char *DELIMETER_DOCML = ":/xml/delimeterscreen.docml";
const char *DIALOG = "dialog";
const char *DATAFORM = "dataForm";

//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::SettingsWidget()
//
//----------------------------------------------------------------------------------------------------------------------------
SettingsWidget::SettingsWidget() :
    signalMapper(NULL), popup(NULL), mDocumentLoader(NULL), dataform(NULL),
            mModel(NULL), mSelectedScope(0), mSelectedProvider(0), comboBox(
                    NULL), mInternetHandler(NULL),mUiLoader(NULL)
    {
    mUiLoader = SearchUiLoader::instance();
  
    connect(mUiLoader, SIGNAL(dbChanged), this, SLOT(slotdbChanged()));

    mchangestate = true;
    mInstialize = true;
    isInternetSelected = false;
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::intialize()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::initialize()
    {
    bool ok = false;
    if (!mDocumentLoader)
        {
        mDocumentLoader = new HbDocumentLoader();
        mDocumentLoader->load(DELIMETER_DOCML, &ok);
        }
    QGraphicsWidget *widget = NULL;
    if (!popup)
        {
        widget = mDocumentLoader->findWidget(DIALOG);
        Q_ASSERT_X(ok && (widget != 0), "DIALOG", "invalid view");
        popup = qobject_cast<HbDialog*> (widget);
        }
    if (!dataform)
        {
        widget = mDocumentLoader->findWidget(DATAFORM);
        Q_ASSERT_X(ok && (widget != 0), "DATAFORM", "invalid view");
        dataform = qobject_cast<HbDataForm*> (widget);
        }
    if (!mModel)
        {
        mModel = new HbDataFormModel();
        }
    if (!signalMapper)
        {
        signalMapper = new QSignalMapper(this);
        }
    createGui();
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::createGui()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::createGui()
    {
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);

    primaryAction = new HbAction(hbTrId("txt_common_button_ok"), popup);
  
    connect(primaryAction, SIGNAL(triggered()), this, SLOT(slotOkEvent()));
  
    secondaryAction = new HbAction(hbTrId("txt_common_button_cancel"), popup);
    connect(secondaryAction, SIGNAL(triggered()), this, SLOT(slotCancelEvent()));
     
    popup->addAction(primaryAction);
    popup->addAction(secondaryAction);

    dataform->setModel(mModel);

    HbLabel *label = new HbLabel(hbTrId("txt_search_title_search_scope"));
    popup->setHeadingWidget(label);

    preparecategories();

    loadBaseSettings();

    HbDataFormModelItem* themeComboGeneral = mModel->appendDataFormItem(HbDataFormModelItem::ComboBoxItem, hbTrId("txt_search_info_select_search_scope"));

    themeComboGeneral->setContentWidgetData("items", mCategoryList);

    themeComboGeneral->setContentWidgetData("currentIndex", mSelectedScope);

    QModelIndex index = mModel->indexFromItem(themeComboGeneral);

    HbDataFormViewItem *formItem = static_cast<HbDataFormViewItem *> (dataform->itemByIndex(index));

    comboBox = static_cast<HbComboBox*> (formItem->dataItemContentWidget());

    slotcurrentIndexChanged(mSelectedScope);

    connect(comboBox, SIGNAL(currentIndexChanged(int)), this,SLOT(slotcurrentIndexChanged(int)));
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::slotcurrentIndexChanged()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotcurrentIndexChanged(int avalue)
    {
    for (int i = 0; i < mModelItemList.count(); i++)
        {
        mModel->removeItem(mModelItemList.at(i));
        }
    mModelItemList.clear();    
    if (!avalue) //device category creation
        {
        isInternetSelected = false;
        
        HbCheckBox *checkboxitem = NULL;
        
        qDeleteAll(mDeviceCheckBoxList.begin(), mDeviceCheckBoxList.end());
        
        mDeviceCheckBoxList.clear();
        
        for (int i = 0; i < mDeviceListDisplay.count(); i++)
            {
            HbDataFormModelItem* mModelItem = mModel->appendDataFormItem( HbDataFormModelItem::CheckBoxItem);
            
            mModelItemList.append(mModelItem);
            
            QModelIndex index = mModel->indexFromItem(mModelItem);

            HbDataFormViewItem *formItem = static_cast<HbDataFormViewItem *> (dataform->itemByIndex(index));

            checkboxitem = static_cast<HbCheckBox*> (formItem->dataItemContentWidget());

            checkboxitem->setText(mDeviceListDisplay.at(i));

            mDeviceCheckBoxList.append(checkboxitem);

            signalMapper->setMapping(mDeviceCheckBoxList.at(i), i);

            connect(mDeviceCheckBoxList.at(i), SIGNAL(stateChanged(int)),signalMapper, SLOT(map()));

            connect(signalMapper, SIGNAL(mapped(int)), this,SLOT(slotstateChanged(int)));
            }
        loadDeviceSettings();
        }
    else
        {
        isInternetSelected = true;
        
        qDeleteAll(mDeviceCheckBoxList.begin(), mDeviceCheckBoxList.end());
        
        mDeviceCheckBoxList.clear();
        
        HbDataFormModelItem* mModelItem = mModel->appendDataFormItem(HbDataFormModelItem::RadioButtonListItem);
        
        mModelItemList.append(mModelItem);
        
        QStringList internetCategoryList;
        
        QMapIterator<int, QString> i(mServiceProviders);
        
        int selectedindex = 0,Iterator = 0;
        
        while (i.hasNext())
            {
            i.next();
            if (i.key() == mSelectedProvider)
                selectedindex = Iterator;
            Iterator++;
            internetCategoryList.append(i.value());
            }
        mModelItem->setContentWidgetData("items", internetCategoryList);

        mModelItem->setContentWidgetData("previewMode",HbRadioButtonList::NoPreview);

        mModelItem->setContentWidgetData("selected", selectedindex);

        QModelIndex index = mModel->indexFromItem(mModelItem);

        HbDataFormViewItem *formItem = static_cast<HbDataFormViewItem *> (dataform->itemByIndex(index));
        
        mradiolist = static_cast<HbRadioButtonList*> (formItem->dataItemContentWidget());

        connect(mradiolist, SIGNAL(itemSelected(int)), this,SLOT(slotitemSelected(int)));
        }
    
    setActionVisibility();
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::preparecategories()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::preparecategories()
    {
    // read form database
    mDeviceListDisplay.clear();
    mDeviceStoragelist.clear();
    mCategoryList.clear();
    
    QString tempstr;
    for (int i = 0; i < mUiLoader->ContentInfoList().count(); i++)
        {
        if (tempstr != hbTrId(mUiLoader->ContentInfoList().at(i)->getDisplayName().toAscii()))
            {
            tempstr = hbTrId(mUiLoader->ContentInfoList().at(i)->getDisplayName().toAscii());

            mDeviceListDisplay.insert(mUiLoader->ContentInfoList().at(i)->getDisplayOrder(), 
                    hbTrId(mUiLoader->ContentInfoList().at(i)->getDisplayName().toAscii()));
            
            mDeviceStoragelist.insert(mUiLoader->ContentInfoList().at(i)->getDisplayOrder(),
                    mUiLoader->ContentInfoList().at(i)->getDisplayName());
            }
        }
    if (mServiceProviders.count())
        {
        mCategoryList = (QStringList() << hbTrId("txt_search_list_device")<< hbTrId("txt_search_list_internet"));
        }
    else
        {
        mCategoryList = (QStringList() << hbTrId("txt_search_list_device"));
        }
    storeDefaultSettings();
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::~SettingsWidget()
//
//----------------------------------------------------------------------------------------------------------------------------
SettingsWidget::~SettingsWidget()
    {
    SearchUiLoader::deleteinstance();
    delete signalMapper;
    delete mModel;
    mDeviceCheckBoxList.clear();
    mInternetCheckBoxList.clear();
    mModelItemList.clear();    
    delete mDocumentLoader;
    delete mInternetHandler;
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::launchSettingWidget()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::launchSettingWidget()
    {
    if (mInstialize)
        {
        initialize();
        mInstialize = false;
        }
    // refresh the screen to top     
    QModelIndex index = mModel->index(0, 0);
    dataform->scrollTo(index);
    comboBox->setCurrentIndex(mSelectedScope);
    loadDeviceSettings();
    setActionVisibility();
    popup->show();
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::setActionVisibility()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::setActionVisibility()
    {
    bool noItemSelected = true;
    for (int i = 0; i < mDeviceCheckBoxList.count(); i++)
        {
        if (mDeviceCheckBoxList.at(i)->checkState() == Qt::Checked)
            {
            noItemSelected = false;
            break;
            }
        }
    if (!isInternetSelected)
        {
        primaryAction->setVisible(true);
        if (noItemSelected)
            {
            primaryAction->setVisible(false);
            }
        }
    else
        {
        primaryAction->setVisible(true);
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::slotstateChanged( int selectedIndex)
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotstateChanged(int selectedIndex)
    {
    if (mDeviceCheckBoxList.count() != 0)
        {
        if (selectedIndex == 0 && mchangestate) // validating the select all
            {
            if ((mDeviceCheckBoxList.at(selectedIndex)->checkState() == Qt::Checked))
                {
                checkSubCategories();
                }
            else
                {
                unCheckSubCategories();
                }
            }
        else
            {
            if ((mDeviceCheckBoxList.at(selectedIndex)->checkState() == Qt::Unchecked))
                {
                mchangestate = false;
                mDeviceCheckBoxList.at(0)->setCheckState(Qt::Unchecked);
                setActionVisibility();
                return;
                }
            }
        }
    mchangestate = true;
    setActionVisibility();
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::checkSubCategories
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::checkSubCategories()
    {
    for (int i = 0; i < mDeviceCheckBoxList.count(); i++)
        {
        mDeviceCheckBoxList.at(i)->setCheckState(Qt::Checked);
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::unCheckSubCategories
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::unCheckSubCategories()
    {
    for (int i = 0; i < mDeviceCheckBoxList.count(); i++)
        {
        mDeviceCheckBoxList.at(i)->setCheckState(Qt::Unchecked);
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::checkBoxCancelEvent()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotCancelEvent()
    {    
    emit settingsEvent(false);
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::checkBoxOkEvent()
//
//----------------------------------------------------------------------------------------------------------------------------

void SettingsWidget::slotOkEvent()
    {
    storeSettingsToiniFile();
    emit settingsEvent(true);
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::storeSettingsToiniFile()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::storeSettingsToiniFile()
    {
    QSettings appSettings(SETTINGS_INI_PATH, QSettings::IniFormat);
    isInternetSelected ? (mSelectedScope = 1) : (mSelectedScope = 0);
    appSettings.setValue("selectedcategory", mSelectedScope);
    if (!isInternetSelected)
        {
        for (int i = 0; i < mDeviceCheckBoxList.count(); i++)
            {
            int value;
            (mDeviceCheckBoxList.at(i)->checkState() == Qt::Checked) ? (value = 1) : (value = 0);                
            
            appSettings.setValue(mDeviceStoragelist.at(i), value);
            
            for (int j = 0; j < mUiLoader->ContentInfoList().count(); j++)
                {
                if (mUiLoader->ContentInfoList().at(j)->getDisplayName() == mDeviceStoragelist.at(i))
                    value ? (mUiLoader->ContentInfoList().at(j)->setSelected(true)) : (mUiLoader->ContentInfoList().at(j)->setSelected(false));
                }
            }
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::loadBaseSettings()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::loadBaseSettings()
    {
    QSettings appSettings(SETTINGS_INI_PATH, QSettings::IniFormat);
    mSelectedScope = appSettings.value("selectedcategory", 0).toInt();
    mSelectedScope ? (isInternetSelected = true) : (isInternetSelected = false);
    }

//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::loadDeviceSettings()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::loadDeviceSettings()
    {
    QSettings appSettings(SETTINGS_INI_PATH, QSettings::IniFormat);
    int value;
    if (!isInternetSelected)
        {        
        for (int i = 0; i < mDeviceStoragelist.count(); i++)
            {
            value = appSettings.value(mDeviceStoragelist.at(i), 1).toInt();
            if (value && mDeviceCheckBoxList.at(i))
                {
                mDeviceCheckBoxList.at(i)->setCheckState(Qt::Checked);
                }
            else
                {
                mDeviceCheckBoxList.at(i)->setCheckState(Qt::Unchecked);
                }
            }
        QStringList allKeys = appSettings.allKeys();
        for (int k = 0; k < allKeys.count(); k++)
            {
            value = appSettings.value(allKeys.at(k), 1).toInt();
            for (int j = 0; j < mUiLoader->ContentInfoList().count(); j++)
                {
                if (mUiLoader->ContentInfoList().at(j)->getDisplayName() == allKeys.at(k))
                    {
                    value ? (mUiLoader->ContentInfoList().at(j)->setSelected(true)) : (mUiLoader->ContentInfoList().at(j)->setSelected(false));                
                    }
                }
            }
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//SettingsWidget::isInternetSearchOptionSelected()
//
//----------------------------------------------------------------------------------------------------------------------------
bool SettingsWidget::isInternetSearchOptionSelected()
    {
    loadBaseSettings();
    return isInternetSelected;
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::storeDefaultSettings()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::storeDefaultSettings()
    {
    QSettings appSettings(SETTINGS_INI_PATH, QSettings::IniFormat);
    if (!appSettings.contains("selectedcategory"))
        {
        mSelectedScope = 0;
        appSettings.setValue("selectedcategory", mSelectedScope);
        }
    for (int i = 0; i < mDeviceStoragelist.count(); i++)
        {
        if (!appSettings.contains(mDeviceStoragelist.at(i)))
            appSettings.setValue(mDeviceStoragelist.at(i), 1);
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::slotproviderDetails()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotproviderDetails(QString name, HbIcon icon, int id)
    {
    mServiceProviders.insert(id, name);
    emit ISProvidersIcon(icon, id);
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::slotdefaultProvider()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotdefaultProvider(const int value)
    {
    mSelectedProvider = value;
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::slotitemSelected()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotitemSelected(int value)
    {
    QMapIterator<int, QString> i(mServiceProviders);
    int selectedindex = 0;
    while (i.hasNext())
        {
        i.next();
        if (selectedindex == value)
            {
            mInternetHandler->setDefaultProvider(i.key());
            break;
            }
        selectedindex++;
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::loadIS()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::loadIS()
    {
    if (!mInternetHandler)
        {
        mInternetHandler = new OnlineHandler();
        
        connect(mInternetHandler,SIGNAL(providerDetails(QString, HbIcon, int)), this,SLOT(slotproviderDetails(QString, HbIcon, int)));

        connect(mInternetHandler, SIGNAL(defaultProvider(const int)), this,SLOT(slotdefaultProvider(const int)));
        
        mInternetHandler->readSettings();
        }
    }
//----------------------------------------------------------------------------------------------------------------------------
//void SettingsWidget::slotdbChanged()
//
//----------------------------------------------------------------------------------------------------------------------------
void SettingsWidget::slotdbChanged()
    {
    preparecategories();
    
    slotcurrentIndexChanged(mSelectedScope);
    }
