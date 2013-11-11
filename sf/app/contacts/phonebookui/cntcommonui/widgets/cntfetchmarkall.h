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

#ifndef CNTFETCHSELECTIONITEM_H
#define CNTFETCHSELECTIONITEM_H

#include <QObject>
#include <hbwidget.h>

class HbCheckBox;
class HbTextItem;
class HbFrameItem;

class CntFetchMarkAll : public HbWidget
{
    Q_OBJECT
    
public:
    CntFetchMarkAll(QGraphicsItem *aParent = 0);
    ~CntFetchMarkAll();

    void setMaxContactCount( int aContactCount );
    void setSelectedContactCount( int aSelectedCount );
    
signals:
    void markAll(int aState);
    
private:    
    friend class TestCntFetchSelectionItems;
    
    HbCheckBox* mCheckBox; // not own
    HbTextItem* mCounterLabel; // not own
    HbFrameItem* mFrame;
    
    int mContactCount;
    int mSelectionCount;
};

#endif // CNTFETCHSELECTIONITEM_H
