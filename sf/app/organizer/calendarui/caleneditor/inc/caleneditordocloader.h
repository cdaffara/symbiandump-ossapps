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
 * Description: Class declaration for CalenEditorDocLoader.
 *
 */

#ifndef CALENEDITORDOCLOADER_H_
#define CALENEDITORDOCLOADER_H_

// system includes
#include <hbdocumentloader.h>


class CalenEditorDocLoader : public HbDocumentLoader
{
public:
	CalenEditorDocLoader();
	~CalenEditorDocLoader();
	
private:
	virtual QObject *createObject(const QString &type, const QString &name);
	
};

#endif /* CALENEDITORDOCLOADER_H_ */

// End of file	--Don't remove this.
