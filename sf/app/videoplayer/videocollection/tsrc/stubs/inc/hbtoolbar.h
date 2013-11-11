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
* Description:  hbtoolbar stub
*
*/

#ifndef HBTOOLBAR_H
#define HBTOOLBAR_H

#include <hbwidget.h>
#include "hbtoolbarextension.h"
#include <hbaction.h>

class HbToolBar : public HbWidget
{
    Q_OBJECT

public:
    HbToolBar( QGraphicsItem *parent = 0 );
    virtual ~HbToolBar();

    using HbWidget::addAction;
    
/*    HbAction *addAction( const QString &text );
    HbAction *addAction( const HbIcon &icon, const QString &text );
    HbAction *addAction( const QString &text, const QObject *receiver, const char *member );
    HbAction *addAction( const HbIcon &icon, const QString &text, const QObject *receiver, const char *member );
*/
    HbAction *addExtension( HbToolBarExtension *extension );
    /*
    HbAction *insertExtension( HbAction *before, HbToolBarExtension *extension );

    Qt::Orientation orientation() const;

    enum { Type = Hb::ItemType_ToolBar };
    int type() const { return Type; }

public slots:
    void setOrientation(Qt::Orientation orientation);
    void unsetOrientation();

protected:
    HbToolBar( HbToolBarPrivate &dd, QGraphicsItem *parent = 0 );

    QVariant itemChange( GraphicsItemChange change, const QVariant &value );
    void gestureEvent(QGestureEvent *event);
    void changeEvent( QEvent *event );
    void resizeEvent( QGraphicsSceneResizeEvent *event );
    void hideEvent(QHideEvent *event);
    bool event( QEvent *event );

private:
    Q_DECLARE_PRIVATE_D( d_ptr, HbToolBar )
    Q_DISABLE_COPY(HbToolBar)

    Q_PRIVATE_SLOT(d_func(), void _q_orientationChanged(Qt::Orientation))
#ifdef HB_EFFECTS
    Q_PRIVATE_SLOT(d_func(), void _q_delayedHide(HbEffect::EffectStatus status))
    //Q_PRIVATE_SLOT(d_func(), void _q_delayedShow(HbEffect::EffectStatus status))
#endif // HB_EFFECTS*/
    
    static int mAddExtensionCallCount;
    
    HbAction *mToolBarExtensionAction;
};

#endif // HBTOOLBAR_H
