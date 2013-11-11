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
* Description: stub hbmessagebox
* 
*/

#include "hbmenu.h"
#include "hbmenudata.h"

HbAction *HbMenuData::mMenuAction = 0;
bool HbMenuData::mEnabledSetted = false;
bool HbMenuData::mReturnEmptyActionList = false;
QPointF HbMenuData::mExecPoint = QPointF();


HbMenu::HbMenu(QGraphicsItem *parent):
    HbWidget(parent)
{
}

HbMenu::~HbMenu()
{
    HbMenuData::reset();
    
    QMap<QString, HbAction*>::const_iterator iter = mActions.constBegin();
    while(iter != mActions.constEnd())
    {
        delete *iter;           
        iter++;
    }
    mActions.clear();
}

HbAction* HbMenu::exec(const QPointF &pos, HbAction *action)
{
    Q_UNUSED(action);
    
    HbMenuData::mExecPoint = pos;
    
    return 0;
}

HbAction* HbMenu::menuAction()
{
    return HbMenuData::mMenuAction;
}

HbAction* HbMenu::activeAction()
{
    return mActiveAction;
}

void HbMenu::setActiveAction(HbAction *action)
{
    mActiveAction = action;
}

void HbMenu::setEnabled(bool enabled)
{
    HbMenuData::mEnabledSetted = enabled;
}

HbAction* HbMenu::addAction(const QString &text,
    const QObject *receiver,
    const char *member)
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    
    if(mActions.contains(text))
    {
        delete mActions[text];
    }
    HbAction *action = new HbAction(text); 
    mActions[text] = action;
    
    return action;
}

QList<HbAction*> HbMenu::actions()
{
    if(HbMenuData::mReturnEmptyActionList)
    {
        return QList<HbAction*>();
    }
    return mActions.values();
}

void HbMenu::setPreferredPos(const QPointF &position,
    HbPopup::Placement placement)
{
    Q_UNUSED(placement);

    HbMenuData::mExecPoint = position;
}

void HbMenu::setDismissPolicy(HbPopup::DismissPolicy dismissPolicy)
{
    Q_UNUSED(dismissPolicy);
}

// end of file
