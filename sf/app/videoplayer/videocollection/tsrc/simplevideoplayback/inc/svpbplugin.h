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
* Description:  Simple Video Playback MXP view plugin
*
*/

#ifndef SVPBPLUGIN_H
#define SVPBPLUGIN_H

#include <mpxviewpluginqt.h>

class SvpbEngine;
class SvpbView;

class SvpbPlugin : public MpxViewPlugin
{
Q_OBJECT

public:
    SvpbPlugin();
    virtual ~SvpbPlugin();

public: // from MpxViewPlugin
    void createView();
    void destroyView();
    void activateView();
    void deactivateView();
    QGraphicsWidget* getView();

public slots: // from MpxViewPlugin
    void orientationChange(Qt::Orientation orientation);
    void back();

private:
    SvpbView *mView;
    SvpbEngine *mEngine;
};

#endif //SVPBPLUGIN_H

