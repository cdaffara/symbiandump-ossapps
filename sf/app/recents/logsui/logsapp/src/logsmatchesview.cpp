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
//USER
#include "logsmatchesview.h"
#include "logscomponentrepository.h"
#include "logsabstractviewmanager.h"
#include "logsdefs.h"
#include "logslogger.h"
#include "logsmodel.h"
#include "logsmatchesmodel.h"
#include "logscall.h"
#include "logscontact.h"

//SYSTEM
#include <hblistview.h>
#include <hbaction.h>
#include <dialpad.h>
#include <hblineedit.h>
#include <hbabstractviewitem.h>
#include <hblistviewitem.h>
#include <hbpushbutton.h>

Q_DECLARE_METATYPE(LogsMatchesModel*)

// -----------------------------------------------------------------------------
// LogsMatchesView::LogsMatchesView
// -----------------------------------------------------------------------------
//
LogsMatchesView::LogsMatchesView( 
    LogsComponentRepository& repository, LogsAbstractViewManager& viewManager )
    : LogsBaseView(LogsMatchesViewId, repository, viewManager),
      mListView(0),
      mModel(0),
      mAddToContactsButton(0)
{
    LOGS_QDEBUG( "logs [UI] <-> LogsMatchesView::LogsMatchesView()" );
    
    mActivities.append( logsActivityIdViewMatches );
}
    
// -----------------------------------------------------------------------------
// LogsMatchesView::~LogsMatchesView
// -----------------------------------------------------------------------------
//
LogsMatchesView::~LogsMatchesView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::~LogsMatchesView()" );
    delete mModel;
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::~LogsMatchesView()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::activated
// -----------------------------------------------------------------------------
//
void LogsMatchesView::activated(bool showDialer, QVariant args)
{
    LogsBaseView::activated(showDialer, args);
    
    LogsMatchesModel* model = qVariantValue<LogsMatchesModel*>(args);
    updateModel(model);
    dialpadEditorTextChanged();
    
    activateEmptyListIndicator(mModel);
    
    scrollToTopItem(mListView);
    
    LogsBaseView::activationCompleted();
}

// -----------------------------------------------------------------------------
// LogsMatchesView::deactivated
// -----------------------------------------------------------------------------
//
void LogsMatchesView::deactivated()
{
    //base class handling first
    LogsBaseView::deactivated();
    
    deactivateEmptyListIndicator(mModel);
    
    delete mModel;
    mModel = 0;
}

