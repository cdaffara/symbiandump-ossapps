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


#ifndef PERFMON_DATAPOPUPCONTAINER_H
#define PERFMON_DATAPOPUPCONTAINER_H

// INCLUDES
#include <coecntrl.h>

#include "perfmon_drawcallback.h"


// FORWARD DECLARATIONS
class CPerfMonModel;


// CLASS DECLARATIONS

class CPerfMonDataPopupContainer : public CCoeControl, public MDrawUpdateCallback
    {
public:
    void ConstructL(const TRect& aRect);
    ~CPerfMonDataPopupContainer();

protected:
    void Draw(const TRect& aRect) const;
    virtual void SizeChanged();    

public:
    void SetPositionAndSize();
    void UpdateVisibility(TBool aForeground=ETrue);

public: // from MDrawUpdateCallback
    void DrawUpdate();

private:
    CPerfMonModel*                      iModel;
    RWindowGroup                        iWindowGroup;
    const CFont*                        iFont; 
    TInt                                iFontSize;
    };
    
#endif

// End of File
