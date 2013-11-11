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

#include <qstring.h>

#include "hbmessagebox.h"
#include "hbmessageboxdata.h"
#include "testobjectstore.h"

bool HbMessageBoxData::mQuestionReturnValue = false;
QString HbMessageBoxData::mLatestTxt = "";
int HbMessageBoxData::mWarningCallCount = 0;
int HbMessageBoxData::mInformationCallCount = 0;
int HbMessageBoxData::mType = -1;
int HbMessageBoxData::mAttribute = -1;
int HbMessageBoxData::mOpenCallCount = 0;
int HbMessageBoxData::mShowCallCount = 0;

HbMessageBox::HbMessageBox(MessageBoxType type, QGraphicsItem *parent) :
 HbDialog(parent)       
{
    HbMessageBoxData::mType = type;

    HbAction *action = new HbAction();
    mActions.append(action);
    action = new HbAction();
    mActions.append(action);
    
    TestObjectStore::instance().addObject(this);
}
        
HbMessageBox::HbMessageBox(const QString &text, MessageBoxType type, QGraphicsItem *parent)
{
    Q_UNUSED(parent);
    HbMessageBoxData::mLatestTxt = text;
    HbMessageBoxData::mType = type;
    
    HbAction *action = new HbAction();
    mActions.append(action);
    action = new HbAction();
    mActions.append(action);
    
    TestObjectStore::instance().addObject(this);
}

HbMessageBox::~HbMessageBox()
{
    while(!mActions.isEmpty())
    {
        delete mActions.takeFirst();
    }
}

void HbMessageBox::show()
{
    HbMessageBoxData::mShowCallCount++;
}

void HbMessageBox::open( QObject* receiver, const char* member )
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    HbMessageBoxData::mOpenCallCount++;
}

void HbMessageBox::emitDialogFinished( QObject* receiver, const char* member, int actionNum )
{
    if(connect(this, SIGNAL(finished(int)), receiver, member))
    {
        emit finished(actionNum);
        disconnect(this, SIGNAL(finished(int)), receiver, member);
    }
}

void HbMessageBox::setStandardButtons(StandardButtons buttons)
{
    Q_UNUSED(buttons);
	//NOP
}

void HbMessageBox::information(const QString &informationText,
    QGraphicsWidget *headWidget,
    QGraphicsScene *scene,
    QGraphicsItem *parent)
{
    Q_UNUSED(headWidget);
    Q_UNUSED(scene);
    Q_UNUSED(parent);

    HbMessageBoxData::mLatestTxt = informationText;
    HbMessageBoxData::mInformationCallCount++;
}

void HbMessageBox::warning(const QString &warningText,
    QGraphicsWidget *headWidget,
    QGraphicsScene *scene,
    QGraphicsItem *parent)
{
    Q_UNUSED(headWidget);
    Q_UNUSED(scene);
    Q_UNUSED(parent);

    HbMessageBoxData::mLatestTxt = warningText;
    HbMessageBoxData::mWarningCallCount++;
}

void HbMessageBox::setAttribute(int attribute)
{
    HbMessageBoxData::mAttribute = attribute;
}

// end of file
