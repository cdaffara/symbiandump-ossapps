/*!
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
* Description:  Handles phone indicators.
*/

#ifndef LCVTINDICATORCONTROLLER_H_
#define LCVTINDICATORCONTROLLER_H_

// FORWARD DECLARATION

class LcVtIndicatorControllerImpl;

class LcVtIndicatorController
    {
public:
    IMPORT_C explicit LcVtIndicatorController();
    IMPORT_C virtual ~LcVtIndicatorController();

    IMPORT_C void enableActiveCallIndicator();
    IMPORT_C void disableActiveCallIndicator();
    
private:
    LcVtIndicatorControllerImpl* mIndicatorimpl;
    
    friend class UT_LcVtIndicatorController;
    };

#endif /* LCVTINDICATORCONTROLLER_H_ */
