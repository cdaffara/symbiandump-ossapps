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
 * Description: Definition file for class AlarmAlertDocLoader.
 *
 */

// System includes
#include <qvariant.h>

// User includes
#include "alarmalertdocloader.h"
#include "alarmalertwidget_p.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "alarmalertdocloaderTraces.h"
#endif

AlarmAlertDocLoader::AlarmAlertDocLoader(const QVariantMap &parameters):
mParameters(parameters)
{
	OstTraceFunctionEntry0( ALARMALERTDOCLOADER_ALARMALERTDOCLOADER_ENTRY );
	
	OstTraceFunctionExit0( ALARMALERTDOCLOADER_ALARMALERTDOCLOADER_EXIT );
}

AlarmAlertDocLoader::~AlarmAlertDocLoader()
{
	OstTraceFunctionEntry0( DUP1_ALARMALERTDOCLOADER_ALARMALERTDOCLOADER_ENTRY );

	OstTraceFunctionExit0( DUP1_ALARMALERTDOCLOADER_ALARMALERTDOCLOADER_EXIT );
}

QObject* AlarmAlertDocLoader::createObject(const QString &type, 
															const QString &name)
{
	OstTraceFunctionEntry0( ALARMALERTDOCLOADER_CREATEOBJECT_ENTRY );
	if (name == "dialog"){ 
		QObject *alarmAlertDialogPrivate = new AlarmAlertDialogPrivate(
																	mParameters);
		alarmAlertDialogPrivate->setObjectName(name);
		OstTraceFunctionExit0( ALARMALERTDOCLOADER_CREATEOBJECT_EXIT );
		return alarmAlertDialogPrivate;
	}else {
		return HbDocumentLoader::createObject(type, name);
	}
}

// End of file  --Don't remove this.
