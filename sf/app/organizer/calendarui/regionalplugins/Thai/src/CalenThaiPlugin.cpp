/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calendarui_debug.h"

#include <QtGui>

#include <eikenv.h>
#include <bautils.h>
#include <qstring.h>
#include <hblabel.h>
#include <hbwidget.h>
#include "CalenThaiPlugin.h"


// ============================ MEMBER FUNCTIONS ==============================


// ----------------------------------------------------------------------------
// ThaiYearL
// Global method.
// ----------------------------------------------------------------------------
//
static TInt ThaiYearL(const TTime& aDate)
    {
    TRACE_ENTRY_POINT;
    
    TDateTime dt = aDate.DateTime();
    TInt gregorianYear = dt.Year();
    const TInt offset = 543;
    
    TRACE_EXIT_POINT;
    return gregorianYear + offset;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::NewL
// ----------------------------------------------------------------------------
//
CCalenThaiPlugin* CCalenThaiPlugin::NewL(MCalenServices* aServices)
    {
    TRACE_ENTRY_POINT;
    
    CCalenThaiPlugin* self = new (ELeave) CCalenThaiPlugin(aServices );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::~CCalenThaiPlugin
// ----------------------------------------------------------------------------
//
CCalenThaiPlugin::~CCalenThaiPlugin()
    {
    TRACE_ENTRY_POINT;

    if (iServices)
        {
        iServices->CancelNotifications(this);
        iServices->Release();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CCalenThaiPlugin
// ----------------------------------------------------------------------------
//
CCalenThaiPlugin::CCalenThaiPlugin(MCalenServices* aServices)
    : iServices(aServices)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::ConstructL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iServices->RegisterForNotificationsL(this, ECalenNotifyContextChanged);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::UpdateCommonInfoL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::UpdateLocalizerInfoL()
	{
	TRACE_ENTRY_POINT;
	TTime focusTime = iServices->Context().FocusDateAndTimeL().TimeLocalL();
    iThaiYearText.Num( ThaiYearL( focusTime ) );  
    TRACE_EXIT_POINT;
	}


// ----------------------------------------------------------------------------
// CCalenThaiPlugin::HandleCommandL
// ----------------------------------------------------------------------------
//
TBool CCalenThaiPlugin::HandleCommandL( const TCalenCommand&  
                                               /*aCommand*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return EFalse;
	}
// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Avkon views
// ----------------------------------------------------------------------------
//
HbWidget* CCalenThaiPlugin::InfobarL( )
	{
	TRACE_ENTRY_POINT;

    CalenPluginLabel* labelControl = new CalenPluginLabel();
    TRAP_IGNORE(UpdateLocalizerInfoL());
    QString text = QString::fromUtf16(iThaiYearText.Ptr(),
            iThaiYearText.Length());
    labelControl->setPlainText(text);
    TRACE_EXIT_POINT;
    return labelControl;

    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarTextL
// This function is called to get Infobar text
// ----------------------------------------------------------------------------
//
QString* CCalenThaiPlugin::InfobarTextL()
    {
	TRACE_ENTRY_POINT;
    TRAP_IGNORE(UpdateLocalizerInfoL());
    QString text = QString::fromUtf16(iThaiYearText.Ptr(),iThaiYearText.Length());
	TRACE_EXIT_POINT;
    return (new QString(text));
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CustomiseMenu
// This function is called to add menuitem 
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::CustomiseMenu(HbMenu* aHbMenu)
    {
    TRACE_ENTRY_POINT;
	Q_UNUSED(aHbMenu);
    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenThaiPlugin::HandleNotification
// ----------------------------------------------------------------------------
//	
void CCalenThaiPlugin::HandleNotification(const TCalenNotification aNotification)
	{
	TRACE_ENTRY_POINT;
	if (aNotification == ECalenNotifyContextChanged)
        {
        TRAP_IGNORE(UpdateLocalizerInfoL());
        }
	TRACE_EXIT_POINT;	
		
	}
// ----------------------------------------------------------------------------
// CCalenEditorsPlugin::CommandHandlerL
// ----------------------------------------------------------------------------
	
MCalenCommandHandler* CCalenThaiPlugin::CommandHandlerL( TInt aCommand )
	{
	TRACE_ENTRY_POINT;
	Q_UNUSED(aCommand);
    TRACE_EXIT_POINT;
    return NULL;
	}


CalenPluginLabel::CalenPluginLabel(QGraphicsItem* parent)
    :HbLabel(parent)
    {
	TRACE_ENTRY_POINT;
    
	setAlignment(Qt::AlignCenter);
	setTextColor(Qt::blue);
    setTextWrapping(Hb::TextWrapping);
    setOpacity(12); 
	TRACE_EXIT_POINT;	
    }

CalenPluginLabel::~CalenPluginLabel()
    {
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

void CalenPluginLabel::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
    {
	TRACE_ENTRY_POINT;
	Q_UNUSED(painter);
	Q_UNUSED(option);
	Q_UNUSED(widget);
    TRACE_EXIT_POINT;
    }
//EOF



