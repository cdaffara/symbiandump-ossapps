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

#include <QString.h>

#include "cntimportsview.h"
#include "cntglobal.h"

#include "cntsimengine.h"

#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbview.h>
#include <hbmenu.h>
#include <hbframebackground.h>
#include <QStandardItemModel>
#include <hbmainwindow.h>
#include <hblabel.h>
#include <hblistview.h>
#include <QTimer>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbprogressbar.h>
#include <QGraphicsLinearLayout>
#include <hbframebackground.h>
#include <hbabstractviewitem.h>
#include <hbextendedlocale.h>
#include <hbparameterlengthlimiter.h>
#include <hblistviewitem.h>
#include <hbstringutil.h>
#include <QCoreApplication>
#include <hbdevicenotificationdialog.h>
#include <cntdebug.h>

const char *CNT_IMPORT_UI_XML = ":/xml/contacts_sim.docml";

CntImportsView::CntImportsView() : mViewManager(0),
    mSimEngine(0),
    mListView(0),
    mView(0),
    mSoftkey(0),
    mModel(0),
    mImportSimPopup(0),
    mEngine(0)
{
    CNT_ENTRY
    CNT_LOG_ARGS(QString("All bools reset"));
    bool ok = false;
    mDocumentLoader.load(CNT_IMPORT_UI_XML, &ok);

    if (ok)
    {
        mView = static_cast<HbView*>(mDocumentLoader.findWidget(QString("view")));
    }
    else
    {
        qFatal("Unable to read :/xml/contacts_sim.docml");
    }
    
    mSimEngine = new CntSimEngine(*this, *mView);

    //back button
    mSoftkey = new HbAction(Hb::BackNaviAction, mView);
    connect(mSoftkey, SIGNAL(triggered()), this, SLOT(showPreviousView()));
    connect(mSimEngine, SIGNAL(showNamesView()), this, SLOT(showPreviousView()));
    connect(mSimEngine, SIGNAL(closePopup()), this, SLOT(closeImportPopup()));
    CNT_EXIT
      
}

void CntImportsView::deactivate()
{
}

   
CntImportsView::~CntImportsView()
{
    CNT_ENTRY
    mView->deleteLater();
    delete mModel;
    delete mSimEngine;
    CNT_EXIT
}

/*!
Activates a previous view
*/
void CntImportsView::showPreviousView()
{
    CNT_ENTRY
    CntViewParameters args;
    mViewManager->back(args);
    CNT_EXIT
}

void CntImportsView::closeImportPopup()
{
    CNT_ENTRY
    if (mImportSimPopup != NULL)
    {
        mImportSimPopup->close();
        mImportSimPopup = NULL;     // Dilaog is deleted on close()
    }
    CNT_EXIT
}

/*
Activates a default view
*/
void CntImportsView::activate(const CntViewParameters aArgs)
{
    CNT_ENTRY
   Q_UNUSED(aArgs);
    //back button
          
    if (mView->navigationAction() != mSoftkey)
    {
        mView->setNavigationAction(mSoftkey);
    }
            
    mViewManager = &mEngine->viewManager();  
    QContactManager *ContactSymbianManager = &mEngine->contactManager(SYMBIAN_BACKEND);
 
    mModel = new QStandardItemModel();

    mSimEngine->init(*mEngine);
    
    mListView = static_cast<HbListView*>(mDocumentLoader.findWidget(QString("listView")));
    
    HbFrameBackground frame;
    frame.setFrameGraphicsName("qtg_fr_list_normal");
    frame.setFrameType(HbFrameDrawer::NinePieces);
    mListView->itemPrototypes().first()->setDefaultFrame(frame);
    mListView->listItemPrototype()->setStretchingStyle(HbListViewItem::StretchLandscape);
    mListView->setUniformItemSizes(true);
    
    connect(mListView, SIGNAL(activated (const QModelIndex&)),
                                      this,  SLOT(onListViewActivated(const QModelIndex&)));
       
    
    QStandardItem *importItem = new QStandardItem();
    QStringList simList;
        
    mSimEngine->checkEntries(simList, *importItem);
    
    importItem->setData(simList, Qt::DisplayRole);
    importItem->setData(HbIcon("qtg_large_sim"), Qt::DecorationRole);   
     
    mModel->insertRow(0,importItem);
        
    mListView->setModel(mModel);
    mListView->setSelectionMode(HbAbstractItemView::NoSelection);
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(userCancelsImport()));
    CNT_EXIT
}


