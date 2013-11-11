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
 * Description:  Implementation of the SEARCH state provider
 *
 */

#include "searchstateprovider.h"
#include "searchprogressivestate.h"
#include "searchinitstate.h"
#include "searchsettingsstate.h"
#include "searchonlinestate.h"

#include <qstate.h>
// states
const char initStateFileUri[] = "search.nokia.com/state/initstate";

const char wizardProgressiveStateUri[] =
        "search.nokia.com/state/wizardprogressivestate";
const char wizardSettingStateUri[] =
        "search.nokia.com/state/wizardsettingstate";
const char wizardOnlineStateUri[] =
        "search.nokia.com/state/wizardonlinestate";

// ---------------------------------------------------------------------------
// searchStateProvider::searchStateProvider()
// ---------------------------------------------------------------------------
//
SearchStateProvider::SearchStateProvider(QObject *parent) :
    QObject(parent)
    {
    }
// ---------------------------------------------------------------------------
// SearchStateProvider::createState(const StateToken& aToken)
// ---------------------------------------------------------------------------
//
QState* SearchStateProvider::createState(const QString& uri)
    {
    if (uri == wizardProgressiveStateUri)
        {
        return new SearchProgressiveState();
        }
    else if (uri == wizardSettingStateUri)
        {
        return new SearchSettingsState();
        }
    else if (uri == initStateFileUri)
        {
        return new SearchInitState();
        }
    else if (uri == wizardOnlineStateUri)
        {
        return new SearchOnlineState();
        }
    return NULL;
    }
