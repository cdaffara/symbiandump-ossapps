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
* Description:  Implements the wizard activation and interactions.
*
*/


#include "ftuwizardactivatedstate.h"
#include "fturuntimeservices_global.h"
#include "ftucontentservice.h"
#include "ftustateprovider_global.h"
#include <ftuwizard.h>

#include <hbmainwindow.h>
#include <hbdocumentloader.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbstackedwidget.h>
#include <hbicon.h>
#include <hbinstance.h>
#include <hbaction.h>
#include <hbgridview.h>
#include <QStandardItemModel>
#include <QDebug>
#include <QGraphicsWidget>
#include <QDir>
#include <hbmenu.h>
#include <HbTranslator>


const char *FTUSTATEPROVIDER_DOCML2 = ":/xml/ftustateprovider.docml";
const char *WIZARD_VIEW = "wizardView";
const char *WIZARD_INFOTEXT_LABEL = "wizardInfoTextLabel";
const char *WIZARD_STACKEDWIDGET = "wizardStackedWidget";
const char *WIZARD_GRID_VIEW = "wizardGridView";

/*
#define LOG_GEOM(txt, r) logGeometry(txt, r)

static void logGeometry(const char *txt, const QRectF& r)
{
    qDebug() << "ftu:" << txt << " x: " << r.topLeft().x() << " y: "  << r.topLeft().y() 
             << " w: " << r.width() << " h: " << r.height();
}
*/

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::FtuWizardActivatedState
// ---------------------------------------------------------------------------
//
FtuWizardActivatedState::FtuWizardActivatedState(QState *parent) :
    QState(parent),
    mMainWindow(NULL),
    mDocumentLoader(NULL),
    mPluginView(NULL),
    mActiveWizard(NULL),
    mPluginTitleLabel(NULL),
    mWizardStackedWidget(NULL),
    mBackAction(NULL),
    mPreviousView(NULL), 
    mCurrentView(NULL),
    mTranslator(NULL)
{
    mMainWindow = hbInstance->allMainWindows().at(0);
    mDocumentLoader = new HbDocumentLoader();
    mPluginView = new HbView();
	mTranslator = new HbTranslator("/resource/qt/translations/","firsttimesetup");
    bool ok = false;
	mDocumentLoader->load(FTUSTATEPROVIDER_DOCML2, &ok);
	QGraphicsWidget *widget = mDocumentLoader->findWidget(WIZARD_VIEW);
	Q_ASSERT_X(ok && (widget != 0), "ftustateprovider", "invalid DocML file");

    mPluginView->setWidget(widget);
    mMainWindow->addView(mPluginView);

    mPluginView->setTitle(hbTrId("txt_long_caption_FTU_widget"));

    mPluginTitleLabel = qobject_cast<HbLabel *>(mDocumentLoader->findWidget(WIZARD_INFOTEXT_LABEL)); 

    mWizardStackedWidget = qobject_cast<HbStackedWidget *>(mDocumentLoader->findWidget(WIZARD_STACKEDWIDGET));

    QString path = QDir::currentPath();
    
    mBackAction = new HbAction(Hb::BackNaviAction, this);
    
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::~FtuWizardActivatedState
// ---------------------------------------------------------------------------
//
FtuWizardActivatedState::~FtuWizardActivatedState()
{
    delete mDocumentLoader;

	if(mBackAction)
	{		
		delete mBackAction;
	}
	if(mTranslator){
        delete mTranslator;
        mTranslator = NULL;
    }
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::onEntry
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::onEntry(QEvent *event)
{
    QDEBUG("ftu:FtuWizardActivatedState::onEntry";)
    QState::onEntry(event);    

    int activeIndex = content()->activeWizard();
    mActiveWizard = content()->wizard(activeIndex);
    
    mPluginView->setNavigationAction(mBackAction);
    connect(mBackAction, SIGNAL(triggered()),
            this, SLOT(handleBackEvent()));

    setActiveWizardConnections();

    mActiveWizard->activateWizard();
    
    if (mActiveWizard->wizardSettings().mNoViews)
    {
        emit backEventTriggered();
    }
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::onExit
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::onExit(QEvent *event)
{
    QState::onExit(event);
    mMainWindow->currentView()->takeMenu();
    disconnect(mBackAction, SIGNAL(triggered()),
                   this, SLOT(handleBackEvent()));
    
    
    disconnect(mActiveWizard, SIGNAL(wizardDeactivated(FtuWizard *)),
               this, SLOT(handleGotoToC()));
    
    disconnect(mActiveWizard, SIGNAL(viewChanged(FtuWizard *, 
                                                 QGraphicsWidget* )),
               this, SLOT(changeWizardView(FtuWizard*, QGraphicsWidget*)));
    

    disconnect(mActiveWizard, SIGNAL(infoTextUpdated(FtuWizard *, QString )),
            this, SLOT(updateInfoText(FtuWizard *, QString)));
    
    //disconnect from the existing wizard's updateMainMenu signal
    disconnect(mActiveWizard,SIGNAL(updateMainMenu(FtuWizard *, HbMenu * )),
            this,SLOT(updateMainMenu(FtuWizard *, HbMenu * )));
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::setActiveWizardConnections
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::setActiveWizardConnections()
{
    // first disconnect possible old connections with active wizard
    disconnect(mActiveWizard, SIGNAL(wizardDeactivated(FtuWizard *)),
                   this, SLOT(handleGotoToC()));
    
    disconnect(mActiveWizard, SIGNAL(viewChanged(FtuWizard *, 
                                                 QGraphicsWidget* )),
               this, SLOT(changeWizardView(FtuWizard*, QGraphicsWidget*)));
    
    disconnect(mActiveWizard, SIGNAL(infoTextUpdated(FtuWizard *, QString )),
            this, SLOT(updateInfoText(FtuWizard *, QString)));
    
    //disconnect from the existing existing wizard's updateMainMenu signal
    disconnect(mActiveWizard,SIGNAL(updateMainMenu(FtuWizard *, HbMenu * )),
            this,SLOT(updateMainMenu(FtuWizard *, HbMenu * )));
    
    // then connect new ones to active wizard
    connect(mActiveWizard, SIGNAL(viewChanged(FtuWizard *, QGraphicsWidget* )),
            this, SLOT(changeWizardView(FtuWizard*, QGraphicsWidget*)));
    
    connect(mActiveWizard, SIGNAL(infoTextUpdated(FtuWizard *, QString )),
            this, SLOT(updateInfoText(FtuWizard *, QString)));
    
    // connect to the updateMainMenu wizard signal
    connect(mActiveWizard,SIGNAL(updateMainMenu(FtuWizard *, HbMenu * )),
            this,SLOT(updateMainMenu(FtuWizard *, HbMenu * )));
    
    connect(mActiveWizard, SIGNAL(wizardDeactivated(FtuWizard *)),
                   this, SLOT(handleGotoToC()));
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::content
// ---------------------------------------------------------------------------
//
FtuContentService *FtuWizardActivatedState::content() const
{
    return property(FTU_CONTENT_SERVICE_KEY).value<FtuContentService*>();
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::handleBackEvent
// ---------------------------------------------------------------------------
//

void FtuWizardActivatedState::handleBackEvent()
{
    if(!mActiveWizard->handleBackEvent())
    {
        emit backEventTriggered();
    }    
}

void FtuWizardActivatedState::handleGotoToC()
    {
    emit backEventTriggered();
    }

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::updateMainMenu
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::updateMainMenu(FtuWizard * caller, HbMenu * menu)
{
    //check if only current wizard has emmited this signal
    if(caller==mActiveWizard)
        {
            mMainWindow->currentView()->takeMenu();
            if(menu)
                mMainWindow->currentView()->setMenu(menu);
        }
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::changeWizardView
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::changeWizardView(FtuWizard *caller, 
                                            QGraphicsWidget* viewWidget)
{
    if(caller == mActiveWizard)        
    {
        if(viewWidget)
        {
            if (mWizardStackedWidget->indexOf(viewWidget) == -1)
            {
                // add wizard's widget to stacked widget if not yet there
                mWizardStackedWidget->addWidget(viewWidget);
            }
            // set wizard's widget as current widget
            mWizardStackedWidget->setCurrentWidget(viewWidget);
            
            QDEBUG("Ftu: switching view due plugin view change";)
            mainWindow()->setCurrentView(mPluginView, true);
            
            QList<FtuWizard*> wizards = content()->wizards();
            // get index of active wizard
            int index = wizards.indexOf(mActiveWizard);
            
            // temp solution to skip edge indexes - start
            if (index == 0) { index++; }
            if (index == wizards.count()-1) { index--; }
        }        
    }
}


// ---------------------------------------------------------------------------
// FtuWizardActivatedState::updateInfoText
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::updateInfoText(FtuWizard *caller, 
                                                QString text)
{
    if(caller == mActiveWizard)
    {
        mPluginTitleLabel->setPlainText(text);
    }    
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::mainWindow
// ---------------------------------------------------------------------------
//
HbMainWindow* FtuWizardActivatedState::mainWindow()
{
    return hbInstance->allMainWindows().at(0);
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::activateWizard
// ---------------------------------------------------------------------------
//
void FtuWizardActivatedState::activateWizard(const QModelIndex index)
{   
    // get index for selected wizard
    int wizardIndex = index.row();
    if (wizardIndex != -1)
    {
        // check if other wizard than current is activated
        if (mActiveWizard != content()->wizard(wizardIndex))
        {
            int prevWizardIndex = content()->activeWizard();
			mMainWindow->currentView()->takeMenu();
            
            // first deactivate current active wizard
		    if(mActiveWizard)
		    {
			    mActiveWizard->deactivateWizard();
		    }
            // set selected wizard active (precaution, as state does not change)
            content()->setActiveWizard(wizardIndex);
            // save active wizard
            mActiveWizard = content()->wizard(wizardIndex);
            // reset connections for new active wizard
            setActiveWizardConnections();
            // set new active wizard visible
            mActiveWizard->activateWizard();
            
            if (mActiveWizard->wizardSettings().mNoViews) 
            {
                mActiveWizard->deactivateWizard();
                content()->setActiveWizard(prevWizardIndex);
                mActiveWizard = content()->wizard(prevWizardIndex);
                setActiveWizardConnections();
                mActiveWizard->activateWizard();
            }
        }
    }
}

// ---------------------------------------------------------------------------
// FtuWizardActivatedState::calculateWizardGeometry
// ---------------------------------------------------------------------------
//
QRectF FtuWizardActivatedState::calculateWizardGeometry()
{
	QRectF widgetRect = mWizardStackedWidget->geometry();
	QRectF pluginRect = mPluginView->geometry();	
	pluginRect.setSize(QSizeF(widgetRect.width(),
	                                widgetRect.height()));
    return pluginRect;
}
