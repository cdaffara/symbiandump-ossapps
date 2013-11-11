/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Defines some common data used by clock
*
*/



#ifndef CLOCKCOMMON_H
#define CLOCKCOMMON_H

// Constants
const QString CITY_INFO_DB_PATH("c:\\data\\clock\\");
const QString CITY_INFO_DB ("cityInfo.dat");
const QString CLOCK_DEBUG_FILE("debug.txt");
const QString PATH_TEXT("C:/");
const int KMaximumCityListCount(50);

// Common definitions for docml.
#define CLOCK_MAIN_VIEW_DOCML						":/xml/clockmainview.docml"
#define CLOCK_MAIN_VIEW_PORTRAIT_SECTION			"portrait"
#define CLOCK_MAIN_VIEW_LANDSCAPE_SECTION			"landscape"
#define CLOCK_MAIN_VIEW								"clockMainView"
#define CLOCK_WIDGET								"clockWidget"
#define CLOCK_ALARMLIST								"alarmListView"
#define CLOCK_NOALARMLABEL							"noAlarmLabel"
#define CLOCK_WORLD_VIEW_DOCML						":/xml/clockworldview.docml"
#define CLOCK_WORLD_VIEW							"worldClockView"
#define CLOCK_WORLD_CITYLIST						"worldClockCityListView"
#define CLOCK_WORLD_HOMECITY						"homeCityWidget"
#define CLOCK_WORLD_VIEW_PORTRAIT_NOHOMECITY_SECTION	"portrait-nohomecity"
#define CLOCK_WORLD_VIEW_PORTRAIT_HOMECITY_SECTION		"portrait-homecity"
#define CLOCK_WORLD_VIEW_LANDSCAPE_NOHOMECITY_SECTION	"landscape-nohomecity"
#define CLOCK_WORLD_VIEW_LANDSCAPE_HOMECITY_SECTION		"landscape-homecity"

#endif		// CLOCKCOMMON_H

// End of file	--Don't remove this.
