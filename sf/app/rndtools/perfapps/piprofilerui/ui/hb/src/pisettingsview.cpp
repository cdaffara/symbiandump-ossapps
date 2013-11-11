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

#include <HbMainWindow>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <QGraphicsLinearLayout>
#include <QValidator>
#include <QIntValidator>
#include <QModelIndex>
#include <HbLineEdit>
#include <HbValidator>
#include <HbEditorInterface>
#include <hbabstractitemview.h>

#include "pisettingsview.h"
#include "pimainview.h"
#include "notifications.h"
#include "piprofilerengine.h"
#include "pluginattributes.h"

const QStringList OUTPUTMODES = (QStringList() << "Debug output" << "File system");

PISettingsView::PISettingsView(PIProfilerEngine *engine) :
    mEngine(engine), mGeneralAttributes(), mPluginList(0), mSettingForm(0), mModel(0),
        mGeneralSettingsGroup(0), mPluginSettingsGroup(0), mOutputItem(0), mFileLocation(0),
        mPrefix(0), mSamplingPeriod(0)

{
    this->setTitle("Settings");

    //create setting form
    mSettingForm = new HbDataForm(this);

    //create a model class
    mModel = new HbDataFormModel(this);

    this->createGeneralSettingForms();

    // Set created model model to form
    mSettingForm->setModel(mModel);

    // Set widget into view
    setWidget(mSettingForm);

    // load general settings into data form
    loadGeneralSettings();

    // Let us know when list is shown
    connect(mSettingForm, SIGNAL(itemShown(const QModelIndex)), this,
        SLOT(addTextValidators(const QModelIndex)));

    // Set general group as expanded. 
    /*HbDataFormViewItem *viewItem = mSettingForm->dataFormViewItem(mModel->indexFromItem( 
        mGeneralSettingsGroup));
    viewItem->setExpanded(true);*/

}

// ---------------------------------------------------------------------------

PISettingsView::~PISettingsView()
{
    if (mSettingForm != 0) {
        delete mSettingForm;
        mSettingForm = 0;
    }

    if (mModel != 0) {
        delete mModel;
        mModel = 0;
    }
}

// ---------------------------------------------------------------------------

void PISettingsView::addTextValidators(const QModelIndex index)
{
    HbDataFormModelItem *modelItem = mModel->itemFromIndex(index);

    // Check if item is integer text box
    if (modelItem->contentWidgetData(QString("TextboxType")) == QString("integer")) {

        QString value = modelItem->contentWidgetData(QString("text")).toString();
        // Create validator that accepts only integer values.
        HbValidator *validator = new HbValidator(this);
        validator->addField(new QIntValidator(0, 10000, validator), "1");

        // Line editor widget from form
        HbDataFormViewItem *viewItem = qobject_cast<HbDataFormViewItem *> (mSettingForm->itemByIndex(index));
        HbWidget *widget = viewItem->dataItemContentWidget();
        HbLineEdit *editor = static_cast<HbLineEdit *> (widget);

        if (editor != 0) {
            // Create interface and attach lineEdit to it.
            HbEditorInterface editInterface(editor);
            // set editor as phone number editor so that inserting numbers is easier
            editor->setInputMethodHints(Qt::ImhDigitsOnly);
            editor->setValidator(validator);
        }
        modelItem->setContentWidgetData(QString("text"), value);

    }

}

// ---------------------------------------------------------------------------

void PISettingsView::createGeneralSettingForms()
{

    // Add data into setting model

    // General settings group
    mGeneralSettingsGroup = mModel->appendDataFormGroup(QString("General settings"),
        mModel->invisibleRootItem());

    // Output drop down list
    mOutputItem = mModel->appendDataFormItem(HbDataFormModelItem::ComboBoxItem,
        QString("Output to"), mGeneralSettingsGroup);
    mOutputItem->setContentWidgetData(QString("items"), OUTPUTMODES);

    // Data file location textbox
    mFileLocation = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, QString(
        "Save file in"), mGeneralSettingsGroup);
    mFileLocation->setContentWidgetData("maxLength", 64);

    // File name textbox
    mPrefix = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, QString("File prefix"),
        mGeneralSettingsGroup);
    mPrefix->setContentWidgetData("maxLength", 64);

    // Period that used when using timed progfiling
    mSamplingPeriod = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, QString(
        "Sampling period(s)"), mGeneralSettingsGroup);
    mSamplingPeriod->setContentWidgetData(QString("TextboxType"), QString("integer"));
    mSamplingPeriod->setContentWidgetData("maxLength", 64);

}

// ---------------------------------------------------------------------------

