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
 * Description:  Search Setting state.
 *
 */

#include "searchsettingsstate.h"
#include "settingswidget.h"
#include <hbdocumentloader.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbstackedwidget.h>
#include <hbicon.h>
#include <hbinstance.h>
#include <hbaction.h>
#include <hbgridview.h>
#include <qstandarditemmodel.h>
#include <qgraphicswidget.h>
#include <qdir.h>
// ---------------------------------------------------------------------------
// SearchSettingsState::SearchSettingsState
// ---------------------------------------------------------------------------
//
SearchSettingsState::SearchSettingsState(QState *parent) :
    QState(parent), mDocumentLoader(NULL)
    {
    minitialCount = true;
    mWidget = new SettingsWidget();

    connect(mWidget, SIGNAL(settingsEvent(bool)), this,
            SLOT(handleBackEvent(bool)));
    
    connect(mWidget, SIGNAL(ISProvidersIcon(HbIcon, int)), this,
            SLOT(slotISProvidersIcon(HbIcon, int)));
    }
// ---------------------------------------------------------------------------
// SearchSettingsState::~SearchSettingsState
// ---------------------------------------------------------------------------
//
SearchSettingsState::~SearchSettingsState()
    {
    delete mDocumentLoader;
    delete mWidget;
    }
// ---------------------------------------------------------------------------
// SearchSettingsState::slotISProvidersIcon
// ---------------------------------------------------------------------------
//
void SearchSettingsState::slotISProvidersIcon(HbIcon icon, int id)
    {
    emit publishISProviderIcon(id, icon);
    }
// ---------------------------------------------------------------------------
// SearchSettingsState::onEntry
// ---------------------------------------------------------------------------
//
void SearchSettingsState::onEntry(QEvent *event)
    {
    QState::onEntry(event);
    emit settingslaunched();
    if (minitialCount)
        {
        mWidget->loadBaseSettings();
        mWidget->loadDeviceSettings();
        mWidget->loadIS();
        isInternetOn();
        minitialCount = false;
        }
    else
        {
        mWidget->launchSettingWidget();
        }    
    }
// ---------------------------------------------------------------------------
// SearchSettingsState::onExit
// ---------------------------------------------------------------------------
//
void SearchSettingsState::onExit(QEvent *event)
    {
    QState::onExit(event);
    }
// ---------------------------------------------------------------------------
// SearchSettingsState::handleBackEvent
// ---------------------------------------------------------------------------
//
void SearchSettingsState::handleBackEvent(bool aStatus)
    {
    if (mWidget)
        {
        isInternetOn();
        emit clickstatus(aStatus);
        }
    }
// ---------------------------------------------------------------------------
// SearchSettingsState::isInternetOn
// ---------------------------------------------------------------------------
//
void SearchSettingsState::isInternetOn()
    {
    if (mWidget->isInternetSearchOptionSelected())
        {
        emit switchToOnlineState();
        }
    else
        {
        emit switchToProState();
        }
    }
