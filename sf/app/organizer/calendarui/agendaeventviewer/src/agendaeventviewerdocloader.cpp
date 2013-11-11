/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of the class AgendaEventViewerDocLoader
*
*/

// User includes.
#include "agendaeventviewerdocloader.h"
#include "agendaeventvieweritem.h"
#include "agendaeventviewercommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "agendaeventviewerdocloaderTraces.h"
#endif


/*!
	\class AgendaEventViewerDocLoader

	Custom document loader class for constructing custom widgets/objects.
 */

/*!
	From HbDocumentLoader.
	Creates and returns an object of type and assigns name as its object name.

	\param type The type name as mentioned in docml.
	\param name The name of the object as mentioned in docml.
	\return QObject* Pointer to the constructed QObject.
	\sa HbDocumentLoader
*/
QObject* AgendaEventViewerDocLoader::createObject(const QString &type,
                                                  const QString &name)
{
	OstTraceFunctionEntry0( AGENDAEVENTVIEWERDOCLOADER_CREATEOBJECT_ENTRY );
	if (name == QString(AGENDA_EVENT_VIEWER_SUBJECT_WIDGET) 
			|| name == QString(AGENDA_EVENT_VIEWER_DATE_TIME_WIDGET) 
			|| name == QString(AGENDA_EVENT_VIEWER_LOCATION_WIDGET)
			|| name == QString(AGENDA_EVENT_VIEWER_REPEAT_WIDGET)
			|| name == QString(AGENDA_EVENT_VIEWER_REMINDER_WIDGET)
			|| name == QString(AGENDA_EVENT_VIEWER_DESCRIPTION_WIDGET)) {
		
		QObject *agendaEventViewerItem = new AgendaEventViewerItem();
		agendaEventViewerItem->setObjectName(name);
		OstTraceFunctionExit0( AGENDAEVENTVIEWERDOCLOADER_CREATEOBJECT_EXIT );
		return agendaEventViewerItem;
	} else {
	    OstTraceFunctionExit0( DUP1_AGENDAEVENTVIEWERDOCLOADER_CREATEOBJECT_EXIT );
		return HbDocumentLoader::createObject(type, name);
	}
}

// End of file	--Don't remove this.
