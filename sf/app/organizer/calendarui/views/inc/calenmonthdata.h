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
* Description:  CalenEventLabel class definition.
*
*/


#ifndef     CALENMONTHDATA_H
#define     CALENMONTHDATA_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <QDateTime>

//  CLASS DEFINITIONS
/**
 *  For data of month view
 */
NONSHARABLE_CLASS( CalenMonthData )
    {
public :  // Constructors and destructor
    /**
     * C++ default constructor.
     */
    CalenMonthData();

    /**
     * C++ constructor.
     */
    CalenMonthData(QDateTime day);

    /**
     * Destructor.
     */
    virtual ~CalenMonthData();

public : // New funcitons
    /**
     * return day
     * @return day
     */
    QDateTime Day();

    /**
     * 
     */
    void SetHasEvents(bool HasEvents);
    
    /**
     * 
     */
    bool HasEvents();
    
    void setActive(bool isActive);
    
    bool isActive();

private :  // New data
    /**
     * 
     */
    QDateTime mDay;
    
    /**
     * 
     */
    bool mHasEvents;
    
    bool mIsActive;
    };

#endif      //  CALENMONTHDATA_H


// End of File
