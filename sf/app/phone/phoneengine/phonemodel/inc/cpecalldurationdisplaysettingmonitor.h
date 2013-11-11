/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header of the phone engine base 
*                central repository monitor class
*
*/


#ifndef CPECALLDURATIONDISPLAYSETTINGMONITOR_H
#define CPECALLDURATIONDISPLAYSETTINGMONITOR_H

// INCLUDE FILES
#include "cpecenrepmonitor.h"

// CLASS DECLARATION

/**
*   CPECallDurationDisplaySettingMonitor
*   Retrieves call duration display setting from the central repository
*
*  @lib PhoneEngineBase.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPECallDurationDisplaySettingMonitor ) : public CPECenRepMonitor
{
public:
    static CPECallDurationDisplaySettingMonitor* NewL();
    ~CPECallDurationDisplaySettingMonitor();
    TInt GetValue(TInt& aValue);
protected: // From base class
    virtual void UpdateL();
protected:
    CPECallDurationDisplaySettingMonitor();
    void ConstructL();
private:
    TBool iDisplayCallDuration;
    TInt iError;
};

#endif // CPECALLDURATIONDISPLAYSETTINGMONITOR_H
