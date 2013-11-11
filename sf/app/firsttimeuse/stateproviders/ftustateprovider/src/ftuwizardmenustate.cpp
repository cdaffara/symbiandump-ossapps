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
* Description:  Implementation of the wizard menu state.
*
*/


#include "ftuwizardmenustate.h"
#include "ftucontentservice.h"
#include <ftuwizard.h>
#include "ftustateprovider_global.h"
#include <hbmainwindow.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbinstance.h>
#include <hblistview.h>
#include <hbdocumentloader.h>
#include <HbTranslator>
#include <HbGroupBox>
#include <HbListViewItem>

#include <QStandardItemModel>
#include <QDate>
#include <QTime>
#include <xqsettingsmanager.h>

#include "ftustatecenrephandler.h"

const int progressCompelete = 100;
const char* emptyLine = " ";

const char *FTUSTATEPROVIDER_DOCML = ":/xml/ftustateprovider.docml";
const char *TOC_VIEW = "tocView";
const char *TOC_INFOTEXT_LABEL = "tocInfoTextLabel";
const char *TOC_LIST_VIEW = "tocListView";


// ---------------------------------------------------------------------------
// FtuWizardMenuState::FtuWizardMenuState
// ---------------------------------------------------------------------------
//
FtuWizardMenuState::FtuWizardMenuState(QState *parent) :
    QState(parent),
    mMainWindow(NULL),
    mTocView(NULL),
    mInfoText(NULL),
    mListView(NULL),
	mDocumentLoader(NULL),
    mModel(NULL),
    mTranslator(NULL)
{
    mMainWindow = hbInstance->allMainWindows().at(0);
    mModel = new QStandardItemModel(this);
    mTranslator = new HbTranslator("/resource/qt/translations/","firsttimesetup");  
    mDocumentLoader = new HbDocumentLoader();
	bool ok = false;
	mDocumentLoader->load(FTUSTATEPROVIDER_DOCML, &ok);
	QGraphicsWidget *widget = mDocumentLoader->findWidget(TOC_VIEW);
	Q_ASSERT_X(ok && (widget != 0), "ftustateprovider", "invalid DocML file");
	mTocView = qobject_cast<HbView*>(widget);

	mTocView->setTitle(hbTrId("txt_long_caption_FTU_widget"));

	mMainWindow->addView(mTocView);
    // Set as initial view.
    mMainWindow->setCurrentView(mTocView);
    mCenrepHandler = new FtuStateCenrepHandler(NULL);
    int registeredPlugins = mCenrepHandler->registeredPluginCount();
    for(int counter = 0; counter < registeredPlugins; counter ++){
        mCompletedWizardList << mCenrepHandler->getPluginInfo(counter);
    }
    connect(mMainWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(changeListStrechingStyle(Qt::Orientation)));
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::~FtuWizardMenuState
// ---------------------------------------------------------------------------
//
FtuWizardMenuState::~FtuWizardMenuState()
{
	if(mModel)
	{
		delete mModel;
	}
	delete mDocumentLoader;
	delete mCenrepHandler;
	if(mTranslator){
        delete mTranslator;
        mTranslator = NULL;
	}
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::onEntry
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::onEntry(QEvent *event)
{    
    QDEBUG("FtuWizardMenuState::onEntry";)
    QState::onEntry(event);

    if(!mInfoText)
    {    
        createInfoText();
    }    
    if(!mListView)
    {
        createTocList();
    }

    // If this is not the current view, we're getting back from plugin view
    if(mMainWindow->currentView() != mTocView)
    {
        mMainWindow->setCurrentView(mTocView, true);
    }

    mMainWindow->show();
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::onExit
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::onExit(QEvent *event)
{
    //remove the options menu so that plugin can perform the cleanup
    mMainWindow->currentView()->takeMenu();
    QState::onExit(event);
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::content
// ---------------------------------------------------------------------------
//
FtuContentService *FtuWizardMenuState::content() const
{
    return property(FTU_CONTENT_SERVICE_KEY).value<FtuContentService*>();
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::updatedAsString
// ---------------------------------------------------------------------------
//
QString FtuWizardMenuState::updatedAsString(const QDate& date) const
{
    QString status;

    if(date == QDate().currentDate() )
    { 	
        status.append(qtTrId("txt_ftu_list_update_today"));
    }
    else
    {
        status.append(qtTrId("txt_ftu_list_update_date").arg(date.toString()));
    }
    return status;
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::addWizardToListModel
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::addWizardToListModel(int aIndex)
{    

    QDEBUG("FtuWizardMenuState::addWizardToListModel idx : " << aIndex;)
    FtuContentService* ftuContentService = content();

    FtuWizard* addedWizard = ftuContentService->wizard(aIndex);
    if(addedWizard)
    {    
        // Connect for progress updates.
        connect(addedWizard, SIGNAL(progressUpdated(FtuWizard *, bool, int)), 
                this, SLOT(updateProgress(FtuWizard *, bool, int)));

        connect(addedWizard, SIGNAL(wizardSettingsChanged(FtuWizard*)), 
                        this, SLOT(changeWizardSettings(FtuWizard*)));
        
        
        const FtuWizardSetting& settings = addedWizard->wizardSettings();
        QStandardItem* newItem = new QStandardItem();
        QList<QVariant> iconList;
        HbIcon icon (settings.mTocDefaultIcon.filePath() );
        iconList.append(icon);
        HbIcon rightIcon(QString(qtTrId("qtg_small_tick")));
        
        QStringList data;
        data << settings.mTocLabel;
        data << settings.mTocSecondaryLabel;
        QDate date = addedWizard->wizardCompletedDate();
        if(!date.isNull())
        {
            //Plugin has already completed, Append tick mark on right side
            iconList.append(rightIcon);
        }
        

        newItem->setData(iconList, Qt::DecorationRole);
        newItem->setData(QVariant(data), Qt::DisplayRole);

        mModel->appendRow(newItem);        
    }
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::createTocList
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::createTocList()
{
    if (!mListView)
    {
		mListView = qobject_cast<HbListView *>(mDocumentLoader->findWidget(
															TOC_LIST_VIEW));
	    connect(mListView,
                SIGNAL(activated(const QModelIndex)),
                this,
                SLOT(activateWizard(const QModelIndex)));

        mListView->setModel(mModel);
        QDEBUG("FtuWizardMenuState.cpp Model is set"<<__FUNCTION__<<"~~~~~~~"<<QTime::currentTime().toString("hh:mm:ss.zzz");)
    }
}

// ---------------------------------------------------------------------------
// FtWizardMenuState::createInfoText
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::createInfoText()
{
    mInfoText = qobject_cast<HbGroupBox *>(mDocumentLoader->findWidget(TOC_INFOTEXT_LABEL));                               
    mInfoText->setHeading(hbTrId("txt_ftu_subhead_select_setting_you_want_to_edit"));
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::activateWizard
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::activateWizard(const QModelIndex index)
{
    //remove the main menu 
    mMainWindow->currentView()->takeMenu();
    // Set the active wizard index
    content()->setActiveWizard(index.row());
    FtuWizard * wizard=content()->wizard(index.row());
    
    // signal to activated state
    emit wizardSelected();
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::updateProgress
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::updateProgress(FtuWizard *caller, bool show, 
                                           int progress)
{
    Q_UNUSED(show)
    QList<FtuWizard*> wizards = content()->wizards();
    int index = -1;
    
    // Get the index of the wizard
    for(int i = 0 ; i < wizards.size() ; ++i)
    {
        if(caller == wizards[i])
        {
            QDEBUG("::updateProgress wizard found at: " << i;)
            index = i;
        }
    }
    if(index != -1)
    {  
        QStringList data;
        data << wizards[index]->wizardSettings().mTocLabel;
        QList<QVariant> iconList;
        HbIcon icon (wizards[index]->wizardSettings().mTocDefaultIcon.filePath());
        iconList.append(icon);
        HbIcon rightIcon(QString("qtg_small_tick"));
        
        if(progress < progressCompelete)
        {
            data << wizards[index]->wizardSettings().mTocSecondaryLabel;
        }
        else
        {   
            data << wizards[index]->wizardSettings().mTocSecondaryLabel;
            //Plugin has completed 100%, Append tick mark on right side
            iconList.append(rightIcon);

            XQSettingsManager settingsManager;
            
            if(false == mCompletedWizardList[index])
            {
            mCompletedWizardList[index] = true; 
            mCenrepHandler->updatePluginInfo(index);
            }

        }
        mModel->item(index)->setData(QVariant(data), Qt::DisplayRole);
        mModel->item(index)->setData(iconList, Qt::DecorationRole);
    }
}

// ---------------------------------------------------------------------------
// FtuWizardMenuState::changeWizardSettings
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::changeWizardSettings(FtuWizard* caller)
    {
    QList<FtuWizard*> wizards = content()->wizards();
        int index = -1;
        
        // Get the index of the wizard
        for(int i = 0 ; i < wizards.size() ; ++i)
        {
            if(caller == wizards[i])
            {
                QDEBUG("::changeWizardSettings wizard found at: " << i;)
                index = i;
            }
        }
        if(index != -1)
        {

            QStringList data;
            data << wizards[index]->wizardSettings().mTocLabel;
            data << wizards[index]->wizardSettings().mTocSecondaryLabel;
            QList<QVariant> iconList;
            HbIcon icon (wizards[index]->wizardSettings().mTocDefaultIcon.filePath());
            iconList.append(icon);
            HbIcon rightIcon(QString("qtg_small_tick")); 
            if(true == mCompletedWizardList[index])
                {
                iconList.append(rightIcon);
                }
            mModel->item(index)->setData(QVariant(data), Qt::DisplayRole);
            mModel->item(index)->setData(iconList, Qt::DecorationRole);
        }
    }

// ---------------------------------------------------------------------------
// FtuWizardMenuState::changeListStrechingStyle
// ---------------------------------------------------------------------------
//
void FtuWizardMenuState::changeListStrechingStyle(Qt::Orientation orientation)
{
    HbListViewItem *prototypeItem = mListView->listItemPrototype();
    if (orientation == Qt::Vertical) {
        // change the streching style of list to no strech
        prototypeItem->setStretchingStyle(HbListViewItem::NoStretching );
        
    } else if (orientation == Qt::Horizontal) {
        // change the streching style of list to landscape strech
        prototypeItem->setStretchingStyle(HbListViewItem::StretchLandscape);
    }
}

