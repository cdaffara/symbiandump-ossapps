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
* Description: stub hbinputdialog
* 
*/

#include "hbinputdialog.h"
#include "testobjectstore.h"

int HbInputDialog::mGetTextCallCount = 0;
int HbInputDialog::mAttribute = -1;
QString HbInputDialog::mLastHeading = "";
QString HbInputDialog::mLastText = "";
QVariant HbInputDialog::mValueReturnValue = QVariant();
int HbInputDialog::mValueCallCount = 0;
int HbInputDialog::mOpenCallCount = 0;

HbInputDialog::HbInputDialog(QGraphicsItem *parent) : HbWidget(parent) 
{
    HbAction *action = new HbAction();
    mActions.append(action);
    action = new HbAction();
    mActions.append(action);
    
    TestObjectStore::instance().addObject(this);
}

HbInputDialog::~HbInputDialog()
{
    while(!mActions.isEmpty())
    {
        delete mActions.takeFirst();
    }
}   

void HbInputDialog::open( QObject* receiver, const char* member )
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    
    mOpenCallCount++;
}

void HbInputDialog::getText(const QString &heading
        ,QObject *receiver
        ,const char *member
        ,const QString &text
        ,QGraphicsScene *scene
        ,QGraphicsItem *parent)
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    Q_UNUSED(scene);
    Q_UNUSED(parent);
    
    mLastHeading = heading;
    mLastText = text;
    mGetTextCallCount++;
}

void HbInputDialog::emitDialogFinished( QObject* receiver, const char* member, int actionNum )
{
    if(connect(this, SIGNAL(finished(HbAction *)), receiver, member))
    {
        emit finished(mActions.value(actionNum));
        disconnect(this, SIGNAL(finished(HbAction *)), receiver, member);
    }
}

void HbInputDialog::setAttribute(int attribute)
{
    HbInputDialog::mAttribute = attribute;
}

void HbInputDialog::setPromptText(const QString &text, int row)
{
    while(mSetPromptTexts.count() < row+1)
    {
        mSetPromptTexts.append("");
    }
    mSetPromptTexts[row] = text;
}

void HbInputDialog::setValue(const QVariant &value, int row)
{
    while(mSetValues.count() < row+1)
    {
        mSetValues.append("");
    }
    mSetValues[row] = value;
}

