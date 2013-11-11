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
* Description:  CalenPluginLabel implementation.
*
*/

// System includes
#include <hbtapgesture.h>

// User includes
#include "calenservices.h"
#include "calenpluginlabel.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenpluginlabelTraces.h"
#endif

/*!
 \class CalenPluginLabel

 Class to show plugin information on Label
 */

/*!
 constructor
 */
CalenPluginLabel::CalenPluginLabel(MCalenServices& services, 
                                   QGraphicsItem* parent) :
	HbLabel(parent),mServices(services)
{
    OstTraceFunctionEntry0( CALENPLUGINLABEL_CALENPLUGINLABEL_ENTRY );
    
	setAlignment(Qt::AlignCenter);
	grabGesture(Qt::TapGesture);
	
	OstTraceFunctionExit0( CALENPLUGINLABEL_CALENPLUGINLABEL_EXIT );
}

/*!
 Destructor
 */
CalenPluginLabel::~CalenPluginLabel()
{
    OstTraceFunctionEntry0( DUP1_CALENPLUGINLABEL_CALENPLUGINLABEL_ENTRY );
    
    OstTraceFunctionExit0( DUP1_CALENPLUGINLABEL_CALENPLUGINLABEL_EXIT );
}


/*!
	Functo listen for all gestures
*/
void CalenPluginLabel::gestureEvent(QGestureEvent *event)
{
    OstTraceFunctionEntry0( CALENPLUGINLABEL_GESTUREEVENT_ENTRY );
    
    if(HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) {
        if (gesture->state() == Qt::GestureFinished) {
            if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                // Regional label is tapped
                mServices.IssueCommandL(ECalenRegionalPluginTapEvent);
                event->accept(Qt::TapGesture);
            }
        }
    }
    
    OstTraceFunctionExit0( CALENPLUGINLABEL_GESTUREEVENT_EXIT );
}


// End of file  --Don't remove this.
