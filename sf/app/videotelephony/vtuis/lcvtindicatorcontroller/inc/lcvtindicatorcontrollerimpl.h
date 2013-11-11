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

#ifndef LCVTINDICATORCONTROLLERIMPL_H_
#define LCVTINDICATORCONTROLLERIMPL_H_

// FORWARD DECLARATION
class HbIndicator;

class LcVtIndicatorControllerImpl
    {
public:
    explicit LcVtIndicatorControllerImpl();
    virtual ~LcVtIndicatorControllerImpl();
    
    void enableActiveCallIndicator();
    void disableActiveCallIndicator();
    
private:
    HbIndicator* mIndicator;
    friend class UT_LcVtIndicatorController;
    };



#endif /* LCVTINDICATORCONTROLLERIMPL_H_ */
