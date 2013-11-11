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
* Implementation file for class ClockSettingsDocLoader.
*
*/

// User includes
#include "clocksettingsdocloader.h"
#include "clocksettingsdefines.h"
#include "clocksettingsview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clocksettingsdocloaderTraces.h"
#endif


/*!
	\class ClockSettingsDocLoader

	Inherits from HbDocumentLoader
	Called whenever the docml file is parsed to create objects
	Over-riding it since we need to create custom views and widgets.

	\param type The object type which is being created. For ex: HbWidget
	\param name The name of the created object

	\return QObject* Pointer to the created object
 */
QObject *ClockSettingsDocLoader::createObject(const QString &type, const QString &name)
{
	OstTraceFunctionEntry0( CLOCKSETTINGSDOCLOADER_CREATEOBJECT_ENTRY );
	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
