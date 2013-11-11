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

#include <hbview.h>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hblabel.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtextedit.h>
#include <hbmessagebox.h>
#include <qgraphicslinearlayout.h>
#include "enginewrapper.h"
#include "settingsview.h"
#include "mainview.h"
#include "notifications.h"
#include "hbtoolbar.h"

// ---------------------------------------------------------------------------

MainView::MainView(HbMainWindow &mainWindow, EngineWrapper &engineWrapper):
    mMainWindow(mainWindow),
    mSettingsView(0),
    mEngineWrapper(engineWrapper)
{
}

// ---------------------------------------------------------------------------

MainView::~MainView()
{
}

// ---------------------------------------------------------------------------

void MainView::init(HbApplication &app)
{
    this->setTitle("Screen Grabber");
    createMenu(app);
    createInfoText();
    createToolbar();

}

void MainView::createMenu(HbApplication& app)
{
    
    HbMenu* menu = this->menu();
   
    if (menu != NULL) {
    
        actionSendToBg = menu->addAction("Send to background");
        connect(actionSendToBg, SIGNAL( triggered() ), this, SLOT( sendToBackground() ) );
        
        actionSettings = menu->addAction("Settings");
        connect(actionSettings, SIGNAL( triggered() ), this, SLOT( showSettings() ) );
    
        actionAbout = menu->addAction("About");
        connect(actionAbout, SIGNAL( triggered() ), this, SLOT( showAboutPopup() ) );
    
        actionExit = menu->addAction("Exit");
        connect(actionExit, SIGNAL(triggered()), this, SLOT( my_quit() ) );

		HbAction *mDefaultNavigationAction = new HbAction(Hb::QuitNaviAction, this);
		connect(mDefaultNavigationAction,SIGNAL(triggered()), this, SLOT(my_quit()));
		setNavigationAction(mDefaultNavigationAction);
        
    	//setNavigationAction(actionExit);
    }
}


void MainView::quitYesNoQuestionClosed(HbAction* action)
    {
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->primaryAction())
        {
        qApp->quit(); 
        }
    }

void MainView::my_quit()
    {
	HbMessageBox::question("Do you really want to exit the Screen Grabber application?", 
	    this, //receiver
	    SLOT(quitYesNoQuestionClosed(HbAction *)), //member
	    tr("Yes"), //primaryButtonText
	    tr("No"), //secondaryButtonText
	    new HbLabel("Exit Screen grabber confirm", this)
        );
	}

// ---------------------------------------------------------------------------

void MainView::createInfoText()
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);
    if (layout != NULL) {
        HbTextEdit* edit = new HbTextEdit(this);
        if(edit != NULL) {
            edit->setPlainText("Please use the settings screen to define the "
                               "capture mode and any other related settings "
                               "such as capture hotkey and saving format. "
                               "Using the PNG format for screen shots is "
                               "recommended, since it is compressed but "
                               "lossless. The captures can be managed with "
                               "the Media Gallery application.\n\nYou may "
                               "start taking screenshots or capturing video "
                               "now. Please press the Applications key to go back "
                               "to the Application menu or send this application "
                               "to the background via the Options menu. To be "
                               "able to take screenshots or capture video, this "
                               "application needs to stay running in the "
                               "background.");
            edit->setReadOnly(true);
            //TODO edit->setCursorHidden(true);
            //TODO edit->setAttribute(Qt::WA_InputMethodEnabled, false);
            layout->addItem(edit);
            layout->setContentsMargins(5, 5, 5, 5);
            this->setLayout(layout);
        }
        
    }
    
}

// ---------------------------------------------------------------------------

void MainView::showSettings()
{
    mEngineWrapper.EnableRcpOfFoc(ETrue);
	mEngineWrapper.LoadSettings();	
	mSettingsView->loadSettings();
	mMainWindow.setCurrentView(mSettingsView);
}

// ---------------------------------------------------------------------------

void MainView::createToolbar()
{
    HbToolBar *toolbar = new HbToolBar(this); // main view takes the ownership
    toolbar->addAction(actionSettings);
    toolbar->addAction(actionSendToBg);
    setToolBar(toolbar);
}

// ---------------------------------------------------------------------------

void MainView::showAboutPopup()
{
    Notifications::about();
}

// ---------------------------------------------------------------------------

void MainView::sendToBackground()
    {
    mEngineWrapper.sendUIToBackground();
    }