void PISettingsView::createPluginSpecificSettings()
{

    // Create plug-in specific group
    mPluginSettingsGroup = mModel->appendDataFormGroup(QString("Plug-in specific settings"),
        mModel->invisibleRootItem());

    if (mPluginList != 0) {

        for (int index = 0; index < mPluginList->size(); index++) {

            // create plug-in specific settings group for each plug-in which has own settings
            if (mPluginList->at(index).mItemCount > 0 || mPluginList->at(index).mSampleRate != -1) {

                // Create settings group
                HbDataFormModelItem *mPluginGroup = mModel->appendDataFormGroup(QString(
                    mPluginList->at(index).mName), mPluginSettingsGroup);

          
                // get plugin attributes
                PluginAttributes attributes = mPluginList->at(index);

                // Create sample rate item if it is needed
                if (attributes.mSampleRate != -1) {
                    attributes.mFormItemSampleRate = mModel->appendDataFormItem(
                        HbDataFormModelItem::TextItem, "Sample rate (ms)", mPluginGroup);
                    attributes.mFormItemSampleRate->setContentWidgetData(QString("TextboxType"), QString(
                        "integer"));

                    attributes.mFormItemSampleRate->setContentWidgetData("text",
                        attributes.mSampleRate);
                }

                // create other setting items
                attributes.mFormItemSettingItem1 = createPluginSettingItem(
                    &attributes.mSettingItem1, mPluginGroup);
                attributes.mFormItemSettingItem2 = createPluginSettingItem(
                    &attributes.mSettingItem2, mPluginGroup);
                attributes.mFormItemSettingItem3 = createPluginSettingItem(
                    &attributes.mSettingItem3, mPluginGroup);
                attributes.mFormItemSettingItem4 = createPluginSettingItem(
                    &attributes.mSettingItem4, mPluginGroup);
                attributes.mFormItemSettingItem5 = createPluginSettingItem(
                    &attributes.mSettingItem5, mPluginGroup);
                attributes.mFormItemSettingItem6 = createPluginSettingItem(
                    &attributes.mSettingItem6, mPluginGroup);
                mPluginList->replace(index, attributes);
            }
        }
    }

}

// ---------------------------------------------------------------------------

HbDataFormModelItem *PISettingsView::createPluginSettingItem(const SettingItem *item,
    HbDataFormModelItem *parentGroup)
{
    // create individual plug-in setting item

    HbDataFormModelItem *settingItem = 0;
    if (item->mSettingText != QString("")) {
        switch (item->mType) {
        case SettingItem::SettingItemTypeBool:
            settingItem = mModel->appendDataFormItem(HbDataFormModelItem::CheckBoxItem,
                item->mUIText, parentGroup);
            bool value = false;
            if (item->mValue == "true") {
                value = true;
            }
            settingItem->setContentWidgetData("tristate", value);

            break;
        case SettingItem::SettingItemTypeHex:
            settingItem = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, item->mUIText,
                parentGroup);
            settingItem->setContentWidgetData("text", item->mValue);
            settingItem->setContentWidgetData("maxLength", 64);

            break;

        case SettingItem::SettingItemTypeInt:
            settingItem = mModel->appendDataFormItem(HbDataFormModelItem::TextItem, item->mUIText,
                parentGroup);
            settingItem->setContentWidgetData("text", item->mValue);
            settingItem->setContentWidgetData(QString("TextboxType"), QString("integer"));
            settingItem->setContentWidgetData("maxLength", 64);

            break;

        case SettingItem::SettingItemTypeText:
            HbDataFormModelItem *settingItem = mModel->appendDataFormItem(
                HbDataFormModelItem::TextItem, item->mUIText, parentGroup);
            settingItem->setContentWidgetData("text", item->mValue);
            settingItem->setContentWidgetData("maxLength", 64);

            break;

        }
    }
    return settingItem;

}

// ---------------------------------------------------------------------------

void PISettingsView::loadGeneralSettings()
{
    // load general settings from engine
    mEngine->getGeneralSettings(mGeneralAttributes);

    // set output item correct
    if (mGeneralAttributes.mTraceOutput == "file_system") {
        mOutputItem->setContentWidgetData(QString("currentIndex"), OUTPUT_MODE_FILE);
    }
    else {
        mOutputItem->setContentWidgetData(QString("currentIndex"), OUTPUT_MODE_DEBUG);
    }

    mFileLocation->setContentWidgetData(QString("text"), mGeneralAttributes.mSaveFileDrive);
    mPrefix->setContentWidgetData(QString("text"), mGeneralAttributes.mTraceFilePrefix);
    mSamplingPeriod->setContentWidgetData(QString("text"), mGeneralAttributes.mTimedSamplingPeriod);

}

// ---------------------------------------------------------------------------

