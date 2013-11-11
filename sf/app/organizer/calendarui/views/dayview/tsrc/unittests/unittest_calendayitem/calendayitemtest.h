/* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Test class for CalenDayItem
 *
 */
 
#ifndef CALENDAYITEMTEST_H
#define CALENDAYITEMTEST_H

// System includes
#include <HbFrameItem>
#include <HbTextItem>
#include <HbStyle>
#include <HbColorScheme>
#include <agendaentry.h>
#include <hbabstractviewitem.h>

// User includes
#include "calendaycommonheaders.h"
#include "calendayitem.h"
#include "calendaystatusstrip.h"
#include "calendaymodel.h"
#include "calendayutils.h"
#include "calenagendautils.h"
#include "calendaycontainer.h"

#ifndef __WINSCW__
#define private public
#define protected public
#endif

#include "calendayitem.h"

class CalenDayItemTest : public CalenDayItem
{
public:
    CalenDayItemTest::CalenDayItemTest(const CalenDayContainer *container) :
                CalenDayItem(container)
    {
#ifndef __WINSCW__
        mBg = new HbFrameItem("qtg_fr_cal_meeting_bg", HbFrameDrawer::NinePieces,
            this);
        mEventDesc = new HbTextItem(this);
        // TODO: probably ElideLeft needed for mirrored layout
        mEventDesc->setElideMode(Qt::ElideRight);
        mEventDesc->setTextWrapping(Hb::TextWrapAnywhere);
        mColorStripe = new CalenDayStatusStrip(this);

        HbStyle::setItemName(mBg, QLatin1String("backgroundFrame"));
        HbStyle::setItemName(mEventDesc, QLatin1String("eventDescription"));
        HbStyle::setItemName(static_cast<QGraphicsItem *> (mColorStripe),
            QLatin1String("colorStripe"));

        HbDeviceProfile deviceProfile;
        qreal stripeWidth = KCalenTimeStripWidth * deviceProfile.unitValue();
        mColorStripe->setPreferredWidth(stripeWidth);

        // Minimum size of event frame (without/with description) 
        mFrameMinWidth = KCalenMinEventWidth * deviceProfile.unitValue();
        mEventDescMinWidth = KCalenMinTouchableEventWidth
            * deviceProfile.unitValue();
        
        // Minimum width is assured by widgetml and css, additionally called here 
        // to prevent minimum size hint caching inside effectiveSizeHint
        setMinimumWidth(stripeWidth);
#endif
    }
};

#endif
