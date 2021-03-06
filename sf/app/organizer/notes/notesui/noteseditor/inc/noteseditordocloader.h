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
* Description: Header file for NotesEditorDocLoader
*
*/

#ifndef NOTESEDITORDOCLOADER_H
#define NOTESEDITORDOCLOADER_H

// System includes
#include <hbdocumentloader.h>

// Forward declarations
class QObject;
class QString;

class NotesEditorDocLoader : public HbDocumentLoader
{
protected:
	QObject* createObject(const QString &type, const QString &name);
};

#endif // NOTESEDITORDOCLOADER_H

// End of file	--Don't remove this.
