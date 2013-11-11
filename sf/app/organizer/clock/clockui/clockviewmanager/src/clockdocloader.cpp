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
* Description:   Definition file for class ClockDocLoader.
 *
*/

// User includes
#include "clockdocloader.h"
#include "clockcommon.h"
#include "clockmainview.h"
#include "clockwidget.h"
#include "clockworldview.h"
#include "clockhomecityitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockdocloaderTraces.h"
#endif


/*!
	\class ClockDocLoader

	Inherits from HbDocumentLoader
	Called whenever the docml file is parsed to create objects
	Over-riding it since we need to create custom views and widgets.

	\param type The object type which is being created. For ex: HbWidget
	\param name The name of the created object

	\return QObject* Pointer to the created object
 */
QObject *ClockDocLoader::createObject(const QString &type, const QString &name)
{
	OstTraceFunctionEntry0( CLOCKDOCLOADER_CREATEOBJECT_ENTRY );
	if (CLOCK_MAIN_VIEW == name) {
		QObject *object = new ClockMainView();
		object->setObjectName(name);
		OstTraceFunctionExit0( CLOCKDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (CLOCK_WIDGET == name) {
		QObject *object = new ClockWidget();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP1_CLOCKDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (CLOCK_WORLD_VIEW == name) {
		QObject *object = new ClockWorldView();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP2_CLOCKDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	} else if (CLOCK_WORLD_HOMECITY == name) {
		QObject *object = new ClockHomeCityItem();
		object->setObjectName(name);
		OstTraceFunctionExit0( DUP3_CLOCKDOCLOADER_CREATEOBJECT_EXIT );
		return object;
	}

	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
