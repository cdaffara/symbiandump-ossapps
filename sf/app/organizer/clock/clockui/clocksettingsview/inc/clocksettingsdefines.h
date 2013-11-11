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
* Definition file for ClockSettingsView.
*
*/

#ifndef CLOCKSETTINGSDEFINES_H
#define CLOCKSETTINGSDEFINES_H

// System includes
#include <qglobal.h>

#ifdef CLOCKSETTINGSVIEW_LIB
#	define CLOCKSETTINGSVIEW_EXPORT Q_DECL_EXPORT
#else
#	define CLOCKSETTINGSVIEW_EXPORT Q_DECL_IMPORT
#endif

// Definitions for docml.
#define CLOCK_SETTINGS_VIEW_DOCML				":/xml/clocksettingsview.docml"
#define CLOCK_SETTINGS_VIEW_PORTRAIT_SECTION	"portrait"
#define CLOCK_SETTINGS_VIEW_LANDSCAPE_SECTION	"landscape"
#define CLOCK_SETTINGS_VIEW						"clockSettingsView"
#define CLOCK_WIDGET							"clockWidget"
#define CLOCK_SETTINGS_DATA_FORM				"dataForm"
#define CLOCK_SETTINGS_DATE_LABEL				"dateLabel"
#define CLOCK_SETTINGS_PLACE_LABEL				"placeLabel"

#define CLOCK_REG_SETTINGS_VIEW_DOCML	":/xml/clockregionalsettingsview.docml"
#define CLOCK_REG_SETTINGS_VIEW			"regionalSettingsView"
#define CLOCK_REG_SETTINGS_DATA_FORM	"dataForm"

#endif // CLOCKSETTINGSDEFINES_H

// End of file	--Don't remove this.
