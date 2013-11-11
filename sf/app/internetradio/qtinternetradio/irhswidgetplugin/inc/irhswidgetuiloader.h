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
*
*/

#ifndef IRHSWIDGETUILOADER_H_
#define IRHSWIDGETUILOADER_H_

#include <hbdocumentloader.h>

class IRHsWidgetUiLoader : public HbDocumentLoader
{
public:
    virtual QObject *createObject(const QString& aType, const QString &aName);
};

#endif /* IRHSWIDGETUILOADER_H_ */
