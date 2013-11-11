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
#ifndef HBTOOLBAR_H
#define HBTOOLBAR_H

#include <QObject>
#include <QAction>

class HbAction;

extern bool m_clearCalled;
extern bool m_hideCalled;
extern bool m_showCalled;
extern bool m_clearActionsCalled;
extern bool m_setOrientationCalled;
extern bool m_toolBarUpdateCalled;
extern HbAction *m_action;
extern QList<QAction*> m_actions;


class HbToolBar : public QObject
{
    Q_OBJECT

public:

    HbToolBar (QObject *parent = 0);    
    virtual ~HbToolBar () {};
    void reset ();
    void clear ();
    void hide ();
    void show ();
    void addAction (HbAction *action);
    void clearActions ();
    void setOrientation(Qt::Orientation orientation);
    void removeAction(HbAction *action);
    QList<QAction*> actions() const;
    void update();

private:
    
};

#endif // HBTOOLBAR_H
