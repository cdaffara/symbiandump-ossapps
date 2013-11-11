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
* Description: Plugin interface for Music Player playback view.
*
*/

#ifndef MPSETTINGSVIEWPLUGIN_H
#define MPSETTINGSVIEWPLUGIN_H

#include <QObject>
#include <mpxviewpluginqt.h>

//forward declartions
class MpSettingsView;

//class declaration
class MpSettingsViewPlugin : public MpxViewPlugin
{
    Q_OBJECT

public:
    explicit MpSettingsViewPlugin();
    virtual ~MpSettingsViewPlugin();

    void createView();
    void destroyView();
    void activateView();
    void deactivateView();
    QGraphicsWidget *getView();
    
signals:
    void command( int command );

public slots:
    void orientationChange( Qt::Orientation orientation );
    void back();

private:
    enum State { NullView, Created, Initialized, Activated };
    MpSettingsView    *mView; //owned
    State             mState;
};

#endif  // MPSETTINGSVIEWPLUGIN_H

