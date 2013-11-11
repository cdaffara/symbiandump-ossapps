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
* Description: Declaration of the class AgendaEventViewerDocLoader
*
*/


#ifndef AGENDAEVENTVIEWERDOCLOADER_H
#define AGENDAEVENTVIEWERDOCLOADER_H

// System includes
#include <hbdocumentloader.h>

// Forward declarations
class QObject;
class QString;

class AgendaEventViewerDocLoader : public HbDocumentLoader
{
protected:
	QObject* createObject(const QString &type, const QString &name);
};

#endif // AGENDAEVENTVIEWERDOCLOADER_H

// End of file	--Don't remove this.
