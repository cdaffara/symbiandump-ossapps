/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Qt Mpx blank view plugin.
*
*/



#include <QtGui>
#include <QTimer>

#include <hbview.h>
#include <hblabel.h>
//#include <hbgridlayout.h>
#include <hbtoolbutton.h>
#include <hbaction.h>
#include <qgraphicsgridlayout.h>
#include <hbpushbutton.h>
#include <glxstatemanager.h>
#include <qplugin2.h>
#include "glxexternalviewplugin.h"
#include <glxplugincommandid.hrh>

GlxExternalViewPlugin::GlxExternalViewPlugin() : iView( NULL ), 
                                    collectionButton( NULL ),
                                    imageButton( NULL ),
                                    iLayout ( NULL )
{
	qDebug("GlxExternalViewPlugin::GlxExternalViewPlugin");
    QObject::connect(&mStateMgr, SIGNAL(externalCommand(int )), this, SLOT(externalCommand(int )) );
}

GlxExternalViewPlugin::~GlxExternalViewPlugin()
{
	qDebug("GlxExternalViewPlugin::~GlxExternalViewPlugin"); 
    if( iView ){
    delete iView;
    }
}

QGraphicsWidget* GlxExternalViewPlugin::getView()
{
	qDebug("GlxExternalViewPlugin::getView");
    if( !iView ){
        iView = new HbView();
    }
    return iView;
}

void GlxExternalViewPlugin::destroyView()
{
	qDebug("GlxExternalViewPlugin::destroyView");
}

void GlxExternalViewPlugin::activateView()
{
	qDebug("GlxExternalViewPlugin::activateView");
	mStateMgr.launchFromExternal(); 

}

void GlxExternalViewPlugin::deactivateView()
{
	qDebug("GlxExternalViewPlugin::deactivateView");
    mStateMgr.cleanupExternal();
}

void GlxExternalViewPlugin::externalCommand(int cmdId)
{
	qDebug("GlxExternalViewPlugin::externalCommand");
    emit command( cmdId );
}

void GlxExternalViewPlugin::orientationChange( Qt::Orientation /*orientation*/ )
{
	qDebug("GlxExternalViewPlugin::orientationChange");

}


Q_EXPORT_PLUGIN3( GlxExternalViewPlugin, GlxExternalViewPlugin );
