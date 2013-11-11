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

#include "fbsettingsview.h"
#include "enginewrapper.h"
#include "filebrowsersettings.h"

#include <HbView>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbAction>
#include <HbToolBar>

#include <QGraphicsLinearLayout>

const QStringList DisplayModeModes = (QStringList() << "Full screen" << "Normal");
const QStringList FileViewModes = (QStringList() << "Simple" << "Extended");
const QStringList Modes = (QStringList() << "Yes" << "No");
const QString YesText = QString("Yes");
const QString NoText = QString("No");
const QString acceptActionText = "OK";
const QString rejectActionText = "Cancel";

FbSettingsView::FbSettingsView(EngineWrapper &engineWrapper) :
    mEngineWrapper(engineWrapper),
    mForm(0),
    mDisplayModeItem(0),
    mFileViewItem(0),
    mShowDirectoryInfoItem(0),
    mShowAssociatedIconsItem(0),
    mRememberFolderOnExitItem(0),
    mRememberLastFolderItem(0),
    mShowToolbarItem(0),
    mSupportNetDrivesItem(0),
    mBypassPlatformSecurityItem(0),
    mUnlockFilesViaSBItem(0),
    mIgnoreProtectionAttributesItem(0),
    mNoROAttributeCopyFromZItem(0)
{
    setTitle("Settings");

    // Override back navigation action
    HbAction *leaveViewAction = new HbAction(Hb::BackNaviAction, this);
    connect(leaveViewAction, SIGNAL(triggered()), this, SLOT(reject()));
    setNavigationAction(leaveViewAction);

    initDataForm();
    createToolbar();
}

FbSettingsView::~FbSettingsView()
{
}

void FbSettingsView::initDataForm()
{
    mForm = new HbDataForm(this);
    HbDataFormModel *model = new HbDataFormModel();

//    connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
//            this, SLOT(toggleChange(QModelIndex, QModelIndex)));


    // TODO to be done
//    mDisplayModeItem = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem);
//    mDisplayModeItem->setLabel("Display mode: ");
//    mDisplayModeItem->setContentWidgetData("items", DisplayModeModes);
//
//    mFileViewItem = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem);
//    mFileViewItem->setLabel("File view: ");
//    mFileViewItem->setContentWidgetData("items", FileViewModes);
//
//    mShowDirectoryInfoItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
//    mShowDirectoryInfoItem->setLabel("Show directory info: ");
//    mShowDirectoryInfoItem->setContentWidgetData(QString("text"), YesText);
//    mShowDirectoryInfoItem->setContentWidgetData(QString("additionalText"), NoText);
//
//    mShowAssociatedIconsItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
//    mShowAssociatedIconsItem->setLabel("Show associated icons: ");
//    mShowAssociatedIconsItem->setContentWidgetData(QString("text"), YesText);
//    mShowAssociatedIconsItem->setContentWidgetData(QString("additionalText"), NoText);
//
//    mRememberFolderOnExitItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
//    mRememberFolderOnExitItem->setLabel("Remember folder on exit: ");
//    mRememberFolderOnExitItem->setContentWidgetData(QString("text"), YesText);
//    mRememberFolderOnExitItem->setContentWidgetData(QString("additionalText"), NoText);
//
//    mRememberLastFolderItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
//    mRememberLastFolderItem->setLabel("Remember last folder: ");
//    mRememberLastFolderItem->setContentWidgetData(QString("text"), YesText);
//    mRememberLastFolderItem->setContentWidgetData(QString("additionalText"), NoText);
//
//    mShowToolbarItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
//    mShowToolbarItem->setLabel("Show toolbar: ");
//    mShowToolbarItem->setContentWidgetData(QString("text"), YesText);
//    mShowToolbarItem->setContentWidgetData(QString("additionalText"), NoText);

    mSupportNetDrivesItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
    mSupportNetDrivesItem->setLabel("Support net drives: ");
    mSupportNetDrivesItem->setContentWidgetData(QString("text"), YesText);
    mSupportNetDrivesItem->setContentWidgetData(QString("additionalText"), NoText);

    mBypassPlatformSecurityItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
    mBypassPlatformSecurityItem->setLabel("Bypass plat.security: ");
    mBypassPlatformSecurityItem->setContentWidgetData(QString("text"), YesText);
    mBypassPlatformSecurityItem->setContentWidgetData(QString("additionalText"), NoText);

    mUnlockFilesViaSBItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
    mUnlockFilesViaSBItem->setLabel("Unlock files via SB: ");
    mUnlockFilesViaSBItem->setContentWidgetData(QString("text"), YesText);
    mUnlockFilesViaSBItem->setContentWidgetData(QString("additionalText"), NoText);

    mIgnoreProtectionAttributesItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
    mIgnoreProtectionAttributesItem->setLabel("Ign. protection atts: ");
    mIgnoreProtectionAttributesItem->setContentWidgetData(QString("text"), YesText);
    mIgnoreProtectionAttributesItem->setContentWidgetData(QString("additionalText"), NoText);

    mNoROAttributeCopyFromZItem = new HbDataFormModelItem(HbDataFormModelItem::ToggleValueItem);
    mNoROAttributeCopyFromZItem->setLabel("No RO-att copy from Z: ");
    mNoROAttributeCopyFromZItem->setContentWidgetData(QString("text"), YesText);
    mNoROAttributeCopyFromZItem->setContentWidgetData(QString("additionalText"), NoText);

    // load file search attribute values from FB engine:
    loadSettings(mEngineWrapper.settings());

//    model->appendDataFormItem(mDisplayModeItem);
//    model->appendDataFormItem(mFileViewItem);
//    model->appendDataFormItem(mShowDirectoryInfoItem);
//    model->appendDataFormItem(mShowAssociatedIconsItem);
//    model->appendDataFormItem(mRememberFolderOnExitItem);
//    model->appendDataFormItem(mRememberLastFolderItem);
//    model->appendDataFormItem(mShowToolbarItem);

    model->appendDataFormItem(mSupportNetDrivesItem);
    model->appendDataFormItem(mBypassPlatformSecurityItem);
    model->appendDataFormItem(mUnlockFilesViaSBItem);
    model->appendDataFormItem(mIgnoreProtectionAttributesItem);
    model->appendDataFormItem(mNoROAttributeCopyFromZItem);

    mForm->setModel(model);

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical,this);
    layout->addItem(mForm);
    setLayout(layout);
}