void CntImportsView::onListViewActivated(const QModelIndex &index)
{
    CNT_ENTRY
    int row = index.row();
    if ( row == 0 ) // row 0 has "Imports from SIM" 
    {
       if (!mSimEngine->startSimImport())
       {
           //show error note
           mSimEngine->simInfoErrorMessage(KErrAccessDenied);
       }
       else
       {
           //start popup and progress
           showWaitNote();
       }
    }        
    CNT_EXIT
    
 }

void CntImportsView::showWaitNote()
{
    CNT_ENTRY
    mImportSimPopup = new HbDialog();    
    mImportSimPopup->setDismissPolicy(HbPopup::NoDismiss);
    mImportSimPopup->setTimeout(HbPopup::NoTimeout);
    mImportSimPopup->setBackgroundFaded(true);
    mImportSimPopup->setAttribute(Qt::WA_DeleteOnClose, true);
    
    HbLabel *headingLabel = new HbLabel(mImportSimPopup);
    headingLabel->setPlainText(hbTrId("txt_phob_title_import_contacts")); 
    mImportSimPopup->setHeadingWidget(headingLabel);
    
    QGraphicsLinearLayout *containerLayout = new QGraphicsLinearLayout(Qt::Vertical);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(10);
    
    HbLabel *icon = new HbLabel(mImportSimPopup);
    icon->setIcon(HbIcon("qtg_large_sim"));  
    
    HbLabel *simText= new HbLabel(mImportSimPopup);
    simText->setPlainText(hbTrId("txt_phob_info_importing_contacts_from_sim")); 
    simText->setTextWrapping(Hb::TextWordWrap);
    simText->setElideMode(Qt::ElideNone);
    
    HbProgressBar *progressBar = new HbProgressBar(mImportSimPopup);
    progressBar->setRange(0,0); 
    
    HbPushButton *stopButton = new HbPushButton(mImportSimPopup);
    stopButton->setText(hbTrId("txt_phob_button_cancel"));
    connect(stopButton, SIGNAL(clicked()), mSimEngine, SLOT(stopSimImport()));
    
    QGraphicsLinearLayout *containerLayout1 = new QGraphicsLinearLayout(Qt::Horizontal);
    containerLayout1->addItem(icon);
    containerLayout1->addItem(simText);
    
    QGraphicsWidget *containerWidget = new QGraphicsWidget;    
    containerLayout->addItem(containerLayout1);
    containerLayout->addItem(progressBar);
    containerLayout->addItem(stopButton);  
    containerWidget->setLayout(containerLayout);
    mImportSimPopup->setContentWidget(containerWidget);
    mImportSimPopup->open();
    CNT_EXIT
}

void CntImportsView::setPreferredDetails( QContact *aContact )
{
    CNT_ENTRY
    mSimEngine->setPreferredDetails(*aContact);
    CNT_EXIT
}


void CntImportsView::userCancelsImport()
{   
    CNT_ENTRY
    QString results;
    
    if(mSimEngine->userCancelsImport(results))
    {
        HbDeviceNotificationDialog::notification(QString(),results);
    }
    CNT_EXIT
}


void CntImportsView::setListBoxItemText(QString& aPrimary, QString& aSecondary)
{
    CNT_ENTRY
    QList<QStandardItem*> importItems = mModel->takeRow(0);
    QStandardItem* importItem = NULL;
    if (importItems.count() > 0) {
        importItem = importItems.at(0);
    }
    
    if (importItem != NULL)
    {
        QStringList simList;
        
        simList << aPrimary;
        simList << aSecondary;
        
        importItem->setData(simList, Qt::DisplayRole);
        importItem->setData(HbIcon("qtg_large_sim"), Qt::DecorationRole);
        mModel->insertRow(0, importItem);
        mListView->reset();
    }
    CNT_EXIT
}

void CntImportsView::setListBoxItemEnabled(bool aEnabled)
{
    CNT_ENTRY
    QList<QStandardItem*> importItems = mModel->takeRow(0);
    QStandardItem* importItem = NULL;
    if ( !importItems.isEmpty() )
    {
       importItem = importItems.first();
    }
    
    if (importItem != NULL)
    {
       importItem->setEnabled(aEnabled);
    }
    CNT_EXIT
}

QContactManager *CntImportsView::contactSymbianManager()
{
    if (mSimEngine)
    {
        return mSimEngine->contactSymbianManager();
    }
    
    return NULL;
}

// EOF
