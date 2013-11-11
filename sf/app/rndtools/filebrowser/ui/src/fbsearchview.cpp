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

#include "fbsearchview.h"
#include "enginewrapper.h"
#include "notifications.h"

#include <HbView>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbAction>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbDialog>
#include <HbToolBar>
#include <HbProgressDialog>

#include <QtGui>
#include <QDir>
#include <QFileInfo>

const QStringList RECURSEMODES = (QStringList() << "Non-recursive" << "Recursive");
const QString acceptActionText = "Search";
const QString rejectActionText = "Cancel";


FbSearchView::FbSearchView(EngineWrapper &engineWrapper)
    : mEngineWrapper(engineWrapper),
    mAttributes(),
    mResults(),
    mForm(0),
    mSearchFileNameItem(0),
    mWildCardItem(0),
    mHasString(0),
    mRecurse(0),
    mMinSize(0),
    mMaxSize(0),
    mMinDate(0),
    mMaxDate(0),
    mPath(),
    mProgressDialog(0)
{
    // Set title and add this view to main window
    setTitle("Search");

    // Override back navigation action
    HbAction *leaveViewAction = new HbAction(Hb::BackNaviAction, this);
    connect(leaveViewAction, SIGNAL(triggered()), this, SLOT(reject()));
    setNavigationAction(leaveViewAction);

    initDataForm();
    createToolbar();
}

FbSearchView::~FbSearchView()
{
}

void FbSearchView::initDataForm() {
    mForm = new HbDataForm(/*HbFormView::ModeEdit,*/ this);
    HbDataFormModel *model = new HbDataFormModel();

    mSearchFileNameItem = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mSearchFileNameItem->setLabel("Search dir: ");

    mWildCardItem = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mWildCardItem->setLabel("Wildcards: ");

    mHasString = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mHasString->setLabel("Has string: ");

    mRecurse = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem);
    mRecurse->setLabel("Recurse: ");
    mRecurse->setContentWidgetData("items", RECURSEMODES);

    mMinSize = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mMinSize->setLabel("Min size: ");

    mMaxSize = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mMaxSize->setLabel("Max size: ");

    mMinDate = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mMinDate->setLabel("Min date: ");
    mMinDate->setContentWidgetData("text", "01/01/1980");

    mMaxDate = new HbDataFormModelItem(HbDataFormModelItem::TextItem);
    mMaxDate->setLabel("Max date: ");
    mMaxDate->setContentWidgetData("text", "31/12/2060");

    model->appendDataFormItem(mSearchFileNameItem);
    model->appendDataFormItem(mWildCardItem);
    model->appendDataFormItem(mHasString);
    model->appendDataFormItem(mRecurse);
    model->appendDataFormItem(mMinSize);
    model->appendDataFormItem(mMaxSize);
    model->appendDataFormItem(mMinDate);
    model->appendDataFormItem(mMaxDate);
    mForm->setModel(model);

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical,this);
    layout->addItem(mForm);
    setLayout(layout);
}

void FbSearchView::createToolbar()
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

void FbSearchView::loadAttributes()
{
    // get settings from engine;
    mAttributes = mEngineWrapper.getFileSearchAttributes();

    // set data:
    mSearchFileNameItem->setContentWidgetData("text", mPath);
//    mSearchFileNameItem->setContentWidgetData("text", mAttributes.mSearchDir);
    if (!mAttributes.mDefaultWildCard) {
        mWildCardItem->setContentWidgetData("text", mAttributes.mWildCards);
    }
    mHasString->setContentWidgetData("text", mAttributes.mTextInFile);

    mRecurse->setContentWidgetData("selected", mAttributes.mRecurse);

    mMinSize->setContentWidgetData("text", mAttributes.mMinSize);
    mMaxSize->setContentWidgetData("text", mAttributes.mMaxSize);

    mMinDate->setContentWidgetData("text", mAttributes.mMinDate);
    mMaxDate->setContentWidgetData("text", mAttributes.mMaxDate);
}

/**
 * Reads form items and saves values of them into member variable mAttributes
 */
void FbSearchView::readFormItems()
{
    mAttributes.mSearchDir  = mSearchFileNameItem->contentWidgetData("text").toString();

    mAttributes.mDefaultWildCard = false;
    QString wildCard = mWildCardItem->contentWidgetData("text").toString();
    if (wildCard.trimmed().isEmpty()) {
        wildCard = QString("*");
        mAttributes.mDefaultWildCard = true;
    }
    mAttributes.mWildCards  = wildCard;
    mAttributes.mTextInFile = mHasString->contentWidgetData("text").toString();
    mAttributes.mMinSize    = mMinSize->contentWidgetData("text").toInt();
    mAttributes.mMaxSize    = mMaxSize->contentWidgetData("text").toInt();
    mAttributes.mMinDate    = mMinDate->contentWidgetData("text").toDate();
    mAttributes.mMaxDate    = mMaxDate->contentWidgetData("text").toDate();
    mAttributes.mRecurse    = mRecurse->contentWidgetData("selected").toBool();
}

void FbSearchView::open(const QString &path)
{   
    mPath = path;
    // load file search attribute values from FB engine:
    loadAttributes();
}

void FbSearchView::startFileSearch()
{
    // save form data values and set them as search attributes into FB engine. 
    readFormItems();
    mEngineWrapper.setFileSearchAttributes(mAttributes);
    
    bool err = mEngineWrapper.searchFiles();

    // show results of file search:
    mResults = mEngineWrapper.getSearchResults();
    fileSearchResults();
}

void FbSearchView::fileSearchResults()
{
    QString noteMsg("%1 entries found");
    Notifications::showInformationNote(noteMsg.arg(mEngineWrapper.itemCount()), QString(""));
}

void FbSearchView::accept()
{
    startFileSearch();
    emit finished(true);
}

void FbSearchView::reject()
{
    emit finished(false);
}
