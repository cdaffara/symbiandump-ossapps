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
#include "lcfsearchresultsview.h"
#include "lcfmodel.h"
#include "logslogger.h"
#include "logscntfinder.h"

//SYSTEM
#include <hbview.h>
#include <hblistview.h>
#include <hblistviewitem.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <qgraphicslayout.h>
#include <hbtextedit.h>
#include <hbpushbutton.h>


void LcfSearchResultsView::init( LcfViewLoader& loader )
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::init()" );
		
    mListView = qobject_cast<HbListView*>( loader.findWidget( "logs_list_view" ) );
    mListView->setItemRecycling(true);

    HbPushButton *buttons[11];
    for (int i = 0; i < 11; ++i) {
        QString text = QString::number(i);
        QString buttonString("pushButton_");    
        buttonString.append(text);
        buttons[i] = static_cast<HbPushButton*>(loader.findWidget(buttonString));
        QObject::connect(buttons[i], SIGNAL(clicked()), this, SLOT(callKeyPressed()));
    }

    mListView->listItemPrototype()->setTextFormat( Qt::RichText );
    
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::init()" );
	      
}

// -----------------------------------------------------------------------------
// LcfSearchResultsView::LcfSearchResultsView
// -----------------------------------------------------------------------------
//
LcfSearchResultsView::LcfSearchResultsView()
    : HbView(0),
      mModel(0),
      mFinder(0)
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::LcfSearchResultsView()" );
    mSoftKeyBackAction = new HbAction(Hb::BackAction, this);
    connect(mSoftKeyBackAction, SIGNAL(triggered()), this, SLOT(closeView()));
    if ( !hbInstance->allMainWindows().isEmpty() ){
    	hbInstance->allMainWindows().at(0)->addSoftKeyAction( 
    		Hb::SecondarySoftKey, mSoftKeyBackAction );
    }
    
    mFinder = new LogsCntFinder();
    mModel = new LcfModel( *mFinder );
    
    connect(mFinder, SIGNAL(queryReady()), this, SLOT(queryReady()));
    
    
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::LcfSearchResultsView()" );
}
    

// -----------------------------------------------------------------------------
// LcfSearchResultsView::~LcfSearchResultsView
// -----------------------------------------------------------------------------
//
LcfSearchResultsView::~LcfSearchResultsView()
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::~LcfSearchResultsView()" );
    delete mModel;
    delete mFinder;
    clearSoftKey();    
    delete mSoftKeyBackAction;
    
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::~LcfSearchResultsView()" );
}



void LcfSearchResultsView::queryReady()
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::queryReady()" ); 
    delete mModel;
    mModel = new LcfModel( *mFinder );
    mModel->reset( !mInput.isEmpty() );
    mListView->setModel( mModel );
    
    mListView->reset();
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::queryReady()" ); 
}


// -----------------------------------------------------------------------------
// LcfSearchResultsView::callKeyPressed
// -----------------------------------------------------------------------------
//
void LcfSearchResultsView::callKeyPressed()
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::callKeyPressed()" );
    HbPushButton* button = (HbPushButton *)sender();

    if ( button->text() == QString("C") ) {
        mInput.clear();
        mModel->reset( false );
    } else {
        mInput += button->text();
        mModel->reset( true );
    }
    setTitle( mInput );
    mFinder->predictiveSearchQuery( mInput );

    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::callKeyPressed()" );
}


// -----------------------------------------------------------------------------
// LcfSearchResultsView::closeView
// -----------------------------------------------------------------------------
//
void LcfSearchResultsView::closeView()
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::closeView()" );
    qApp->quit();
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::closeView()" );
}


// -----------------------------------------------------------------------------
// LcfSearchResultsView::clearSoftKey
// -----------------------------------------------------------------------------
//
void LcfSearchResultsView::clearSoftKey()
{
    LOGS_QDEBUG( "logs [LCFPROTO] -> LcfSearchResultsView::clearSoftKey()" );
    if ( !hbInstance->allMainWindows().isEmpty() ){
    	hbInstance->allMainWindows().at(0)->removeSoftKeyAction( 
    		Hb::SecondarySoftKey, mSoftKeyBackAction );  
    }
    LOGS_QDEBUG( "logs [LCFPROTO] <- LcfSearchResultsView::clearSoftKey()" );
}

