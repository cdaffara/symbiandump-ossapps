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
 *     The source file for messaging's audio fetcher dialog.
 *
 */

// SYSTEM INCLUDES
#include <hbaction.h>
#include <HbStyleLoader>
#include <HbTextItem>
#include <HbColorScheme>
#include <HbListView>
#include <QModelIndex>
#include <QStandardItemModel>

// USER INCLUDES
#include "msgaudiofetcherdialog.h"
#include "msgaudiofetchermodel.h"
#include "msgaudiopreview.h"

#define LOC_SELECT_SELECT hbTrId("txt_common_button_select")
#define LOC_SELECT_CANCEL hbTrId("txt_common_button_cancel")
#define LOC_SELECT_SOUND  hbTrId("txt_messaging_title_select_a_sound")

const QString LIST_ITEM_TITLE("qtc_list_item_title_normal");

MsgAudioFetcherDialog::MsgAudioFetcherDialog(QGraphicsItem *parent) :
	HbDialog(parent),
	mListView(NULL),
	mFetcherModel(NULL),
	mAudioPreview(NULL),
	mSelected(false)
{
    setDismissPolicy(HbDialog::NoDismiss);
    setFrameType(HbDialog::Strong);
    setModal(true);
    
    initActions();
    initMainWidget();  
    
    connect(
            this, 
            SIGNAL(aboutToClose()), 
            this, 
            SLOT(reset()));
    
    //TODO: Goal was to put in viewReady, but connect is
    //not working since it only works for views not dialogs
    doDelayedConstruction();
}

MsgAudioFetcherDialog::~MsgAudioFetcherDialog()
{
    //do nothing
}

void MsgAudioFetcherDialog::initMainWidget()
{
	//set heading
	HbTextItem* heading = new HbTextItem(
	        LOC_SELECT_SOUND, 
	        this);
	QColor color = HbColorScheme::color( LIST_ITEM_TITLE );
	heading->setTextColor( color );
	heading->setAlignment(Qt::AlignLeft);
	setHeadingWidget(heading);	
}

void MsgAudioFetcherDialog::initActions()
{
    mLeftAction = new HbAction(this);
    mLeftAction->setText(LOC_SELECT_SELECT);
    addAction(mLeftAction);
    mLeftAction->setEnabled(false);

    HbAction* rightAction = new HbAction(this);
    rightAction->setText(LOC_SELECT_CANCEL);
    rightAction->setEnabled(true);
    addAction(rightAction);    
    
    connect(mLeftAction,
                SIGNAL(triggered()),
                this,
                SLOT(onSelectAction()));
    
    connect(rightAction,
                SIGNAL(triggered()),
                this,
                SLOT(onCancelAction()));    
}

QString MsgAudioFetcherDialog::getCurrentItemPath()
{
    if (mSeletedItem.isValid())
    {
        QString temppath = 
                    mFetcherModel->data(
                            mSeletedItem, 
                            Qt::UserRole).toString();
        //return the path
        return QDir::toNativeSeparators(temppath);
    }
    return QString();
}

void MsgAudioFetcherDialog::doDelayedConstruction()
{
    //set the content list
    mListView = new HbListView(this);
    Q_ASSERT(mListView);
    mListView->setSelectionMode(
            HbAbstractItemView::SingleSelection);
    mFetcherModel = new MsgAudioFetcherModel(this);
    mListView->setModel(mFetcherModel);
    mListView->setUniformItemSizes(true);
    setContentWidget(mListView);
    connect(mListView, SIGNAL(activated(QModelIndex)),
            this, SLOT(on_list_activated(QModelIndex)));
    
    //initializes the audio preview
    mAudioPreview = new MsgAudioPreview(this);
}

void MsgAudioFetcherDialog::onSelectAction()
{
	//stop the playing file
    if (mAudioPreview->IsPlaying()) {
            mAudioPreview->Stop();        
        }
    
    //emit the selected sound file
    QString filePath = getCurrentItemPath();
    emit audioSelected(filePath);
    
    //close the dialog
    close();
}

void MsgAudioFetcherDialog::onCancelAction()
{    
    //stop the playing file and return
    if (mAudioPreview->IsPlaying()) {
                mAudioPreview->Stop();        
            }
    close();
    return;
}

void MsgAudioFetcherDialog::reset()
{
    mSelected = false;
    mLeftAction->setEnabled(false);
    
    //list scroll to top
    mListView->scrollTo(
            mListView->model(
                    )->index(0,0));
    
    //deselect the selected item
    mListView->selectionModel(
            )->select(
                    mSeletedItem, 
                    QItemSelectionModel::Deselect);    
}

void MsgAudioFetcherDialog::on_list_activated(
        const QModelIndex &index)
{
    //stop previewing when clicking another item.
    if (mAudioPreview->IsPlaying()) {
        mAudioPreview->SetAttr(getCurrentItemPath());
        mAudioPreview->Play();        
    }

    //enable the action select
    if(!mSelected)
    {
        mLeftAction->setEnabled(true);        
        mSelected = true;
    }     
    
    //save the latest selected
    mSeletedItem = index;
    
    //start play the item
    mAudioPreview->SetAttr(getCurrentItemPath());
    mAudioPreview->Play();
    
    return;
}
//End of File
