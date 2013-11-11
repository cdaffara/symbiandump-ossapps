/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Mocked classes for testing CalenDayInfo class
*
*/

#ifndef  CALENDAYINFO_H
#define  CALENDAYINFO_H

#include <calinstance.h>
#include "caleninstanceid.h"

const int KFSCalMaxDescriptionLength = 100;

/*!
 Mocked struct SCalenApptInfo
 */
struct SCalenApptInfo
{
    QModelIndex iIndex;
    QDateTime iStartTime;
    QDateTime iEndTime;
    bool iAllDay;
    TCalenInstanceId iId;
    AgendaEntry::Status iStatus;
    TBufC<KFSCalMaxDescriptionLength> iSummary;
    TUint32 iColor;
};

/*!
 Mocked class CalenTimeColumn
 */
class CalenTimeColumn
{
public:
    CalenTimeColumn() {
        
    }
    
    ~CalenTimeColumn() {
        
    }
};

/*!
 Mocked class CalenTimeRegion
 */
class CalenTimeRegion
{
public:
    CalenTimeRegion() {
        
    }
    
    ~CalenTimeRegion() {
        
    }
    
    QList<CalenTimeColumn> iColumns;
    
    int iStartSlot;
    int iEndSlot;
};

/*!
 Mocked class CalenDayInfo
 */
class CalenDayInfo
{
public:
    enum TSlotsInHour
    {
        EOne = 1, ETwo, EThree, EFour
    };

public:

    CalenDayInfo(TSlotsInHour aSlotsInHour) 
    {
        Q_UNUSED(aSlotsInHour)
    }
    virtual ~CalenDayInfo()
    {
        
    }

    void GetLocation( const SCalenApptInfo& aItemInfo, int& aStartSlot,
        int& aEndSlot, int& aColumnIndex, int& aColumns ) 
    {
        Q_UNUSED(aItemInfo)
        Q_UNUSED(aStartSlot)
        Q_UNUSED(aEndSlot)
        Q_UNUSED(aColumnIndex)
        Q_UNUSED(aColumns)
    }
    
    int AlldayCount() 
    {
        return 0;
    }
    
    const QList<CalenTimeRegion>& RegionList() const 
    {
       return mList; 
    }
    
    void InsertAlldayEvent( const SCalenApptInfo& aItemInfo )
    {
        Q_UNUSED(aItemInfo)
            
        CalenTimeRegion region;
        
        region.iStartSlot = 10;
        region.iEndSlot = 13;
        region.iColumns << CalenTimeColumn();
        
        mList  << region;
    }
    
public:
    QList<CalenTimeRegion> mList;
};

#endif // CALENDAYINFO_H

// End of File
