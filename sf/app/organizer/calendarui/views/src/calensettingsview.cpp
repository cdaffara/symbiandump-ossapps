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
* Description:  Source file for the class CalenSettingsView
*
*/


#include <QGraphicsItem>
#include <QtGui>
#include <QTimer>
#include <hbdataform.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <HbInstance>

#include <CalenUid.h>
#include "calensettingsview.h"
#include "calenservices.h"
#include "calensettings.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calensettingsviewTraces.h"
#endif


CalenSettingsView::CalenSettingsView(MCalenServices& services, QGraphicsItem *parent)
:HbView(parent), 
 mServices(services),
 mCalenSettingsForm(NULL),
 mCalenSettings(NULL),
 mSoftKeyAction(NULL)
{
    OstTraceFunctionEntry0( CALENSETTINGSVIEW_CALENSETTINGSVIEW_ENTRY );
    

	mCalenSettingsForm = new HbDataForm(this);

	// Set the title text.
	setTitle(hbTrId("txt_calendar_title_calendar"));

	// Construct the settings utility.
	if(mCalenSettingsForm)
	{
	    mCalenSettings = new CalenSettings(mServices, mCalenSettingsForm);
        if(mCalenSettings)
        {
    	    mCalenSettings->createModel();
            setWidget(mCalenSettingsForm);
        }
    }
	
	OstTraceFunctionExit0( CALENSETTINGSVIEW_CALENSETTINGSVIEW_EXIT );
}

CalenSettingsView::~CalenSettingsView()
{
    OstTraceFunctionEntry0( DUP1_CALENSETTINGSVIEW_CALENSETTINGSVIEW_ENTRY );
    
	if (mCalenSettings) {
		delete mCalenSettings;
	}
	if (mSoftKeyAction) {
		delete mSoftKeyAction;
	}
	
	OstTraceFunctionExit0( DUP1_CALENSETTINGSVIEW_CALENSETTINGSVIEW_EXIT );
}

void CalenSettingsView::launchPreviousView()
{
    OstTraceFunctionEntry0( CALENSETTINGSVIEW_LAUNCHPREVIOUSVIEW_ENTRY );
    
	// Remove the view from mainwindow

	disconnect(mSoftKeyAction, SIGNAL(triggered()), 
											this, SLOT(launchPreviousView()));
	mServices.MainWindow().removeView(this);
	mServices.IssueNotificationL(ECalenNotifySettingsClosed);
	
	OstTraceFunctionExit0( CALENSETTINGSVIEW_LAUNCHPREVIOUSVIEW_EXIT );
}

void CalenSettingsView::initializeForm()
{
    OstTraceFunctionEntry0( CALENSETTINGSVIEW_INITIALIZEFORM_ENTRY );
    
    mCalenSettings->populateSettingList();
    
    // Add view on main window and set back softkey
	mServices.MainWindow().addView(this);
	mSoftKeyAction = new HbAction(Hb::BackNaviAction);
	if(mSoftKeyAction)
	{
		setNavigationAction(mSoftKeyAction);
		connect(mSoftKeyAction, SIGNAL(triggered()), 
											this, SLOT(launchPreviousView()));
	}
	
	OstTraceFunctionExit0( CALENSETTINGSVIEW_INITIALIZEFORM_EXIT );
}

void CalenSettingsView::refreshView()
    {
    OstTraceFunctionEntry0( CALENSETTINGSVIEW_REFRESHVIEW_ENTRY );
    
    //populate the new items 
    mCalenSettings->populateSettingList();
    
    OstTraceFunctionExit0( CALENSETTINGSVIEW_REFRESHVIEW_EXIT );
    }

// End of file
