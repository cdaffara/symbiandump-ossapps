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

#include "settingsview.h"

#include <HbAction>
#include <HbToolBar>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbLineEdit>
#include <HbPushButton>

#include "enginewrapper.h"

const QStringList PRIORITY_ITEMS = QStringList() << SettingsView::tr("Much less") <<
    SettingsView::tr("Less") << SettingsView::tr("Normal") <<
    SettingsView::tr("More") << SettingsView::tr("Much more") <<
    SettingsView::tr("Real time") << SettingsView::tr("Abs. very low") <<
    SettingsView::tr("Abs. low") << SettingsView::tr("Abs background") <<
    SettingsView::tr("Abs. foreground") << SettingsView::tr("Abs high");

const QStringList CPU_SAMPLING = QStringList() << SettingsView::tr("CPU Time") <<
    SettingsView::tr("NOPs");

const QStringList POPUP_LOCATION = QStringList() << SettingsView::tr("Top right") <<
    SettingsView::tr("Bottom middle");

const QStringList POPUP_VISIBILITY = QStringList() << SettingsView::tr("On") <<
    SettingsView::tr("Backgr. only") << SettingsView::tr("Off");

const QStringList SOURCES = QStringList() << SettingsView::tr("CPU") <<
    SettingsView::tr("Ram") << SettingsView::tr("C:") << SettingsView::tr("D:") <<
    SettingsView::tr("E:") << SettingsView::tr("F:") << SettingsView::tr("G:") <<
    SettingsView::tr("H:") << SettingsView::tr("I:") << SettingsView::tr("Power");

const QStringList LOG_MODES = QStringList() << SettingsView::tr("RDebug") <<
    SettingsView::tr("Log file") << SettingsView::tr("RDebug & log file");


SettingsView::SettingsView(EngineWrapper &engine)
    : mEngine(engine), mLogFilePathItem(0)
{
    setTitle(tr("Settings"));

    //create toolbar showing launch popup
    HbToolBar *toolBar = this->toolBar();
    HbAction *actionOk = new HbAction(tr("Ok"), toolBar);
    HbAction *actionCancel = new HbAction(tr("Cancel"), toolBar);
    
    toolBar->addAction(actionOk);
    toolBar->addAction(actionCancel);

    //create setting form
    mSettingsForm = new HbDataForm();

    //create a model class
    mModel = new HbDataFormModel(this);
    createModel(*mModel);
    load(engine.settings());

    connect(mSettingsForm, SIGNAL(itemShown(const QModelIndex)),
            this, SLOT(dataItemDisplayed(const QModelIndex)));

    // Set created model to form
    mSettingsForm->setModel(mModel);
    setWidget(mSettingsForm);

    connect(actionOk, SIGNAL(triggered()), this, SLOT(accept()));
    connect(actionCancel, SIGNAL(triggered()), this, SLOT(reject()));
}

