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
* Description: stub hbmessagebox
* 
*/

#include "hbmessagebox.h"

QString HbMessageBox::mLatestTxt = "";
int HbMessageBox::mType = -1;
int HbMessageBox::mAttribute = -1;
int HbMessageBox::mOpenCallCount = 0;
int HbMessageBox::mShowCallCount = 0;

HbMessageBox::HbMessageBox(MessageBoxType type, QGraphicsItem *parent)
{
    Q_UNUSED(parent);
    HbMessageBox::mType = type;
    
    HbAction *action = new HbAction();
    mActions.append(action);
    action = new HbAction();
    mActions.append(action);   
}

HbMessageBox::HbMessageBox(const QString &text, MessageBoxType type, QGraphicsItem *parent)
{
    Q_UNUSED(parent);
    HbMessageBox::mLatestTxt = text;
    HbMessageBox::mType = type;
    
    HbAction *action = new HbAction();
    mActions.append(action);
    action = new HbAction();
    mActions.append(action);   
}

HbMessageBox::~HbMessageBox()
{
    while(!mActions.isEmpty())
    {
        delete mActions.takeFirst();
    }
}

void HbMessageBox::setStandardButtons(StandardButtons /*buttons*/)
{
	//NOP
}

void HbMessageBox::show()
{
    HbMessageBox::mShowCallCount++;
}

void HbMessageBox::open( QObject* receiver, const char* member )
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    HbMessageBox::mOpenCallCount++;
}

void HbMessageBox::emitDialogFinished( QObject* receiver, const char* member, int actionNum )
{
    if(connect(this, SIGNAL(finished(int)), receiver, member))
    {
        emit finished(actionNum);
        disconnect(this, SIGNAL(finished(int)), receiver, member);
    }
}

void HbMessageBox::setAttribute(int attribute)
{
    HbMessageBox::mAttribute = attribute;
}
