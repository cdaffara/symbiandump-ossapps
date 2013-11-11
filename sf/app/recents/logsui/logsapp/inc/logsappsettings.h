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

#ifndef LOGSAPPSETTINGS_H
#define LOGSAPPSETTINGS_H

class LogsAppSettings
{
public:
    LogsAppSettings(int &argc, char *argv[]);
    virtual ~LogsAppSettings();
    
    bool logsFeaturePreloadingEnabled();
    bool logsFeatureFakeExitEnabled();   
    
private:
    
    static bool isBootupStart(int &argc, char *argv[]);
    
private:
    
    bool mFeaturePreloadedEnabled;
    int mFeatureFakeExitEnabledVal;
    
    friend class UT_LogsApplication;
    friend class UT_LogsViewManager;
    friend class UT_LogsAppSettings;
};

#endif // LOGSAPPSETTINGS_H
