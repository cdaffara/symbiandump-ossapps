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
 *     The source file for tone fetcher widget.
 *     
 */
#include "tonefetcherwidget.h"
#include "hbabstractviewitem.h"
#include "hbstyle.h"
#include "hbabstractitemview.h"
#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <QModelIndex>
#include <QGraphicsLinearLayout>
#include <QDirModel>
#include <QTime>
#include <QFileInfo>
#include <QString>
#include <QStandardItemModel>
#include <XQUtils>
#include <hblabel.h>
#include "tonefetcherview.h"
#include "tonefetchermodel.h"
#include <hbmessagebox.h>
#include <hbprogressdialog.h>

ToneFetcherWidget::ToneFetcherWidget(HbWidget *parent, ToneFetcherView *serviceView)
    : HbWidget(parent),
      mLabel(0),
      mListView(0),
      mLayout(0),
      mToneModel(0),
      mServiceView(serviceView),         
      mServiceEngine(0),
      mWaitNote(0)
      
{    
    init();
    connect(mServiceEngine, SIGNAL(mdeSessionOpened()), 
            this, SLOT(mdeSessionOpened()));
    connect(mServiceEngine, SIGNAL(mdeSessionError(int)),
            this, SLOT(mdeSessionError(int)));
    connect(mServiceEngine, SIGNAL(queryComplete(QStringList)), 
            this, SLOT(queryComplete(QStringList)));
    connect(mServiceEngine, SIGNAL(queryError(int)), 
            this, SLOT(queryError(int)));
    connect(mServiceEngine, 
           SIGNAL(notifyPreviewEvent(int)), 
           this, SLOT(previewEvent(int)));
    connect( mServiceEngine, SIGNAL(notifyObjectChanged()),
            this, SLOT(onObjectChanged()));
}

ToneFetcherWidget::~ToneFetcherWidget()
{
    delete mToneModel;
    mToneModel = 0;
    delete mWaitNote;
    mWaitNote = 0;
}

void ToneFetcherWidget::on_list_activated(const QModelIndex &index)
{  
    //stop previewing when clicking another item.
    if (mServiceEngine->isPlaying()) {
        mServiceEngine->stopPlaying();
    }
    /*
     * when one item is selected, reselecting it will deselect it. selecting another 
     * will also deselect it, while the other is selected.
     */
    QItemSelectionModel *selectionModel = mListView->selectionModel();        
    if (mOldSeletedItem == index) {
        selectionModel->select(index,QItemSelectionModel::Toggle);
    }
    
    QModelIndexList modelIndexList = selectionModel->selectedIndexes();
    if (modelIndexList.isEmpty()) {
        mOldSeletedItem = QModelIndex();
    }
    else {
        mOldSeletedItem = modelIndexList.front();
    }
    emit triggerToolBar(selectionModel->hasSelection());

    
}

void ToneFetcherWidget::init()
{
    mLayout = new QGraphicsLinearLayout(this);
    mLayout->setOrientation(Qt::Vertical);
    setLayout(mLayout);

    mLabel = new HbLabel(this);
    mLabel->setPlainText(hbTrId("Select tone"));
    mLayout->addItem(mLabel);
    
    mListView = new HbListView(this);
    mListView->setObjectName("list");
    mLayout->addItem(mListView);
    mListView->setSelectionMode(HbAbstractItemView::SingleSelection);
     
    mServiceEngine = new ToneFetcherEngine(this);     
    mToneModel = new ToneFetcherModel(this);    
    
    initRomSoundList();    
    
    connect(mListView, SIGNAL(activated(QModelIndex)),
        this, SLOT(on_list_activated(QModelIndex )));
    if( !mWaitNote ){
        mWaitNote = new HbProgressDialog( HbProgressDialog::WaitDialog );
        mWaitNote->setText( hbTrId( "Refreshing..." ) );
        QAction *action = mWaitNote->actions().at(0);//disable Cancel buttion.
        action->setEnabled(false);
    }
}

void ToneFetcherWidget::mdeSessionOpened()
{
    mServiceEngine->getTones();
}

void ToneFetcherWidget::queryComplete(const QStringList &uriList)
{
    addFilesFromMDE(uriList);
    addFilesFromRom();
    mToneModel->sort();
    mToneModel->layoutHasChanged();
    if (!mListView->model()) {    
        mListView->setModel(mToneModel);
    }
    refreshFinish();
}

void ToneFetcherWidget::queryError(int error)
{
    Q_UNUSED(error);
        
}

void ToneFetcherWidget::mdeSessionError(int error)
{
    Q_UNUSED(error);
}

QString ToneFetcherWidget::getCurrentItemPath() 
{
    QModelIndexList modelIndexList = mListView->selectionModel()->selectedIndexes();
    if (modelIndexList.count() > 0) {
        QModelIndex index = modelIndexList.front();
        QString path = mToneModel->getPath(index);
        return path;
    }
    return QString();
}

void ToneFetcherWidget::playOrPause() 
{
    if(mServiceEngine->isPlaying()) {
        mServiceEngine->stopPlaying();
    } else {    
        mServiceEngine->play(getCurrentItemPath());
    }
    
}

void ToneFetcherWidget::previewEvent(int event) 
{
    if (event == 0) {
        //preview successful, reserved
    } else {
        HbMessageBox::information(QString(hbTrId("Preview Error")));
    }
}

void ToneFetcherWidget::onObjectChanged()
{
    refreshStart();
    if (mServiceEngine->isPlaying()) {
        mServiceEngine->stopPlaying();      
    }
    mToneModel->layoutToBeChanged();
    emit triggerToolBar(false);    
    mToneModel->removeRows(0, mToneModel->rowCount());           
    mServiceEngine->getTones();    
}

void ToneFetcherWidget::addFilesFromRom() 
{     
    int currentCount = mToneModel->rowCount();
    mToneModel->insertRows(currentCount, mRomSoundList.size());
    for (int i = 0; i < mRomSoundList.size(); ++i) {
        mToneModel->setData(mToneModel->index(i + currentCount), 
            QFileInfo(mRomSoundList.at(i)).absoluteFilePath());
    }  
}

void ToneFetcherWidget::addFilesFromMDE(const QStringList &uriList)
{
    int currentCount = mToneModel->rowCount();
    mToneModel->insertRows(currentCount, uriList.size());
    for (int i = 0; i < uriList.size(); ++i) {
        mToneModel->setData(mToneModel->index(i + currentCount), QFileInfo(uriList.at(i)).absoluteFilePath());
    }   
}

void ToneFetcherWidget::refreshFinish()
{
    if (mWaitNote) {
        mWaitNote->close();
    }
}

void ToneFetcherWidget::refreshStart()
{
    if (mWaitNote) {
        mWaitNote->open();
    }
}

void ToneFetcherWidget::initRomSoundList()
{
    QDir digitalSoundPath(XQUtils::romRootPath() + XQUtils::digitalSoundsPath());
    QDir simpleSoundPath(XQUtils::romRootPath() + XQUtils::simpleSoundsPath());
    mRomSoundList = digitalSoundPath.entryInfoList() + simpleSoundPath.entryInfoList();
}
//End of File
