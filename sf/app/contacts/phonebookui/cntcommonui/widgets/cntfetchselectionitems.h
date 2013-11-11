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

class QGraphicsLinearLayout;
class HbCheckBox;
class HbLabel;

/**
 * Custom widget class for use with a HbPopup. 
 * This adds a "Mark All" checkbox and a selection counter.
 */
class CntFetchSelectionItems : public HbWidget
{
    Q_OBJECT
    
public:
    CntFetchSelectionItems(QGraphicsItem *aParent = 0);
    ~CntFetchSelectionItems();
    
    HbLabel* counter() const;
    
signals:
    void passStateChanged(int aState);
    
public slots:
    void handleStateChange(int aState);

private:    
    friend class TestCntFetchSelectionItems;
    
    HbCheckBox*             mCheckBox;
    HbLabel*                mCounterLabel;
    QGraphicsLinearLayout*  mLayout; // Ownership transferred to this class
};

#endif // CNTFETCHSELECTIONITEM_H
