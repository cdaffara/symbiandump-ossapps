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


#ifndef SETTINGS_H_
#define SETTINGS_H_
#include <QString>
class Settings
    {
public:
    Settings();
    Settings(int timeout, bool memAllocFailureSim, QString outputFormat);
    ~Settings();
    bool isEqual(const Settings& settings);
public:
    // Data
    int timeout;
    bool memAllocFailureSim;
    QString outputFormat;
    };

#endif /* SETTINGS_H_ */
