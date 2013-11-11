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
* Description: Plugin interface for Music Player details view.
*
*/

#ifndef MPDETAILSVIEWPLUGIN_H
#define MPDETAILSVIEWPLUGIN_H

#include <QObject>
#include <mpxviewpluginqt.h>

//forward declartions
class MpDetailsView;

//class declaration
/*!
 MpDetailsViewPlugin is one of the four view plugins in Music Player.
 Each view plugin is instantiated on music player startup, and
 destructed on music player exit.

 The actual view is a member variable of ViewPlugin.
 View's visibility is controlled by using its activateView() and
 deactivateView() methods.
*/
class MpDetailsViewPlugin : public MpxViewPlugin
{
    Q_OBJECT

public:
    explicit MpDetailsViewPlugin();
    virtual ~MpDetailsViewPlugin();

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
    MpDetailsView    *mView; //owned
    State             mState;
};

#endif  // MPPLAYBACKVIEWPLUGIN_H

