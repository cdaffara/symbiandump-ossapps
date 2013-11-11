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
* Description: MpSettingsManager stub for testing MpMtpInfoLink.
*
*/

#ifndef MPSETTINGSMANAGER_H
#define MPSETTINGSMANAGER_H

#include<QObject>
#include <QtGlobal>

class  MpSettingsManager: public QObject
{
    
Q_OBJECT

private:
    explicit MpSettingsManager();

public:
    virtual ~MpSettingsManager();
    static MpSettingsManager *instance();
    static bool shuffle();
    static bool repeat();
    static void setShuffle(bool shuffle);
    static void setRepeat(bool repeat);

public:
    bool mShuffle;
    bool mRepeat;

private:
    Q_DISABLE_COPY(MpSettingsManager)
};

#endif    // MPSETTINGSMANAGER_H