void FbSettingsView::createToolbar()
{
    HbToolBar *toolBar = new HbToolBar(this);
    HbAction *acceptAction = new HbAction(acceptActionText);
    connect(acceptAction, SIGNAL(triggered()), this, SLOT(accept()));
    toolBar->addAction(acceptAction);

    HbAction *rejectAction = new HbAction(rejectActionText);
    connect(rejectAction, SIGNAL(triggered()), this, SLOT(reject()));
    toolBar->addAction(rejectAction);

    setToolBar(toolBar);
}

/**
  Set form item values from settings \a settings.
 */
void FbSettingsView::loadSettings(const FileBrowserSettings &settings)
{
//    mDisplayModeItem->setContentWidgetData("text", settings.displayMode());
//    mFileViewItem->setContentWidgetData("text", settings.fileViewMode());
//    mShowDirectoryInfoItem->setContentWidgetData("text", settings.showSubDirectoryInfo());
//    mShowAssociatedIconsItem->setContentWidgetData("text", settings.showAssociatedIcons());
//    mRememberFolderOnExitItem->setContentWidgetData("text", settings.rememberFolderSelection());
//    mRememberLastFolderItem->setContentWidgetData("text", settings.rememberLastPath());
//    mShowToolbarItem->setContentWidgetData("text", settings.enableToolbar());

    mSupportNetDrivesItem->setContentWidgetData("text", settings.supportNetworkDrives() ? YesText : NoText);
    mSupportNetDrivesItem->setContentWidgetData("additionalText", !settings.supportNetworkDrives() ? YesText : NoText);

    mBypassPlatformSecurityItem->setContentWidgetData("text", settings.bypassPlatformSecurity() ? YesText : NoText);
    mBypassPlatformSecurityItem->setContentWidgetData("additionalText", !settings.bypassPlatformSecurity() ? YesText : NoText);

    mUnlockFilesViaSBItem->setContentWidgetData("text", settings.removeFileLocks() ? YesText : NoText);
    mUnlockFilesViaSBItem->setContentWidgetData("additionalText", !settings.removeFileLocks() ? YesText : NoText);

    mIgnoreProtectionAttributesItem->setContentWidgetData("text", settings.ignoreProtectionsAttributes() ? YesText : NoText);
    mIgnoreProtectionAttributesItem->setContentWidgetData("additionalText", !settings.ignoreProtectionsAttributes() ? YesText : NoText);

    mNoROAttributeCopyFromZItem->setContentWidgetData("text", settings.removeROMWriteProrection() ? YesText : NoText);
    mNoROAttributeCopyFromZItem->setContentWidgetData("additionalText", !settings.removeROMWriteProrection() ? YesText : NoText);
}

/**
  Store form item data into \a settings
 */
void FbSettingsView::saveSettings(FileBrowserSettings &settings)
{
//    settings.setDisplayMode(mDisplayModeItem->contentWidgetData("text").toInt());
//    settings.setFileViewMode(mFileViewItem->contentWidgetData("text").toInt());
//    settings.setShowSubDirectoryInfo(mShowDirectoryInfoItem->contentWidgetData("text").toBool());
//    settings.setShowAssociatedIcons(mShowAssociatedIconsItem->contentWidgetData("text").toBool());
//    settings.setRememberLastPath(mRememberFolderOnExitItem->contentWidgetData("text").toBool());
//    //settings.setLastPath(mRememberLastFolderItem->contentWidgetData("text").toString());
//    settings.setRememberFolderSelection(mRememberLastFolderItem->contentWidgetData("text").toBool());
//    settings.setEnableToolbar(mShowToolbarItem->contentWidgetData("text").toBool());

    settings.setSupportNetworkDrives(mSupportNetDrivesItem->contentWidgetData("text").toString() == YesText);
    settings.setBypassPlatformSecurity(mBypassPlatformSecurityItem->contentWidgetData("text").toString() == YesText);
    settings.setRemoveFileLocks(mUnlockFilesViaSBItem->contentWidgetData("text").toString() == YesText);
    settings.setIgnoreProtectionsAttributes(mIgnoreProtectionAttributesItem->contentWidgetData("text").toString() == YesText);
    settings.setRemoveROMWriteProrection(mNoROAttributeCopyFromZItem->contentWidgetData("text").toString() == YesText);
}

void FbSettingsView::constructMenu()
{
    
}

void FbSettingsView::accept()
{
    saveSettings(mEngineWrapper.settings());
    mEngineWrapper.saveSettings();
    emit finished(true);
}

void FbSettingsView::reject()
{
    emit finished(false);
}

// End of file
