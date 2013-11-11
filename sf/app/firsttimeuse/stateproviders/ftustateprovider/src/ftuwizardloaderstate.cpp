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


#include "ftuwizardloaderstate.h"
#include "ftuwizardprovider.h"
#include "ftucontentservice.h"
#include <ftuwizardfactory.h>
#include <ftuwizard.h>
#include "ftutest_global.h"
#include "ftustatecenrephandler.h"
#include <QDebug>
#include <QTime>
// ---------------------------------------------------------------------------
// FtuWizardLoaderState::FtuWizardLoaderState
// ---------------------------------------------------------------------------
//
FtuWizardLoaderState::FtuWizardLoaderState(QState *parent) :
    QState(parent),
	mWizardProvider(NULL)
{
    mWizardProvider = new FtuWizardProvider();
}

// ---------------------------------------------------------------------------
// FtuWizardLoaderState::~FtuWizardLoaderState
// ---------------------------------------------------------------------------
//
FtuWizardLoaderState::~FtuWizardLoaderState()
{
    delete mWizardProvider;
}
// ---------------------------------------------------------------------------
// FtuWizardLoaderState::addWizard
// ---------------------------------------------------------------------------
//
void FtuWizardLoaderState::addWizard(FtuWizard* wizard, bool success)
{
    if(NULL!=wizard) {
        QDEBUG("Wizard " << wizard->objectName() << " initialized with " << success;)
     }
    else {
       qFatal("void FtuWizardLoaderState::addWizard(FtuWizard* wizard, bool success): Null pointer argument wizard");
     }

    if(wizard && success)
    {   
        content()->appendWizard(wizard);
        
        // Disconnect the signal, for it is no longer needed.
        disconnect(wizard, SIGNAL(wizardInitialized(FtuWizard*, bool)),
                   this, SLOT(addWizard(FtuWizard*, bool)));
    }
    QDEBUG("FtuWizardLoaderState.cpp"<<__FUNCTION__<<"~~~~~~~"<<QTime::currentTime().toString("hh:mm:ss.zzz");)
}

// ---------------------------------------------------------------------------
// FtuWizardLoaderState::onEntry
// ---------------------------------------------------------------------------
//
void FtuWizardLoaderState::onEntry(QEvent *event)
{
    QState::onEntry(event);
    
    QList<FtuWizardFactory*> pluginList;   
    mWizardProvider->loadFTUPlugins(pluginList);
    int size = pluginList.size();
    
    if(size > 0)
    {     
        for(int i = 0; i < size; i++)
        {
        	FtuWizard* wizard = pluginList[i]->createWizard();
        	if(wizard)
        	{                
                
                connect(wizard, SIGNAL(wizardInitialized(FtuWizard*, bool)),
                        this, SLOT(addWizard(FtuWizard*, bool)));
                wizard->initializeWizard(KCrUidFtuStateProvider, KBaseIndex + i);
        	}
        }	
    }
}

// ---------------------------------------------------------------------------
// FtuWizardLoaderState::onExit
// ---------------------------------------------------------------------------
//
void FtuWizardLoaderState::onExit(QEvent *event)
{
    QState::onExit(event);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
FtuContentService *FtuWizardLoaderState::content() const
{
    return property(FTU_CONTENT_SERVICE_KEY).value<FtuContentService*>();
}

