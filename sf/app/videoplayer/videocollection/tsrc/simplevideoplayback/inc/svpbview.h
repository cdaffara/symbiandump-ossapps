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
* Description:  Main view of the SimpleVideoPlayback MPX view plugin.
*
*/

#ifndef SVPBVIEW_H_
#define SVPBVIEW_H_

#include <hbview.h>

class CSvpbNativeWindow;
class SvpbSurfaceContainer;

class SvpbView : public HbView
{
	Q_OBJECT

public:
	SvpbView(QGraphicsItem *parent=0);
	virtual ~SvpbView();

public:
    void activate();
    void deactivate();

    SvpbSurfaceContainer *surfaceContainer() const;

signals:
    void finished();
    void tapped();
    void longTapped();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void gestureEvent(QGestureEvent *event);
    bool handleTap(Qt::GestureState state);
    bool handleLongTap(Qt::GestureState state);

private:
    CSvpbNativeWindow *mNativeWindow;
};

#endif /* SVPBVIEW_H_ */
