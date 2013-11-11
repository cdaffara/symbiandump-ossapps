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
 * Description: calender db create notification .
 *
 */

#ifndef CALENDERNOTIFICATION_H_
#define CALENDERNOTIFICATION_H_

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include "notification.h" 
/**  
 * The observer class gives notification to derive class .
 * When calender db creates in specific path.
 */

class CCalenderNotification : public CActive
{
public:
    /** 
     * CCalenderNotification::NewL()
     */
    static CCalenderNotification* NewL(MNotifyChange* aNotifyChange);

    /** 
     * CCalenderNotification::NewL()
     */
    static CCalenderNotification* NewLC(MNotifyChange* aNotifyChange);
    /**
     * ~CCalenderNotification.
     * Virtual Destructor.
     */
    virtual ~ CCalenderNotification();

private:
    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();
    /**
     * CMyLocationsEngine.
     * C++ default constructor. 
     */
    CCalenderNotification(MNotifyChange* aNotifyChange);
    
public:
    
    /**
     * CheckCalenderDbFileStructure.
     * Observes the calendar db file structure changes. 
     */
    void CheckCalenderDbFileStructure( TChar aDrive );

    /**
     *  Handles active object's request completion event. 
     */
    void RunL();
    /** 
     * Implements cancellation of an outstanding request. 
     */
    void DoCancel();
private:
    //data member
    //file server 
    RFs iFsession;
    //observer to provide notification on completion of event
    MNotifyChange& iNotifyChange;
};
#endif /* CALENDERNOTIFICATION_H_ */
//End of file
