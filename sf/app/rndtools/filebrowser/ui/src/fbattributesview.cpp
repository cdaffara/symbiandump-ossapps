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

#include "fbattributesview.h"

#include <HbView>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbAction>
#include <HbToolBar>

#include <QGraphicsLinearLayout>

#include <f32file.h>

const QStringList AttributesChoices = QStringList() << "Skip" << "On" << "Off";
const QStringList RecurseChoices = QStringList() << "Recursive" << "Non-recursive";
const QString acceptActionText = "OK";
const QString rejectActionText = "Cancel";
const int AttributeOn = 1;
const int AttributeOff = 2;
const int AttributeEnableRecursion = 0;
const int AttributeDisableRecursion = 1;

FbAttributesView::FbAttributesView(quint32 &aSetAttMask, quint32 &aClearAttMask, bool &aRecurse) :
    mSetAttributesMask(aSetAttMask),
    mClearAttributesMask(aClearAttMask),
    mRecurse(aRecurse),
    mForm(0),
    mArchiveItem(0),
    mHiddenItem(0),
    mReadOnlyItem(0),
    mSystemItem(0),
    mRecurseItem(0)
{
    setTitle("Set attributes");

    // Override back navigation action
    HbAction *leaveViewAction = new HbAction(Hb::BackNaviAction, this);
    connect(leaveViewAction, SIGNAL(triggered()), this, SLOT(reject()));
    setNavigationAction(leaveViewAction);

    initDataForm();
    createToolbar();
}

FbAttributesView::~FbAttributesView()
{
}

void FbAttributesView::initDataForm()
{
    mForm = new HbDataForm(this);
    HbDataFormModel *model = new HbDataFormModel();

    // Create Group for sequential image components
    //mGroupSeguantialCapture = model->appendDataFormGroup(QString("Archive"), parent);

    mArchiveItem = new HbDataFormModelItem(HbDataFormModelItem::RadioButtonListItem);
    mArchiveItem->setLabel("Archive");
    mArchiveItem->setContentWidgetData(QString("items"), AttributesChoices);
    mArchiveItem->setContentWidgetData(QString("displayMode"), QString("embedded"));

    mHiddenItem = new HbDataFormModelItem(HbDataFormModelItem::RadioButtonListItem);
    mHiddenItem->setLabel("Hidden");
    mHiddenItem->setContentWidgetData(QString("items"), AttributesChoices);
    mHiddenItem->setContentWidgetData(QString("displayMode"), QString("embedded"));

    mReadOnlyItem = new HbDataFormModelItem(HbDataFormModelItem::RadioButtonListItem);
    mReadOnlyItem->setLabel("Read only");
    mReadOnlyItem->setContentWidgetData(QString("items"), AttributesChoices);
    mReadOnlyItem->setContentWidgetData(QString("displayMode"), QString("embedded"));

    mSystemItem = new HbDataFormModelItem(HbDataFormModelItem::RadioButtonListItem);
    mSystemItem->setLabel("System");
    mSystemItem->setContentWidgetData(QString("items"), AttributesChoices);
    mSystemItem->setContentWidgetData(QString("displayMode"), QString("embedded"));

    mRecurseItem = new HbDataFormModelItem(HbDataFormModelItem::RadioButtonListItem);
    mRecurseItem->setLabel("Recurse");
    mRecurseItem->setContentWidgetData(QString("items"), RecurseChoices);
    mRecurseItem->setContentWidgetData(QString("displayMode"), QString("embedded"));

    // load file search attribute values from FB engine:
    loadSettings();

    model->appendDataFormItem(mArchiveItem);
    model->appendDataFormItem(mHiddenItem);
    model->appendDataFormItem(mReadOnlyItem);
    model->appendDataFormItem(mSystemItem);
    model->appendDataFormItem(mRecurseItem);

    mForm->setModel(model);

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical,this);
    layout->addItem(mForm);
    setLayout(layout);
}

void FbAttributesView::createToolbar()
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
void FbAttributesView::loadSettings()
{
    if (mSetAttributesMask & KEntryAttArchive)
        mArchiveItem->setContentWidgetData(QString("selected"), AttributeOn);
    else if (mClearAttributesMask & KEntryAttArchive)
        mArchiveItem->setContentWidgetData(QString("selected"), AttributeOff);

    if (mSetAttributesMask & KEntryAttHidden)
        mHiddenItem->setContentWidgetData(QString("selected"), AttributeOn);
    else if (mClearAttributesMask & KEntryAttHidden)
        mHiddenItem->setContentWidgetData(QString("selected"), AttributeOff);

    if (mSetAttributesMask & KEntryAttReadOnly)
        mReadOnlyItem->setContentWidgetData(QString("selected"), AttributeOn);
    else if (mClearAttributesMask & KEntryAttReadOnly)
        mReadOnlyItem->setContentWidgetData(QString("selected"), AttributeOff);

    if (mSetAttributesMask & KEntryAttSystem)
        mSystemItem->setContentWidgetData(QString("selected"), AttributeOn);
    else if (mClearAttributesMask & KEntryAttSystem)
        mSystemItem->setContentWidgetData(QString("selected"), AttributeOff);

    if (mRecurse)
        mRecurseItem->setContentWidgetData(QString("selected"), AttributeEnableRecursion);
    else
        mRecurseItem->setContentWidgetData(QString("selected"), AttributeDisableRecursion);
}

/**
  Store form item data into \a settings
 */
void FbAttributesView::saveSettings()
{
    mSetAttributesMask = 0;
    mClearAttributesMask = 0;

    int archive = mArchiveItem->contentWidgetData(QString("selected")).toInt();
    if (archive == AttributeOn)
        mSetAttributesMask |= KEntryAttArchive;
    else if (archive == AttributeOff)
        mClearAttributesMask |= KEntryAttArchive;

    int hidden = mHiddenItem->contentWidgetData(QString("selected")).toInt();
    if (hidden == AttributeOn)
        mSetAttributesMask |= KEntryAttHidden;
    else if (hidden == AttributeOff)
        mClearAttributesMask |= KEntryAttHidden;

    int readOnly = mReadOnlyItem->contentWidgetData(QString("selected")).toInt();
    if (readOnly == AttributeOn)
        mSetAttributesMask |= KEntryAttReadOnly;
    else if (readOnly == AttributeOff)
        mClearAttributesMask |= KEntryAttReadOnly;

    int system = mSystemItem->contentWidgetData(QString("selected")).toInt();
    if (system == AttributeOn)
        mSetAttributesMask |= KEntryAttSystem;
    else if (system == AttributeOff)
        mClearAttributesMask |= KEntryAttSystem;

    int recurse = mRecurseItem->contentWidgetData(QString("selected")).toInt();
    if (recurse == AttributeEnableRecursion)
        mRecurse = ETrue;
    else if (recurse == AttributeDisableRecursion)
        mRecurse = EFalse;
}

void FbAttributesView::accept()
{
    saveSettings();
    emit finished(true);
}

void FbAttributesView::reject()
{
    emit finished(false);
}

// End of file
