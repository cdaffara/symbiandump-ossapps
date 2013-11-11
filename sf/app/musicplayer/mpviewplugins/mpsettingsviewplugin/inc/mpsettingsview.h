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
* Description: Music Player Settings view.
*
*/

#ifndef MPSETTINGSVIEW_H
#define MPSETTINGSVIEW_H

#include "mpviewbase.h"


class HbAction;
class HbMainWindow;
class MpSettingsAudioEffectsWidget;

//class declaration
class MpSettingsView : public MpViewBase
{
    Q_OBJECT

public:
    explicit MpSettingsView();
    virtual ~MpSettingsView();
    
    void initializeView();
    void activateView();
    void deactivateView();

signals:
    void command( int aCommand );

public slots:
    void back();
        
private:
    HbMainWindow                 *mWindow;             // not own
    HbAction                     *mSoftKeyBack;        // own
    MpSettingsAudioEffectsWidget *mAudioEffectsWidget; // not own

    Q_DISABLE_COPY(MpSettingsView)
};


#endif //MPSETTINGSVIEW_H