// -----------------------------------------------------------------------------
// LogsMatchesView::initView
// -----------------------------------------------------------------------------
//
void LogsMatchesView::initView()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::initView()" );    
    //base class handling first
    LogsBaseView::initView();
    
    initListWidget();

    mAddToContactsButton = qobject_cast<HbPushButton*>(
            mRepository.findWidget( logsButtonAddToContactsId ) );

    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::initView()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QAbstractItemModel* LogsMatchesView::model() const
{
    return mModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HbListView* LogsMatchesView::listView() const
{
    return mListView;
}

// -----------------------------------------------------------------------------
// LogsMatchesView::initListWidget
// -----------------------------------------------------------------------------
//
void LogsMatchesView::initListWidget()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::initListWidget()" );
    mListView = qobject_cast<HbListView*>
                    ( mRepository.findWidget( logsListWidgetId ) );
    Q_ASSERT_X(mListView != 0, "logs [UI] ", "couldn't find list widget !!");
    
    mListView->setItemRecycling(true);
    mListView->setFrictionEnabled(true);
    mListView->listItemPrototype()->setTextFormat(Qt::RichText);
    
    connect(mListView, SIGNAL(activated(const QModelIndex)),
            this, SLOT(initiateCallback(const QModelIndex)), 
            Qt::UniqueConnection);
    
    connect(mListView,
            SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
            this,
            SLOT(showListItemMenu(HbAbstractViewItem*, const QPointF&)), 
            Qt::UniqueConnection);
   
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::initListWidget() " );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::updateModel
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateModel(LogsMatchesModel* model)
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::updateModel()" );
    
    if ( !model || model != mModel ){
        LOGS_QDEBUG( "logs [UI]     Delete old model" );
        delete mModel;
        mModel = 0;
    }
    
    if ( !model ){
        LOGS_QDEBUG( "logs [UI]     Create new model" );
        mModel = mRepository.model()->logsMatchesModel();
    } else {
        LOGS_QDEBUG( "logs [UI]     Use given model" );
        mModel = model;
    }
    if ( mListView ) {
        mListView->setModel( mModel );//ownership not transferred
    }
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::updateModel()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::::handleBackSoftkey
// -----------------------------------------------------------------------------
//
void LogsMatchesView::handleBackSoftkey()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::::handleBackSoftkey()" );
    mDialpad->editor().blockSignals(true);
    mDialpad->editor().setText(QString());
    mDialpad->editor().blockSignals(false);
    
    if (mDialpad->isOpen()){
        LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::::handleBackSoftkey() closeDialpad" );
        // Block aboutToClose signal to interfere with layout loading 
        // at backstepping phase
        mDialpad->blockSignals(true);
        mDialpad->closeDialpad();
        mDialpad->blockSignals(false);
    }

    LogsBaseView::handleBackSoftkey();

    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::::handleBackSoftkey()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::dialpadEditorTextChanged
// -----------------------------------------------------------------------------
//
void LogsMatchesView::dialpadEditorTextChanged()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::::dialpadEditorTextChanged()" );
    updateCallButton();
    updateMenu();
    
    QString pattern = mDialpad->editor().text();
    if ( pattern.isEmpty() ){
        // Treat empty input field same way as back press
        LogsBaseView::handleBackSoftkey();
    } else if ( mModel ) {
        mModel->logsMatches( pattern );
    }
    LOGS_QDEBUG_2( "logs [UI] <- LogsMatchesView::::dialpadEditorTextChanged(), text: ", pattern );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::dialpadOpened
// -----------------------------------------------------------------------------
//
void LogsMatchesView::dialpadOpened()
{
    updateWidgetsSizeAndLayout();
    updateAddContactButton();
}

// -----------------------------------------------------------------------------
// LogsMatchesView::dialpadClosed
// -----------------------------------------------------------------------------
//
void LogsMatchesView::dialpadClosed()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::dialpadClosed()" );
    updateWidgetsSizeAndLayout();
    updateAddContactButton();
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::dialpadClosed()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::updateWidgetsSizeAndLayout
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateWidgetsSizeAndLayout()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::updateWidgetsSizeAndLayout()" );
    if ( mListView ) {
        updateMenu();
        updateListLayoutName(*mListView);
        updateListSize(*mListView);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::updateWidgetsSizeAndLayout()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::updateEmptyListWidgetsVisibility
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateEmptyListWidgetsVisibility()
{
    updateEmptyListLabelVisibility();
    updateAddContactButton();
}

// -----------------------------------------------------------------------------
// LogsMatchesView::toggleContactSearch
// -----------------------------------------------------------------------------
//
void LogsMatchesView::toggleContactSearch()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::toggleContactSearch()" );

    if ( isContactSearchEnabled() ){
        int result = mModel->setPredictiveSearch( false );
        LogsBaseView::handleBackSoftkey();
	}
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::toggleContactSearch()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateMenu()
{
    LOGS_QDEBUG( "logs [UI] -> LogsMatchesView::updateMenu()" );
    
    updateDialpadCallAndMessagingActions();    
    updateContactSearchAction();
    updateMenuVisibility();
	
    LOGS_QDEBUG( "logs [UI] <- LogsMatchesView::updateMenu()" );
}

// -----------------------------------------------------------------------------
// LogsMatchesView::updateAddContactButton
// -----------------------------------------------------------------------------
//
void LogsMatchesView::updateAddContactButton()
{
    if (mAddToContactsButton) {
        LOGS_QDEBUG( "logs [UI] <-> LogsMatchesView::updateAddContactButton()" );
        // Disable add to contacts button handling while view is being activated
        // to avoid unnecessary quick appear/dissappear of it.
        bool matchesFound(model() && (model()->rowCount() > 0));
        mAddToContactsButton->setVisible(
            !mActivating && !matchesFound && isDialpadInput() );
    }
}
