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


#ifndef PERFMON_GRAPHSCONTAINER_H
#define PERFMON_GRAPHSCONTAINER_H

// INCLUDES
#include <coecntrl.h>

#include "perfmon_drawcallback.h"


// FORWARD DECLARATIONS
class CPerfMonModel;


// CLASS DECLARATIONS

class CPerfMonGraphsContainer : public CCoeControl, public MDrawUpdateCallback
    {
public:
    void ConstructL(const TRect& aRect);
    ~CPerfMonGraphsContainer();

private:
    void Draw(const TRect& aRect) const;
    void HandleResourceChange(TInt aType);

public:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode /*aType*/);

public: // from MDrawUpdateCallback
    void DrawUpdate();

private:
    CPerfMonModel*                      iModel;
    const CFont*                        iFont;
    };
    
#endif

// End of File
