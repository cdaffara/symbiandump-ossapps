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
* Definition file for class NotesEditorDocLoader.
*
*/

// User includes
#include "noteseditordocloader.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "noteseditordocloaderTraces.h"
#endif


/*!
	\class NotesEditorDocLoader

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
QObject* NotesEditorDocLoader::createObject(const QString &type, const QString &name)
{
	OstTraceFunctionEntry0( NOTESEDITORDOCLOADER_CREATEOBJECT_ENTRY );
	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
