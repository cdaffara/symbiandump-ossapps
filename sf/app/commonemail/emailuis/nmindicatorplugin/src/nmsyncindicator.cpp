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

#include "nmsyncindicator.h"

const char NmSyncIndicator::IndicatorType[] = "com.nokia.nmail.indicatorplugin.sync/1.0";

NmSyncIndicator::NmSyncIndicator() :
HbIndicatorInterface(NmSyncIndicator::IndicatorType, ProgressCategory, NoInteraction)
{
}

NmSyncIndicator::~NmSyncIndicator()
{
}

QVariant NmSyncIndicator::indicatorData(int role) const
{
    if (role == DecorationNameRole) {
        // Must return non-empty to make the indicator visible
        return " ";
    }
    return QVariant();
}