void SettingsView::createModel(HbDataFormModel &model)
{
    // General page
    HbDataFormModelItem *generalPage = model.appendDataFormPage(tr("General"));

    mHeartBeatItem = model.appendDataFormItem(
            HbDataFormModelItem::TextItem, tr("Heart beat (ms)"), generalPage);
    mHeartBeatItem->setContentWidgetData("maximum", 99999);
    mHeartBeatItem->setContentWidgetData("minimum", 0);
    mHeartBeatItem->setContentWidgetData("inputMethodHints", Qt::ImhDigitsOnly);

    mMaxSamplesItem = model.appendDataFormItem(
            HbDataFormModelItem::TextItem, tr("Max samples"), generalPage);
    mMaxSamplesItem->setContentWidgetData("maximum", 9999);
    mMaxSamplesItem->setContentWidgetData("minimum", 0);
    mMaxSamplesItem->setContentWidgetData("inputMethodHints", Qt::ImhDigitsOnly);

    mPriorityItem = model.appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("Priority"), generalPage);
    mPriorityItem->setContentWidgetData("items", PRIORITY_ITEMS);

    mCpuSamplingItem = model.appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("CPU sampling mode"), generalPage);
    mCpuSamplingItem->setContentWidgetData("items", CPU_SAMPLING);
    
    mKeepBacklightItem = model.appendDataFormItem(
            HbDataFormModelItem::CheckBoxItem, tr("Backlight"), generalPage);
    mKeepBacklightItem->setContentWidgetData("text", tr("Keep backlight on"));

    mPowerMonitoringEnabledItem = model.appendDataFormItem(
            HbDataFormModelItem::CheckBoxItem, tr("Power"), generalPage);
    mPowerMonitoringEnabledItem->setContentWidgetData("text", tr("Enable power monitoring"));

    // Gray out selection if feature is not supported.
    if (!mEngine.PowerMonitoringSupported())
        {
        mPowerMonitoringEnabledItem->setEnabled(false);
        }
    
    // Data popup page
    HbDataFormModelItem *dataPopupPage = model.appendDataFormPage(tr("Data popup"));

    mPopupVisibilityItem = model.appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("Visibility"),
            dataPopupPage);
    mPopupVisibilityItem->setContentWidgetData("items", POPUP_VISIBILITY);

    mPopupLocationItem = model.appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("Location"), dataPopupPage);
    mPopupLocationItem->setContentWidgetData("items", POPUP_LOCATION);

    mPopupSourcesItem = model.appendDataFormItem(
            HbDataFormModelItem::MultiselectionItem, tr("Sources"), dataPopupPage);
    mPopupSourcesItem->setContentWidgetData("items", SOURCES);

    // Graphs page
    HbDataFormModelItem *graphsPage = model.appendDataFormPage(tr("Graphs"));

    mVerticalBarPeriodItem = model.appendDataFormItem(
            HbDataFormModelItem::TextItem, tr("Vertical bar period (s)"), graphsPage);
    mVerticalBarPeriodItem->setContentWidgetData("maximum", 999);
    mVerticalBarPeriodItem->setContentWidgetData("minimum", 0);
    mVerticalBarPeriodItem->setContentWidgetData("inputMethodHints", Qt::ImhDigitsOnly);

    mGraphSourcesItem = model.appendDataFormItem(
            HbDataFormModelItem::MultiselectionItem, tr("Sources"), graphsPage);
    mGraphSourcesItem->setContentWidgetData("items", SOURCES);

    // Logging page
    mLogPage = model.appendDataFormPage(tr("Logging"));

    mLogModeItem = model.appendDataFormItem(
            HbDataFormModelItem::RadioButtonListItem, tr("Mode"), mLogPage);
    mLogModeItem->setContentWidgetData("items", LOG_MODES);
    mSettingsForm->addConnection(mLogModeItem, SIGNAL(itemSelected(int)),
                                 this, SLOT(logModeChanged(int)));

    mLogSourcesItem = model.appendDataFormItem(
            HbDataFormModelItem::MultiselectionItem, tr("Sources"), mLogPage);
    mLogSourcesItem->setContentWidgetData("items", SOURCES);
}

void SettingsView::load(const PerfMonSettings& settings)
{
    mHeartBeatItem->setContentWidgetData("text", settings.heartBeat());
    mMaxSamplesItem->setContentWidgetData("text", settings.maxSamples());
    mPriorityItem->setContentWidgetData("selected", settings.priority());
    mCpuSamplingItem->setContentWidgetData("selected", settings.cpuMode());
    mKeepBacklightItem->setContentWidgetData("checkState", settings.keepBacklightOn() ? Qt::Checked : Qt::Unchecked);
    mPowerMonitoringEnabledItem->setContentWidgetData("checkState", settings.powerMonitoringEnabled() ? Qt::Checked : Qt::Unchecked);

    mPopupVisibilityItem->setContentWidgetData("selected", settings.dataPopupVisibility());
    mPopupLocationItem->setContentWidgetData("selected", settings.dataPopupLocation());
    mPopupSourcesItem->setContentWidgetData("selectedItems",
            qVariantFromValue(settings.dataPopupSources().enabledIndexes()));

    mVerticalBarPeriodItem->setContentWidgetData("text", settings.graphVerticalBarPeriod());
    mGraphSourcesItem->setContentWidgetData("selectedItems",
            qVariantFromValue(settings.graphSources().enabledIndexes()));

    mLogModeItem->setContentWidgetData("selected", settings.loggingMode());
    mLogFilePath = settings.loggingFilePath();
    if (settings.loggingMode() > ELoggingModeRDebug)
        createLogFilePathItem();
    mLogSourcesItem->setContentWidgetData("selectedItems",
            qVariantFromValue(settings.loggingSources().enabledIndexes()));
}

