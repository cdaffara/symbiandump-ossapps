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
#ifndef IRTERMSCONSVIEW_H
#define IRTERMSCONSVIEW_H

 
#include "irbaseview.h"

class IRTermsConsView : public IRBaseView
{
    Q_OBJECT
        
public:
    ~IRTermsConsView();
    
private:
    IRTermsConsView(IRApplication* aApplication, TIRViewId aViewId);

private slots:
	void handleOrientationChanged( Qt::Orientation aOrientation );
	    
private:
    void initViewContents();
    
    friend class IRViewManager;
};

#endif // IRTERMSCONSVIEW_H
