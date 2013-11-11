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
* Description: 
*
*/




#ifndef CREATORSCRIPTELEMENTFACTORY_H_
#define CREATORSCRIPTELEMENTFACTORY_H_

#include "creator_scriptelement.h"

/**
 * Creates script element instance based on the element name and context.
 */
class TCreatorScriptElementFactory
{
public:
    static CCreatorScriptElement* CreateElementL(CCreatorEngine* aEngine, const TDesC& aElementName, const TDesC& aContext = KNullDesC);
};

#endif /*CREATORSCRIPTELEMENTFACTORY_H_*/
