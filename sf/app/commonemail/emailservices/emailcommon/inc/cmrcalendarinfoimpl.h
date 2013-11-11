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
* Description:  calender info extension for mailbox
*
*/
#ifndef CMRCALENDARINFOIMPL_H
#define CMRCALENDARINFOIMPL_H
                          
#include "cmrcalendarinfo.h" 
#include "cemailextensionbase.h"

/**
* Implementation of calender info
*/
NONSHARABLE_CLASS( CMRCalendarInfoImpl ) : public CMRCalendarInfo
{
public:

    CMRCalendarInfoImpl();
    
    ~CMRCalendarInfoImpl();
    
public: // from CMRCalendarInfo
    /**
    * Return name of calendar database used with sync.
    */
    virtual void GetCalendarDatabaseIdL( TCalFileId& aId ) const;

    /**
    * Sets calender database name.
    */    
    virtual void SetCalendarDatabaseIdL( const TCalFileId& aId );
    
private:
    // calender database name.
    TCalFileId iDatabaseId;        
};

#endif // CMRCALENDARINFOIMPL_H
