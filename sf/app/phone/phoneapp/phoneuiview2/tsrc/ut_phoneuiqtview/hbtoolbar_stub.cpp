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
#include "hbtoolbar.h"
#include <hbaction.h>

bool m_clearCalled;
bool m_hideCalled;
bool m_showCalled;
bool m_clearActionsCalled;
bool m_setOrientationCalled;
bool m_toolBarUpdateCalled;
HbAction *m_action;
QList<QAction*> m_actions;

HbToolBar::HbToolBar (QObject *parent)
{
    Q_UNUSED (parent);
    reset ();
}

void HbToolBar::reset ()
{
    m_clearCalled = false;
    m_hideCalled = false;
    m_showCalled = false;
    m_clearActionsCalled = false;
    m_toolBarUpdateCalled = false;
    m_action = 0;
}

void HbToolBar::clear ()
{
    m_clearCalled = true;	
}

void HbToolBar::hide ()
{
    m_hideCalled = true;	
}

void HbToolBar::show ()
{
    m_showCalled = true;
}

void HbToolBar::addAction (HbAction *action)
{
    m_action = action;
    m_actions.append(action);
}

void HbToolBar::clearActions ()
{
    m_clearActionsCalled = true;
}

void HbToolBar::setOrientation(Qt::Orientation orientation)
{
    m_setOrientationCalled = true;
}

void HbToolBar::removeAction(HbAction *action)
{
    m_actions.removeOne(action);
}

QList<QAction*> HbToolBar::actions() const
{
    return m_actions;	
}

void HbToolBar::update()
{
    m_toolBarUpdateCalled = true;
}

