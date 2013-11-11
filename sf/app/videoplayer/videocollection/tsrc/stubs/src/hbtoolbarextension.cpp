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

#include "hbtoolbarextension.h"

int HbToolBarExtension::mAddActionCallCount = 0;

HbToolBarExtension::HbToolBarExtension(QGraphicsItem *parent) 
: HbWidget(parent)
{

}

HbToolBarExtension::~HbToolBarExtension() 
{
    for(int i = 0; i < mActions.count(); i++)
    {
        delete mActions[i];
    }
    mActions.clear();
}

HbAction *HbToolBarExtension::addAction ( const HbIcon &icon, const QString &text,
                          const QObject *receiver, const char *member )
{
    Q_UNUSED(icon);
    Q_UNUSED(text);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    HbAction *action = new HbAction();
    mActions.append(action);
    mAddActionCallCount++;
    return action;
}

