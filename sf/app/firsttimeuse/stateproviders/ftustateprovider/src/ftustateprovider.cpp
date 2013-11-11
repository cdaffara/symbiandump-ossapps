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
* Description:  Implementation of the FTU state provider
*
*/


#include "ftustateprovider.h"
#include "ftustateprovider_p.h"
#include "ftuwizardmenustate.h"
#include "ftuwizardloaderstate.h"
#include "ftuwizardactivatedstate.h"
#include "ftustateprovider_global.h"
#include <QState>
#include <QDebug>



// constants
const char wizardMenuStateUri [] = "ftu.nokia.com/state/wizardmenustate";
const char wizardLoaderStateUri [] = "ftu.nokia.com/state/wizardloaderstate";
const char wizardActivatedStateUri [] = "ftu.nokia.com/state/wizardactivatedstate";

// ---------------------------------------------------------------------------
// FtuStateProviderPrivate::FtuStateProviderPrivate
// ---------------------------------------------------------------------------
//
FtuStateProviderPrivate::FtuStateProviderPrivate(QObject *parent)
:QObject(parent)
{
}

// ---------------------------------------------------------------------------
// FtuStateProviderPrivate::createState
// ---------------------------------------------------------------------------
//
QState* FtuStateProviderPrivate::createState(const QString& uri)
{
    if(uri == wizardMenuStateUri) {
        return new FtuWizardMenuState();
    } 
    else if(uri == wizardLoaderStateUri){
        return new FtuWizardLoaderState();
    }
    else if(uri == wizardActivatedStateUri){
        return new FtuWizardActivatedState();
    }
    else {
        QDEBUG("FTU: No state found for uri: " << uri;)
        return NULL;
    }
}
// ---------------------------------------------------------------------------
// FtuStateProvider::FtuStateProvider
// ---------------------------------------------------------------------------
//
FtuStateProvider::FtuStateProvider(QObject *parent)
:QObject(parent), mPrivate(new FtuStateProviderPrivate())
{
}

// ---------------------------------------------------------------------------
// FtuStateProviderPrivate::createState
// ---------------------------------------------------------------------------
//
QState* FtuStateProvider::createState(const QString& uri)
{
    return mPrivate->createState(uri);
}
