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
 * Description: settings
 *
 */

#include "settings.h"

Settings::Settings() :
    timeout(0), memAllocFailureSim(false), outputFormat(QString::null)
    {
    }

Settings::Settings(int timeoutVal, bool memAllocFailureSimVal,
        QString outputFormatVal) :
    timeout(timeoutVal), memAllocFailureSim(memAllocFailureSimVal),
            outputFormat(outputFormatVal)
    {

    }

Settings::~Settings()
    {
    }

bool Settings::isEqual(const Settings& settings)
    {
    return timeout == settings.timeout
            && outputFormat.compare(settings.outputFormat) == 0
            && !memAllocFailureSim == !settings.memAllocFailureSim;
    }