void SettingsView::save(PerfMonSettings& settings)
{
    settings.setHeartBeat(mHeartBeatItem->contentWidgetData("text").toInt());
    settings.setMaxSamples(mMaxSamplesItem->contentWidgetData("text").toInt());
    settings.setPriority(mPriorityItem->contentWidgetData("selected").toInt());
    settings.setCpuMode(mCpuSamplingItem->contentWidgetData("selected").toInt());
    settings.setKeepBacklightOn(mKeepBacklightItem->contentWidgetData("checkState").toInt() == Qt::Checked);
    settings.setPowerMonitoringEnabled(mPowerMonitoringEnabledItem->contentWidgetData("checkState").toInt() == Qt::Checked);

    settings.setDataPopupVisibility(mPopupVisibilityItem->contentWidgetData("selected").toInt());
    settings.setDataPopupLocation(mPopupLocationItem->contentWidgetData("selected").toInt());
    settings.dataPopupSources().setEnabledIndexes(
            qVariantValue<QList<QVariant> >(mPopupSourcesItem->contentWidgetData("selectedItems")));

    settings.setGraphVerticalBarPeriod(mVerticalBarPeriodItem->contentWidgetData("text").toInt());
    settings.graphSources().setEnabledIndexes(
            qVariantValue<QList<QVariant> >(mGraphSourcesItem->contentWidgetData("selectedItems")));

    settings.setLoggingMode(mLogModeItem->contentWidgetData("selected").toInt());
    
    if (mLogFilePathItem)
        settings.setLoggingFilePath(mLogFilePathItem->contentWidgetData("text").toString());
    settings.loggingSources().setEnabledIndexes(
            qVariantValue<QList<QVariant> >(mLogSourcesItem->contentWidgetData("selectedItems")));
}

void SettingsView::dataItemDisplayed(const QModelIndex &index)
{
    HbDataFormModelItem* modelItem = mModel->itemFromIndex(index);
    HbDataFormViewItem *viewItem = static_cast<HbDataFormViewItem*>(mSettingsForm->itemByIndex(index));
    HbWidget *dataContentWidget = viewItem->dataItemContentWidget();

    // set input method hint for edits
    // TODO: remove once setContentWidgetData works with inputMethodHints
    if (modelItem == mHeartBeatItem ||
        modelItem == mMaxSamplesItem ||
        modelItem == mVerticalBarPeriodItem)
    {
        HbLineEdit *edit = static_cast<HbLineEdit*>(dataContentWidget);
        edit->setInputMethodHints(Qt::ImhDigitsOnly);
    }
}

void SettingsView::logModeChanged(int index)
{
    if (index == ELoggingModeRDebug && mLogFilePathItem) {
        removeLogFilePathItem();
    }
    else if (index != ELoggingModeRDebug && !mLogFilePathItem) {
        createLogFilePathItem();
    }
}

void SettingsView::createLogFilePathItem()
{
    mLogFilePathItem = mModel->insertDataFormItem(mLogPage->indexOf(mLogModeItem) + 1,
            HbDataFormModelItem::TextItem, tr("Log file path"), mLogPage);
    mLogFilePathItem->setContentWidgetData("text", mLogFilePath);
}

void SettingsView::removeLogFilePathItem()
{
    // we need to store current text so that it is not lost when 
    // user hides and then displays "Log file path" item again.
    mLogFilePath = mLogFilePathItem->contentWidgetData("text").toString();
    mModel->removeItem(mLogFilePathItem);
    mLogFilePathItem = 0;
}

void SettingsView::accept()
{
    save(mEngine.settings());
    mEngine.updateSettings();
    emit finished(true);
}

void SettingsView::reject()
{
    load(mEngine.settings());
    emit finished(false);
}
