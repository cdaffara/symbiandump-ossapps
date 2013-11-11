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

#include "cntkeygrabber.h"

#include <hbmainwindow.h>
#include <xqaiwrequest.h>
#include <xqaiwdecl.h>
#include <xqappmgr.h>
#include <QCoreApplication>
#include <QKeyEvent>
#include <xqserviceutil.h>
#include <xqkeycapture.h>
/*!
    Key press event handler for default actions when pressing either send or end key.
    End key closes the application and send key opens dialer. Default functionality
    can be overwritten to have view-specific functionality by installing an additional 
    event filter for the main window.
    
    See CntGroupActionsView for an example.
*/
CntKeyGrabber::CntKeyGrabber(HbMainWindow *window, QObject *parent) :
    QObject(parent),
    mMainWindow(window),
    mRequest(NULL),
    mKeyCapture(NULL)
{
    mMainWindow->installEventFilter(this);
    mKeyCapture = new XQKeyCapture();
}

/*!
    Destructor
*/
CntKeyGrabber::~CntKeyGrabber()
{
    delete mRequest;
    if ( mKeyCapture )
    {
        mKeyCapture->cancelCaptureKey(Qt::Key_Yes);
    }
    delete mKeyCapture;
    mKeyCapture = NULL;
}

/*!
    Listens to events coming to the QObject obj (in this case HbMainWindow instance) and
    grabs/handles the necessary ones.

    \param obj QObject to which the event was sent to
    \param event QEvent that happened (key press, mouse press etc...)
    \return bool true if the event was handled (and thus it wont be passed on anymore)
*/
bool CntKeyGrabber::eventFilter(QObject *obj, QEvent *event)
{
    if ( event->type() == QEvent::ApplicationActivate )
    {
        mKeyCapture->captureKey( Qt::Key_Yes );
    }
    else if ( event->type() == QEvent::ApplicationDeactivate )
    {
        mKeyCapture->cancelCaptureKey( Qt::Key_Yes );
    }
    else if (event->type() == QEvent::KeyPress && obj == mMainWindow)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_No )
        {
            qApp->exit();
            
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Yes)
        {
            XQApplicationManager appManager;
            
            if (mRequest)
            {
                delete mRequest;
                mRequest = NULL;
            }
            
            mRequest = appManager.create(XQI_LOGS_VIEW, XQOP_LOGS_SHOW, false);
            
            if (mRequest)
            {
                QList<QVariant> args;
                QVariantMap map;
                map.insert(XQLOGS_VIEW_INDEX, QVariant(int(XQService::LogsViewAll)));
                map.insert(XQLOGS_SHOW_DIALPAD, QVariant(true));
                map.insert(XQLOGS_DIALPAD_TEXT, QVariant(QString()));
                args.append(QVariant(map));
                mRequest->setArguments(args);
                mRequest->send();
            }
            
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}
