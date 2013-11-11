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
* Description:  Class declaration for CalenDocLoader.
 *
*/


#ifndef CALENDOCLOADER_H
#define CALENDOCLOADER_H

// system includes
#include <hbdocumentloader.h>

// forward declaration
class CCalenController;

// Class declaration
/**
 * @class  CalenDocLoader
 * @brief The loader class for application
 * Inherits from HbDocumentLoader.
 */
class CalenDocLoader : public HbDocumentLoader
{
public:
	
	CalenDocLoader(CCalenController &controller) : mController(controller){}
	~CalenDocLoader(){}
	
protected:
	
	virtual QObject *createObject(const QString &type, const QString &name){Q_UNUSED(type); Q_UNUSED(name); return 0;}
	
private:
	
	CCalenController &mController;
};

#endif // CALENDOCLOADER_H

// End of file	--Don't remove this.
