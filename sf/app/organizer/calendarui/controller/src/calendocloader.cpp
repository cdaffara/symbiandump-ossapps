/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition file for class CalenDocLoader.
 *
*/

// System includes
#include <HbGroupBox>

// User includes
#include "calendocloader.h"
#include "calenagendaview.h"
#include "calenagendaviewwidget.h"
#include "calenmonthview.h"
#include "calenmonthgrid.h"
#include "calenpreviewpane.h"
#include "calencontroller.h"
#include "calenthicklinesdrawer.h"
#include "calencommon.h"
#include "calenpluginlabel.h"
#include "calendayview.h"
#include "calendayhourscrollarea.h"
#include "calendaycontentscrollarea.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calendocloaderTraces.h"
#endif

// ----------------------------------------------------------------------------
// CalenDocLoader::CalenDocLoader
// C++ default Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDocLoader::CalenDocLoader(CCalenController &controller) :
	mController(controller)
	{
    OstTraceFunctionEntry0( CALENDOCLOADER_CALENDOCLOADER_ENTRY );
    

	OstTraceFunctionExit0( CALENDOCLOADER_CALENDOCLOADER_EXIT );
	}

// ----------------------------------------------------------------------------
// CalenDocLoader::CalenDocLoader
// C++ default Destructor.
// ----------------------------------------------------------------------------
//
CalenDocLoader::~CalenDocLoader()
{
    OstTraceFunctionEntry0( DUP1_CALENDOCLOADER_CALENDOCLOADER_ENTRY );
    

    OstTraceFunctionExit0( DUP1_CALENDOCLOADER_CALENDOCLOADER_EXIT );
}
// ----------------------------------------------------------------------------
// CalenDocLoader::createObject
// @brief From HbDocumentLoader
// Called whenever the docml file is parsed to create objects.
// Over-riding it since we need to create custom views and widgets.
// @param type The object type which is being created. For ex: HbWidget.
// @param name The name of the created object.
// @return QObject* Pointer to the created object.
// ----------------------------------------------------------------------------
//
QObject *CalenDocLoader::createObject(const QString &type,
                                             const QString &name)
{
    OstTraceFunctionEntry0( CALENDOCLOADER_CREATEOBJECT_ENTRY );
    
    MCalenServices &services = mController.Services(); 
	if (name == QString(CALEN_AGENDAVIEW)) {
		QObject *calenAgendaView = new CalenAgendaView(services);
		calenAgendaView->setObjectName(name);
		OstTraceFunctionExit0( CALENDOCLOADER_CREATEOBJECT_EXIT );
		return calenAgendaView;
	} else if (name == QString(CALEN_AGENDAVIEW_WIDGET)) {
	    QObject *calenAgendaViewWidget = new CalenAgendaViewWidget(services, this);
	    calenAgendaViewWidget->setObjectName(name);
	    OstTraceFunctionExit0( DUP1_CALENDOCLOADER_CREATEOBJECT_EXIT );
	    return calenAgendaViewWidget;
	} else if (name == QString(CALEN_MONTHVIEW)) { // For Month View
        QObject *calenMonthView = new CalenMonthView(services);
        calenMonthView->setObjectName(name);
        OstTraceFunctionExit0( DUP2_CALENDOCLOADER_CREATEOBJECT_EXIT );
        return calenMonthView;
	} else if (name == QString(CALEN_MONTHGRID)) {
        QObject *calenMonthGrid = new CalenMonthGrid();
        calenMonthGrid->setObjectName(name);
        OstTraceFunctionExit0( DUP3_CALENDOCLOADER_CREATEOBJECT_EXIT );
        return calenMonthGrid;
	} else if(name == QString(CALEN_PREVPREVIEWPANE)) {
        QObject *prevPreviewPane =
        new CalenPreviewPane(services);
        prevPreviewPane->setObjectName(name);
        OstTraceFunctionExit0( DUP4_CALENDOCLOADER_CREATEOBJECT_EXIT );
        return prevPreviewPane;
    } else if(name == QString(CALEN_CURRPREVIEWPANE)) {
        QObject *currPreviewPane =
        new CalenPreviewPane(services);
        currPreviewPane->setObjectName(name);
        OstTraceFunctionExit0( DUP5_CALENDOCLOADER_CREATEOBJECT_EXIT );
        return currPreviewPane;
    } else if(name == QString(CALEN_NEXTPREVIEWPANE)) {
        QObject *nextPreviewPane =
        new CalenPreviewPane(services);
        nextPreviewPane->setObjectName(name);
        OstTraceFunctionExit0( DUP6_CALENDOCLOADER_CREATEOBJECT_EXIT );
        return nextPreviewPane;
    } else if (name == QString(CALEN_DAYNAMES_WIDGET)) {
	    QObject *calenDayNamesWidget = 
        new CalenThickLinesDrawer(CalendarNamespace::CalenDayNameWidget);
	    calenDayNamesWidget->setObjectName(name);
	    OstTraceFunctionExit0( DUP7_CALENDOCLOADER_CREATEOBJECT_EXIT );
	    return calenDayNamesWidget;
	} else if (name == QString(CALEN_WEEKNUMBERS_WIDGET)) {
	    QObject *calenSWeekNumWidget =
        new CalenThickLinesDrawer(CalendarNamespace::CalenWeekNumWidget);
	    calenSWeekNumWidget->setObjectName(name);
	    OstTraceFunctionExit0( DUP8_CALENDOCLOADER_CREATEOBJECT_EXIT );
	    return calenSWeekNumWidget;
	} else if (name == QString(CALEN_PREVREGIONALINFO)) {
		QObject *calenPrevRegionalLabel = new CalenPluginLabel(services);
		calenPrevRegionalLabel->setObjectName(name);
		OstTraceFunctionExit0( DUP9_CALENDOCLOADER_CREATEOBJECT_EXIT );
		return calenPrevRegionalLabel;
	} else if (name == QString(CALEN_CURRREGIONALINFO)) {
		QObject *calencurrRegionalLabel = new CalenPluginLabel(services);
		calencurrRegionalLabel->setObjectName(name);
		OstTraceFunctionExit0( DUP10_CALENDOCLOADER_CREATEOBJECT_EXIT );
		return calencurrRegionalLabel;
	} else if (name == QString(CALEN_NEXTREGIONALINFO)) {
		QObject *calenNextRegionalLabel = new CalenPluginLabel(services);
		calenNextRegionalLabel->setObjectName(name);
		OstTraceFunctionExit0( DUP11_CALENDOCLOADER_CREATEOBJECT_EXIT );
		return calenNextRegionalLabel;
	} else if (name == QString(CALEN_DAYVIEW)) {
        QObject *calenDayView = new CalenDayView(services);
        calenDayView->setObjectName(name);
        return calenDayView;
	} else if (name == QString(CALEN_DAYVIEW_DAYINFO)) {
	        QObject *calenDayInfo = new HbGroupBox();
	        calenDayInfo->setObjectName(name);
	        return calenDayInfo;
    } else if (name == QString(CALEN_DAYVIEW_CONTENTWIDGET)) {
        QObject *calenContentWidget = new HbWidget();
        calenContentWidget->setObjectName(name);
        return calenContentWidget;
    } else if (name == QString(CALEN_DAYVIEW_HOURSCROLLAREA)) {
            QObject *hourScrollArea = new CalenDayHourScrollArea();
            hourScrollArea->setObjectName(name);
            return hourScrollArea;
    } else if (name == QString(CALEN_DAYVIEW_CONTENTSCROLLAREA)) {
            QObject *contentScrollArea = new CalenDayContentScrollArea();
            contentScrollArea->setObjectName(name);
            return contentScrollArea;
	} else {
        OstTraceFunctionExit0( DUP12_CALENDOCLOADER_CREATEOBJECT_EXIT );
		return HbDocumentLoader::createObject(type, name);
	}
}

// End of file	--Don't remove this.
