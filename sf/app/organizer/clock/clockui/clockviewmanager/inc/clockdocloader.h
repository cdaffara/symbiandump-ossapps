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
* Description:   Class declaration for ClockDocLoader.
*
*/

#ifndef CLOCKDOCLOADER_H
#define CLOCKDOCLOADER_H

// System includes
#include <hbdocumentloader.h>

// Class declaration
/**
 * \class  ClockDocLoader
 * \brief The loader class for countdown timer
 * Inherits from HbDocumentLoader.
 */
class ClockDocLoader : public HbDocumentLoader
{
protected:
	virtual QObject *createObject(const QString &type, const QString &name);
};

#endif // CLOCKDOCLOADER_H

// End of file	--Don't remove this.