GeneralAttributes &PISettingsView::getGeneralAttributes()
{
    // Read items from ui elements and save them into mGeneralAttributes


    QString fileLocation = mFileLocation->contentWidgetData(QString("text")).toString();
    if (fileLocation.length() > 0 && mEngine->checkTraceLocationSanity(fileLocation)) {
        mGeneralAttributes.mSaveFileDrive = fileLocation;
    }
    else {
        Notifications::showErrorNote("Invalid path, check settings!");
        mFileLocation->setContentWidgetData(QString("text"), mGeneralAttributes.mSaveFileDrive);
    }
    bool ok = false;
    mGeneralAttributes.mTimedSamplingPeriod
        = mSamplingPeriod->contentWidgetData(QString("text")).toInt(&ok);

    if (!ok) {
        // this should not happen because intvalidator is set into text box
        qWarning("Unable to convert sampling period to integer");

    }
    mGeneralAttributes.mTraceFilePrefix = mPrefix->contentWidgetData(QString("text")).toString();

    if (mOutputItem->contentWidgetData(QString("currentIndex")).toInt(&ok) == OUTPUT_MODE_FILE) {
        mGeneralAttributes.mTraceOutput = QString("file_system");
    }
    else {
        mGeneralAttributes.mTraceOutput = QString("debug_output");
    }

    return mGeneralAttributes;

}

// ---------------------------------------------------------------------------

void PISettingsView::setPluginSettings(QList<PluginAttributes> *pluginList)
{
    mPluginList = pluginList;
    createPluginSpecificSettings();
}

// ---------------------------------------------------------------------------


void PISettingsView::readPluginSettingItemValue(HbDataFormModelItem *dataFormItem,
    SettingItem *settingItem)
{
    if (settingItem != 0 && dataFormItem != 0) {
        // check if item type is bool, convert bool to string
        if (settingItem->mType == SettingItem::SettingItemTypeBool) {
            bool checkboxValue = true;
            checkboxValue = dataFormItem->contentWidgetData(QString("tristate")).toBool();
            
            if(checkboxValue){
                settingItem->mValue = QString("true");
            }
            else{
                settingItem->mValue = QString("false");
            }

        }
        // otherwise save text straight into settin item
        else {
            settingItem->mValue = dataFormItem->contentWidgetData("text").toString();
        }
    }
}

// ---------------------------------------------------------------------------

void PISettingsView::readPluginSettings()
{
    // Read Plug-in setting item and save values into m
    for (int index = 0; index < mPluginList->size(); index++) {

        PluginAttributes attributes = mPluginList->at(index);

        readPluginSettingItemValue(attributes.mFormItemSettingItem1, &attributes.mSettingItem1);
        readPluginSettingItemValue(attributes.mFormItemSettingItem2, &attributes.mSettingItem2);
        readPluginSettingItemValue(attributes.mFormItemSettingItem3, &attributes.mSettingItem3);
        readPluginSettingItemValue(attributes.mFormItemSettingItem4, &attributes.mSettingItem4);
        readPluginSettingItemValue(attributes.mFormItemSettingItem5, &attributes.mSettingItem5);
        readPluginSettingItemValue(attributes.mFormItemSettingItem6, &attributes.mSettingItem6);

        if (attributes.mFormItemSampleRate != 0) {
            bool ok = false;
            attributes.mSampleRate
                = attributes.mFormItemSampleRate->contentWidgetData("text").toInt(&ok);
            if (!ok) {
                // this should not happen because intvalidator is set into text box
                qWarning("Unable to convert sample rate to integer");
            }
        }

        mPluginList->replace(index, attributes);

    }

}

// ---------------------------------------------------------------------------

void PISettingsView::expandPluginGroup(int uid)
{

    // FIXME some of the methods is deprecating fix once new methods available.
    //collapse all group items
    /*HbDataFormViewItem *viewItem = 0;
    viewItem = mSettingForm->dataFormViewItem(mModel->indexFromItem(mPluginSettingsGroup));
    if (viewItem != 0) {
        viewItem->setExpanded(false);
    }
    viewItem = mSettingForm->dataFormViewItem(mModel->indexFromItem(mGeneralSettingsGroup));
    if (viewItem != 0) {
        viewItem->setExpanded(false);
    }
    // Set plugin group as expanded.
    viewItem = mSettingForm->dataFormViewItem(mModel->indexFromItem(mPluginSettingsGroup));
    if (viewItem != 0) {
        viewItem->setExpanded(true);
    }
    // find group item which has same uid number
    for (int index = 0; index < mPluginSettingsGroup->childCount(); index++) {
        HbDataFormModelItem *item = mPluginSettingsGroup->childAt(index);
        bool ok = false;
        int groupUid = item->data(HbDataFormModelItem::KeyRole).toInt(&ok);
        if (ok) {
            if (uid == groupUid) {
                // Set group as expanded
                viewItem = mSettingForm->dataFormViewItem(mModel->indexFromItem(item));
                if (viewItem != 0) {
                    viewItem->setExpanded(true);
                }

                // scroll view so that group is shown
                mSettingForm->scrollTo(mModel->indexFromItem(item),
                    HbAbstractItemView::PositionAtTop);
                break;

            }
        }

    }*/

}

// ---------------------------------------------------------------------------
