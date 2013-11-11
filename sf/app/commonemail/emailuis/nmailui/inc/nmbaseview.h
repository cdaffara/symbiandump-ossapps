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

#ifndef NMBASEVIEW_H_
#define NMBASEVIEW_H_

#include <QObject>

#include <hbview.h>

#include "nmuiviewids.h"

class NmUiStartParam;
class NmApplication;

/*!
	Base class for views
*/
class NmBaseView : public HbView
{
	Q_OBJECT
public:
    NmBaseView(NmUiStartParam *startParam,
               NmApplication &application,
               QGraphicsItem *parent = 0);
    virtual ~NmBaseView();
    virtual void reloadViewContents(NmUiStartParam *startParam) = 0;
    virtual NmUiViewId nmailViewId() const = 0;
    virtual void okToExitView();
    virtual void aboutToExitView();
    virtual void viewReady();
    virtual void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void handleMousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void handleMouseMoveEvent(QGraphicsSceneMouseEvent *event);

public slots:
    virtual void  aboutToChangeOrientation();
    virtual void  orientationChanged(Qt::Orientation orientation);

protected:
    virtual void loadViewLayout() = 0;

protected:
    NmUiStartParam *mStartParam;   // Owned
    NmApplication &mApplication;
};


#endif /* NMBASEVIEW_H_ */
