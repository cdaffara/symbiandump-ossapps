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
 * Description: List View Item.
 *
 */

#ifndef CUSTOMLISTVIEWITEM_H
#define CUSTOMLISTVIEWITEM_H

#include <hblistviewitem.h>
#include <HbProgressBar>

#include "hsmenustates_global.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

// TODO: this is only temporary class for show progress bar.
// It should be remove when fix from orbit will be in official platfrom.
class HsProgressBar: public HbProgressBar
{
    public:
        HsProgressBar(QGraphicsItem *parent = 0) : HbProgressBar(parent),
            mTargetValue(0), mTimerId(0) {}
        void setTargetProgressValue(int value);

    protected:
        void paint(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0 );
        void timerEvent(QTimerEvent *event);

    private:
        int mTargetValue;
        int mTimerId;

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
};



class HsListViewItem : public HbListViewItem
{
    Q_OBJECT
    public:

        explicit HsListViewItem(QGraphicsItem *parent = 0);
        virtual ~HsListViewItem();

        virtual HbAbstractViewItem* createItem();
        virtual void updateChildItems();

    protected:
	    virtual void polish(HbStyleParameters &params);

    private:
	    HsProgressBar *progress;
        bool isProgress;
    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)
};


#endif // CUSTOMLISTVIEWITEM_H
