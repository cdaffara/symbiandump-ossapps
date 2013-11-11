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



#ifndef CREATORNOTEELEMENT_H_
#define CREATORNOTEELEMENT_H_

#include "creator_scriptelement.h"

namespace creatornote
{
    _LIT(KNote, "note");
    _LIT(KText, "text");
}

class CCreatorNoteElement : public CCreatorScriptElement
{
public:
    static CCreatorNoteElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    
protected:
    CCreatorNoteElement(CCreatorEngine* aEngine);
};

#endif /*CREATORNOTEELEMENT_H_*/
