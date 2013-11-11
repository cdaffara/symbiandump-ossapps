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
* Description:  Storage class for day and week views.
*
*/


//  INCLUDES
#include "calendayinfo.h"
#include "calenagendautils.h"
#include "calenconstants.h"
#include "calendateutils.h"
#include "calendayutils.h"

namespace
{
const int KUntimedDefaultSlot(16);
}

/*!
 \brief CalenDayInfo::CalenDayInfo
*/
CalenDayInfo::CalenDayInfo(TSlotsInHour aSlotsInHour) :
    iFirstUntimedSlot(KUntimedDefaultSlot), iSlotsInHour(aSlotsInHour)
{
    iSelectedSlot = KFSCalStartingHour * iSlotsInHour;
    iSelectedRegion = KErrNotFound;
    iSelectedColumn = 0;
    iSelectedAlldayEvent = KErrNotFound;
    iEarliestEndSlot = KErrNotFound;
    iLastStartSlot = KErrNotFound;
}

/*!
 \brief Destructor
*/
CalenDayInfo::~CalenDayInfo()
{
    for (int i = 0; i < iRegionList.count(); i++) {
        iRegionList[i].Close();
    }
    iRegionList.clear();
    iUntimedEvents.clear();
    iTodoEvents.clear();
    iAlldayEvents.clear();
}

/*!
 \brief CalenDayInfo::Reset
*/
void CalenDayInfo::Reset()
{
    for (int i = 0; i < iRegionList.count(); i++) {
        iRegionList[i].Close();
    }
    iRegionList.clear();
    iUntimedEvents.clear();
    iTodoEvents.clear();
    iAlldayEvents.clear();

    iUntimedSlotCount = 0;
    iFirstUntimedSlot = KFSCalStartingHour * iSlotsInHour;

    iSelectedSlot = KFSCalStartingHour * iSlotsInHour;
    iSelectedRegion = KErrNotFound;
    iSelectedColumn = 0;
    iSelectedAlldayEvent = KErrNotFound;
    iEarliestEndSlot = KErrNotFound;
    iLastStartSlot = KErrNotFound;
}

/*!
 \brief CalenDayInfo::InsertTimedEvent
*/
void CalenDayInfo::InsertTimedEvent(const SCalenApptInfo& aItemInfo)
{

    int startIndex = SlotIndexForStartTime(aItemInfo.iStartTime);
    int endIndex = SlotIndexForEndTime(aItemInfo.iEndTime);
    
    // 10.1 update: the minimum height of event is defined by UI spec.,
    // so number of occupied slots must be verified to hold at least bubble
    // with minimum possible height
    qreal slotHeight = CalenDayUtils::instance()->hourElementHeight()
        / iSlotsInHour;
    int minSlotsInEvent = qRound((CalenDayUtils::instance()->minEventHeight()
        / slotHeight) + 0.5);
    if (endIndex < startIndex + minSlotsInEvent) {
        endIndex = startIndex + minSlotsInEvent;
    }
    if (iRegionList.count() > 0) {
        // the timed events must be added in order
        ASSERT( startIndex >= iLastStartSlot );
    }

    CalenTimedEventInfo info;
    info.iStartSlot = startIndex;
    info.iEndSlot = endIndex;
    info.iId = aItemInfo.iId;
    info.iStatus = aItemInfo.iStatus;
    //    info.iReplicationStatus = aItemInfo.iReplicationStatus;
    iLastStartSlot = startIndex;
    if (iEarliestEndSlot == KErrNotFound || endIndex < iEarliestEndSlot) {
        iEarliestEndSlot = endIndex;
    }

    bool added(false);
    if (iRegionList.count() > 0) {
        // Since events are added in order, the event either overlaps the
        // last region or doesn't overlap any region at all
        CalenTimeRegion& region = iRegionList[iRegionList.count() - 1];
        if (region.Overlaps(info)) {
            region.AddEvent(info);
            added = true;
        }
    }

    if (!added) {
        // If it didn't overlap the last region, add a new region for this event.
        iRegionList.append(CalenTimeRegion());
        CalenTimeRegion& region = iRegionList[iRegionList.count() - 1];
        region.AddEvent(info);

        // Do rounding of the region ends
        if (iRegionList.count() >= 2) {
            // Check if the previous region end time can be rounded down
            CalenTimeRegion& prevRegion = iRegionList[iRegionList.count() - 2];
            int end = RoundHourDown(prevRegion.iEndSlot);
            if (end <= region.iStartSlot) {
                prevRegion.iEndSlot = end;
            }
            // Check if the start time of the new region can be rounded up
            int start = RoundHourUp(region.iStartSlot);
            if (start >= prevRegion.iEndSlot) {
                region.iStartSlot = start;
            }
        }
        else {
            // No previous region, round the start time up
            region.iStartSlot = RoundHourUp(region.iStartSlot);
        }
    }
}

/*!
 \brief CalenDayInfo::InsertAlldayEvent
*/
void CalenDayInfo::InsertAlldayEvent(const SCalenApptInfo& aItemInfo)
{
    CalenTimedEventInfo info;
    info.iStartSlot = 0;
    info.iEndSlot = iSlotsInHour * KCalenHoursInDay + iUntimedSlotCount;
    info.iId = aItemInfo.iId;
    info.iStatus = aItemInfo.iStatus;
    //    info.iReplicationStatus = aItemInfo.iReplicationStatus;

    iAlldayEvents.append(info);
}

/*!
 \brief CalenDayInfo::IsAllDayEvent
*/
bool CalenDayInfo::IsAlldayEvent(QDateTime aStart, QDateTime aEnd)
{
    bool isAllDay(false);

    if (!CalenDateUtils::onSameDay(aStart, aEnd)) {
        QDateTime startMidnight = CalenDateUtils::beginningOfDay(aStart);
        QDateTime endMidnight = CalenDateUtils::beginningOfDay(aEnd);

        if ((aStart == startMidnight) && (aEnd == endMidnight)) {
            isAllDay = true;
        }
    }
    return isAllDay;
}

/*!
 \brief CalenDayInfo::IsAllDayEvent
 Allday event is an event starting and ending at midnight,
 with a duration of n*24h.
*/
/*
bool CalenDayInfo::IsAlldayEvent(const CCalInstance& aInstance)
{
    bool isAllDay(false);

    // If getting the start or end time fails, return false.
    QDateTime start;
    QDateTime end;
    TRAPD( error,
        {
            start = aInstance.StartTimeL().TimeLocalL();
            end = aInstance.EndTimeL().TimeLocalL();
        });

    if (error == KErrNone && !CalenDateUtils::onSameDay(start, end)) {
        QDateTime startMidnight = CalenDateUtils::beginningOfDay(start);
        QDateTime endMidnight = CalenDateUtils::beginningOfDay(end);

        if ((start == startMidnight) && (end == endMidnight)) {
            isAllDay = true;
        }
    }

    if (error != KErrNone) {
        CEikonEnv::Static()->HandleError(error);//codescanner::eikonenvstatic
    }

    return isAllDay;
}
*/

/*!
 \brief CalenDayInfo::AlldayCount
*/
int CalenDayInfo::AlldayCount()
{
    return iAlldayEvents.count();
}

/*!
 \brief CalenDayInfo::TodoCount
*/
int CalenDayInfo::TodoCount()
{
    return iTodoEvents.count();
}

/*!
 \brief CalenDayInfo::GetLocation
*/
void CalenDayInfo::GetLocation(
    const SCalenApptInfo& aItemInfo,
    int& aStartSlot,
    int& aEndSlot,
    int& aColumnIndex,
    int& aColumns)
{
    int startIndex = SlotIndexForStartTime(aItemInfo.iStartTime);
    int endIndex = SlotIndexForEndTime(aItemInfo.iEndTime);
    if (endIndex == startIndex) {
        endIndex++;
    }

    aStartSlot = startIndex;
    aEndSlot = endIndex;

    CalenSlotInterval interval;
    interval.iStartSlot = startIndex;
    interval.iEndSlot = endIndex;

    bool found(false);
    // Scan through all allday events and see if it matches any of them
    for (int i = 0; i < iAlldayEvents.count() && !found; i++) {
        if (aItemInfo.iId == iAlldayEvents[i].iId) {
            aColumns = iAlldayEvents.count();
            aColumnIndex = i;
            found = true;
        }
    }

    // Scan through all regions and see if the event overlaps any region.
    // If it overlaps a region, the event must be in that region.
    for (int i = 0; i < iRegionList.count() && !found; i++) {
        CalenTimeRegion& region = iRegionList[i];
        if (region.Overlaps(interval)) {
            // Scan through all columns and look for the event
            aColumns = region.iColumns.count();
            for (int j = 0; j < aColumns && !found; j++) {
                if (region.iColumns[j].ContainsEvent(aItemInfo.iId)) {
                    aColumnIndex = j;
                    found = true;
                }
            }
            // the event should be in this region if it overlaps the region
            ASSERT( found );
        }
    }
    // the event should have been found somewhere
    ASSERT( found );
}

/*!
 \brief CalenDayInfo::GetSelectedSlot
*/
void CalenDayInfo::GetSelectedSlot(int& aSlot, int& aRegion, int& aColumnIndex, int& aColumns)
{

    aSlot = iSelectedSlot;
    aColumnIndex = iSelectedColumn;
    if (iSelectedRegion >= 0) {
        aColumns = iRegionList[iSelectedRegion].iColumns.count();
        aRegion = iSelectedRegion;
    }
    else {
        aColumns = 0;
        aRegion = KErrNotFound;
    }
}

/*!
 \brief CalenDayInfo::FindRegion
*/
int CalenDayInfo::FindRegion(const CalenSlotInterval& aInterval, int aDirection)
{
    // Setup the start and end region indices for iterating
    int start, end;
    if (aDirection > 0) {
        start = 0;
        end = iRegionList.count();
    }
    else {
        start = iRegionList.count() - 1;
        end = -1;
    }

    int region = KErrNotFound;

    // Iterate over the regions, in the order given, looking for a region
    // overlapping this interval
    for (int i = start; i != end && region < 0; i += aDirection) {
        if (iRegionList[i].Overlaps(aInterval)) {
            region = i;
        }
    }
    return region;
}

/*!
 \brief Find an event within the given interval of the current column,
 searching in the given direction.
*/
int CalenDayInfo::FindEvent(const CalenSlotInterval& aInterval, int aDirection)
{
    ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.count() );

    CalenTimeRegion& region = iRegionList[iSelectedRegion];
    int index = KErrNotFound;

    if (iSelectedColumn < region.iColumns.count()) {
        CalenTimeColumn& column = region.iColumns[iSelectedColumn];
        int start, end;
        if (aDirection > 0) {
            start = 0;
            end = column.iEventArray.count();
        }
        else {
            start = column.iEventArray.count() - 1;
            end = -1;
        }

        // Iterate over the events in the chosen column in the order given,
        // looking for an event overlapping this interval
        for (int i = start; i != end && index < 0; i += aDirection) {
            if (column.iEventArray[i].Overlaps(aInterval)) {
                index = i;
            }
        }
    }
    return index;
}

/*!
 \brief CalenDayInfo::IsEventSelected
*/
bool CalenDayInfo::IsEventSelected() const
{
    bool selected(false);
    if (iSelectedAlldayEvent >= 0) {
        selected = true;
    }
    else
        if (iSelectedRegion >= 0 && iSelectedColumn < iRegionList[iSelectedRegion].iColumns.count()
            && iSelectedColumnEventIndex >= 0) {
            selected = true;
        }
        else
            if (IsExtraSlot(iSelectedSlot) && iSelectedSlot - iFirstUntimedSlot
                >= iEmptyUntimedSlots) {
                selected = true;
            }
            else {
                // if no event is selected, the selection slot must be a hour starting slot or an extra slot
                ASSERT( IsHourStartSlot( iSelectedSlot ) || IsExtraSlot( iSelectedSlot ) );
                selected = false;
            }
    return selected;
}

/*!
 \brief CalenDayInfo::IsMultipleEventsSelected
*/
bool CalenDayInfo::IsMultipleEventsSelected() const
{
    bool isMultiple(false);
    if (iSelectedAlldayEvent >= 0) {
        // If an allday event is selected, disregard the value of
        // iSelectedSlot
        isMultiple = false;
    }
    else
        if (IsExtraSlot(iSelectedSlot)) {
            int index = iSelectedSlot - iFirstUntimedSlot - iEmptyUntimedSlots;
            isMultiple = (index == 0) && iTodoEvents.count() > 1;
        }
    return isMultiple;
}

/*!
 \brief CalenDayInfo::IsAlldayEventSelected
*/
bool CalenDayInfo::IsAlldayEventSelected() const
{
    bool selected(false);
    if (iSelectedAlldayEvent >= 0) {
        ASSERT( iSelectedAlldayEvent < iAlldayEvents.count() );
        selected = true;
    }
    return selected;
}

/*!
 \brief CalenDayInfo::UntimedEvent
*/
TCalenInstanceId CalenDayInfo::UntimedEvent(int aIndex)
{
    TCalenInstanceId id;
    if (iTodoEvents.count() > 0 && aIndex == 0) {
        id = iTodoEvents[0];
    }
    else {
        // If we have one or more todo items, the first slot is used for them
        // (but not more than one slot)
        if (iTodoEvents.count() > 0) {
            aIndex--;
        }

        if (aIndex >= 0 && aIndex < iUntimedEvents.count()) {
            id = iUntimedEvents[aIndex];
        }
        else {
            // should not happen
            User::Invariant();
        }
    }

    return id;
}

/*!
 \brief CalenDayInfo::AlldayEvent
*/
const CalenTimedEventInfo& CalenDayInfo::AlldayEvent(int aIndex)
{
    ASSERT( aIndex >= 0 && aIndex < iAlldayEvents.count() );
    return iAlldayEvents[aIndex];
}

/*!
 \brief CalenDayInfo::SelectedEvent
*/
TCalenInstanceId CalenDayInfo::SelectedEvent()
{
    TCalenInstanceId id;
    if (iSelectedAlldayEvent >= 0) {
        ASSERT( iSelectedAlldayEvent < iAlldayEvents.count() );
        id = iAlldayEvents[iSelectedAlldayEvent].iId;
    }
    else
        if (iSelectedRegion >= 0) {
            ASSERT( iSelectedRegion < iRegionList.count() );

            CalenTimeRegion& region = iRegionList[iSelectedRegion];
            if (iSelectedColumn < region.iColumns.count()) {
                CalenTimeColumn& column = region.iColumns[iSelectedColumn];
                if (iSelectedColumnEventIndex >= 0 && iSelectedColumnEventIndex
                    < column.iEventArray.count()) {
                    CalenTimedEventInfo& info = column.iEventArray[iSelectedColumnEventIndex];
                    id = info.iId;
                }
                else {
                    // should not happen, no event selected in the column
                    User::Invariant();
                }
            }
            else {
                // should not happen, the empty column was selected
                User::Invariant();
            }
        }
        else
            if (IsExtraSlot(iSelectedSlot)) {
                int index = iSelectedSlot - iFirstUntimedSlot - iEmptyUntimedSlots;
                id = UntimedEvent(index);
            }
            else {
                // should not happen
                User::Invariant();
            }

    return id;
}

/*!
 \brief CalenDayInfo::SelectEvent
 Return KErrNotFound if not found, otherwise KErrNone
*/
int CalenDayInfo::SelectEvent(const TCalenInstanceId& aId)
{
    // Id can be of allday event, untimed event or other events

    // Look for this event id in all event data structures and set
    // the state accordingly if it was found

    for (int i(0); i < iAlldayEvents.count(); i++) {
        TCalenInstanceId id = iAlldayEvents[i].iId;
        if (id == aId) {
            iSelectedAlldayEvent = i;
            iSelectedRegion = KErrNotFound;
            return KErrNone;
        }
    }

    for (int i(0); i < iTodoEvents.count(); i++) {
        TCalenInstanceId id = iTodoEvents[i];
        if (id == aId) {
            iSelectedAlldayEvent = KErrNotFound;
            iSelectedRegion = KErrNotFound;
            iSelectedSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
            return KErrNone;
        }
    }

    for (int i(0); i < iUntimedEvents.count(); i++) {
        TCalenInstanceId id = iUntimedEvents[i];
        if (id == aId) {
            iSelectedAlldayEvent = KErrNotFound;
            iSelectedRegion = KErrNotFound;
            iSelectedSlot = iFirstUntimedSlot + iEmptyUntimedSlots + i;
            if (iTodoEvents.count() > 0) {
                iSelectedSlot++;
            }
            return KErrNone;
        }
    }

    for (int i(0); i < iRegionList.count(); i++) {
        CalenTimeRegion& region = iRegionList[i];
        for (int col(0); col < region.iColumns.count(); col++) {
            for (int ind(0); ind < region.iColumns[col].iEventArray.count(); ind++) {
                if (region.iColumns[col].iEventArray[ind].iId == aId) {
                    iSelectedAlldayEvent = KErrNotFound;
                    iSelectedRegion = i;
                    iSelectedColumn = col;
                    iSelectedColumnEventIndex = ind;
                    SetSelectionInEvent(EMoveDirectionDown);
                    return KErrNone;
                }
            }
        }
    }

    return KErrNotFound;
}

/*!
 \brief CalenDayInfo::EnterRegionFromBelow
*/
void CalenDayInfo::EnterRegionFromBelow()
{
    ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.count() );
    iSelectedColumn = KErrNotFound;
    CalenTimeRegion& region = iRegionList[iSelectedRegion];
    int latestEnd = region.iStartSlot;
    // Look for the column with the latest end
    for (int i = 0; i < region.iColumns.count(); i++) {
        CalenTimeColumn& column = region.iColumns[i];
        ASSERT( column.iEventArray.count()> 0 );
        if (column.iEndSlot > latestEnd) {
            // found a column with an event touching the end of the region
            latestEnd = column.iEndSlot;
            iSelectedColumn = i;
            iSelectedColumnEventIndex = column.iEventArray.count() - 1;
            SetSelectionInEvent(EMoveDirectionUp);
        }
    }
    ASSERT( iSelectedColumn >= 0 );
}

/*!
 \brief CalenDayInfo::EnterRegionFromAbove
*/
void CalenDayInfo::EnterRegionFromAbove()
{
    ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.count() );
    iSelectedColumn = KErrNotFound;
    CalenTimeRegion& region = iRegionList[iSelectedRegion];
    int earliestStart = region.iEndSlot;
    // Look for the column with the earliest start
    for (int i = 0; i < region.iColumns.count(); i++) {
        CalenTimeColumn& column = region.iColumns[i];
        ASSERT( column.iEventArray.count()> 0 );
        if (column.iStartSlot < earliestStart) {
            // found a column with an event
            earliestStart = column.iStartSlot;
            iSelectedColumn = i;
            iSelectedColumnEventIndex = 0;
            SetSelectionInEvent(EMoveDirectionDown);
        }
    }
    ASSERT( iSelectedColumn >= 0 );
}

/*!
 \brief CalenDayInfo::NextFocusArea
 Determines how large area to scan for new events/regions when moving in
 the given direction.  If no events are found in this area, focus an
 empty slot instead
*/
CalenSlotInterval CalenDayInfo::NextFocusArea(const CalenSlotInterval& aInterval, int aDirection)
{
    // NOTE: the comments are written with the visual layout in mind.
    // upwards == up in the display, towards earlier times
    // downwards == down in the display, towards later times
    CalenSlotInterval target;
    target.iEndSlot = 0;
    target.iStartSlot = 0;

    if (IsExtraSlot(aInterval.iStartSlot)) {
        // Special case logic for moving within the extra slots.

        // The index within the extra slot area
        int extraIndex = aInterval.iStartSlot - iFirstUntimedSlot;
        // Generally, move just one slot at a time in this area
        int newIndex = extraIndex + aDirection;

        if (newIndex < iEmptyUntimedSlots) {
            // Moved upwards past the first slot, return
            // the whole last hour before the untimed slot area
            target.iEndSlot = iFirstUntimedSlot;
            target.iStartSlot = target.iEndSlot - iSlotsInHour;
        }
        else
            if (newIndex >= iUntimedSlotCount) {
                // Moved downwards past the last untimed slot, return
                // the whole first hour after the untimed slot area
                target.iStartSlot = iFirstUntimedSlot + iUntimedSlotCount;
                target.iEndSlot = target.iStartSlot + iSlotsInHour;
            }
            else {
                // Moved normally within the untimed slot area
                target.iStartSlot = aInterval.iStartSlot + aDirection;
                target.iEndSlot = target.iStartSlot + 1;
            }
        return target;
    }

    // Helper interval for the whole untimed area
    CalenSlotInterval untimedInterval;
    untimedInterval.iStartSlot = iFirstUntimedSlot;
    untimedInterval.iEndSlot = untimedInterval.iStartSlot + iUntimedSlotCount;

    if (aDirection < 0) {
        // Moving upwards
        // Create a target interval of one hour upwards
        target.iEndSlot = aInterval.iStartSlot;
        target.iStartSlot = aInterval.iStartSlot - iSlotsInHour;

        if (iUntimedSlotCount > 0 && untimedInterval.Overlaps(target)) {
            // The target interval overlaps the untimed area
            if (iUntimedSlotCount > iEmptyUntimedSlots) {
                // Make the interval start at the last untimed slot
                target.iStartSlot = untimedInterval.iEndSlot - 1;
            }
            else {
                // Untimed area containing no actual events.
                // Include one whole hour before the untimed area.
                target.iStartSlot = iFirstUntimedSlot - iSlotsInHour;
            }
            return target;
        }

        // Round the start slot upwards to an even hour
        target.iStartSlot = RoundHourUp(target.iStartSlot);
    }
    else {
        // Moving downwards
        // Create a target interval of one hour downwards
        target.iStartSlot = aInterval.iEndSlot;
        target.iEndSlot = aInterval.iEndSlot + iSlotsInHour;

        if (iUntimedSlotCount > 0 && untimedInterval.Overlaps(target)) {
            // The target interval overlaps the untimed area

            // Assumption: There can't be any regions before the untimed area
            if (iUntimedSlotCount > iEmptyUntimedSlots) {
                // Make the interval end at the first untimed slot containing
                // an event
                target.iStartSlot = untimedInterval.iStartSlot + iEmptyUntimedSlots;
                target.iEndSlot = target.iStartSlot + 1;
            }
            else {
                // Untimed area containing no actual events.
                // Include one whole hour after the untimed area.
                target.iStartSlot = untimedInterval.iStartSlot + iUntimedSlotCount;
                target.iEndSlot = target.iStartSlot + iSlotsInHour;
            }
            return target;
        }

        target.iEndSlot = RoundHourDown(target.iEndSlot);
    }

    return target;
}

/*!
 \brief CalenDayInfo::NextEmptyFocusSlot
 If no events are found in an empty area scanned, focus this slot instead
*/
int CalenDayInfo::NextEmptyFocusSlot(const CalenSlotInterval& aInterval, int aDirection)
{
    // NOTE: the comments are written with the visual layout in mind.
    // upwards == up in the display, towards earlier times
    // downwards == down in the display, towards later times

    int target(0);

    // If the next focus area logic says we should go to an untimed slot,
    // skip the rest of this method
    CalenSlotInterval testInterval = NextFocusArea(aInterval, aDirection);
    if (IsExtraSlot(testInterval.iStartSlot)) {
        target = testInterval.iStartSlot;
    }
    else {
        if (aDirection < 0) {
            // Moving upwards, target slot is the start of the area returned by
            // NextFocusArea. This always is an even hour.
            target = testInterval.iStartSlot;
            ASSERT( IsHourStartSlot( target ) );
        }
        else {
            target = testInterval.iStartSlot;
            if (!IsHourStartSlot(target)) {
                // If the interval doesn't start on an even hour, round downwards.
                target = RoundHourDown(target);
            }
        }
    }

    return target;
}

/*!
 \brief CalenDayInfo::EmptySelectionInterval
 Create an interval representation of the current focus area
*/
CalenSlotInterval CalenDayInfo::EmptySelectionInterval()
{
    CalenSlotInterval target;
    target.iEndSlot = 0;
    target.iStartSlot = 0;

    if (IsExtraSlot(iSelectedSlot)) {
        // Return an interval consisting of one single slot
        target.iStartSlot = iSelectedSlot;
        target.iEndSlot = target.iStartSlot + 1;
    }
    else {
        // Round the start slot to an even hour if it isn't.
        // NOTE: This actually updates the selection state!
        // Only call this method if no event actually is selected.
        if (!IsHourStartSlot(iSelectedSlot)) {
            iSelectedSlot = RoundHourUp(iSelectedSlot);
        }
        target.iStartSlot = iSelectedSlot;
        target.iEndSlot = iSelectedSlot + iSlotsInHour;
    }

    return target;
}

/*!
 \brief CalenDayInfo::SelectedInterval
*/
CalenSlotInterval CalenDayInfo::SelectedInterval()
{
    CalenSlotInterval selection;
    selection.iStartSlot = selection.iEndSlot = 0;

    if (iSelectedAlldayEvent >= 0) {
        selection.iStartSlot = 0;
        selection.iEndSlot = iSlotsInHour * KCalenHoursInDay + iUntimedSlotCount;
    }
    else
        if (iSelectedRegion >= 0) {

            CalenTimeRegion& region = iRegionList[iSelectedRegion];
            if (iSelectedColumn < region.iColumns.count()) {
                CalenTimeColumn& column = region.iColumns[iSelectedColumn];
                if (iSelectedColumnEventIndex >= 0 && iSelectedColumnEventIndex
                    < column.iEventArray.count()) {
                    CalenTimedEventInfo& info = column.iEventArray[iSelectedColumnEventIndex];
                    // gets only the CalenSlotInterval part from the CalenTimedEventInfo struct
                    selection = info;
                }
            }
        }

    if (selection.IsEmpty()) {
        // No selection was set in the cases above, no event is selected, thus
        // use the empty area selection interval instead.
        selection = EmptySelectionInterval();
    }
    return selection;
}

/*!
 \brief CalenDayInfo::SetSelectionInRegion
 Sets selection within a region
*/
bool CalenDayInfo::SetSelectionInRegion(int aRegion, int aColumn, int aSlot)
{
    bool selected(false);
    StoreOrigSelection();
    //validate given values
    if (aRegion >= 0 && aRegion < iRegionList.count()) {//ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.count() );
        CalenSlotInterval interval;
        //TODO: round aSlot hour to full hour down????
        interval.iStartSlot = aSlot;
        interval.iEndSlot = interval.iStartSlot + 1;

        CalenTimeRegion& region = iRegionList[aRegion];
        if (region.Overlaps(interval)) {
            if (aColumn >= 0 && aColumn < region.iColumns.count()) {
                bool eventFound(false);
                CalenTimeColumn& column = region.iColumns[aColumn];
                if (column.Overlaps(interval)) {
                    ASSERT( column.iEventArray.count()> 0 );
                    for (int i = 0; i < column.iEventArray.count(); i++) {
                        CalenTimedEventInfo& event = column.iEventArray[i];
                        if (event.Overlaps(interval)) {
                            iSelectedColumnEventIndex = i;

                            iSelectedAlldayEvent = KErrNotFound;
                            iSelectedRegion = aRegion;
                            iSelectedColumn = aColumn;
                            iSelectedSlot = aSlot;
                            selected = true;
                            eventFound = true;
                            break;
                        }

                    }
                }

                if (!eventFound) {
                    iSelectedAlldayEvent = KErrNotFound;
                    iSelectedRegion = aRegion;
                    iSelectedColumn = aColumn;
                    iSelectedSlot = aSlot;
                    iSelectedColumnEventIndex = KErrNotFound;
                    selected = true;
                }
            }
            else
                if (aColumn == iRegionList[aRegion].iColumns.count()) {//empty selection on right selected
                    iSelectedAlldayEvent = KErrNotFound;
                    iSelectedRegion = aRegion;
                    iSelectedColumn = aColumn;
                    iSelectedSlot = aSlot;
                    iSelectedColumnEventIndex = KErrNotFound;
                    selected = true;
                }
        }
    }

    return selected;
}

/*!
 \brief CalenDayInfo::MoveOutFromRegion
*/
void CalenDayInfo::MoveOutFromRegion(int aDirection)
{
    CalenTimeRegion& region = iRegionList[iSelectedRegion];
    CalenSlotInterval targetInterval = NextFocusArea(region, aDirection);
    int next = iSelectedRegion + aDirection;
    // Check what the next region would be and if we should move into that
    // or into an empty area
    if (next >= 0 && next < iRegionList.count()) {
        if (iRegionList[next].Overlaps(targetInterval)) {
            iSelectedRegion = next;
            if (aDirection < 0) {
                EnterRegionFromBelow();
            }
            else {
                EnterRegionFromAbove();
            }
        }
        else {
            iSelectedRegion = KErrNotFound;
            iSelectedSlot = NextEmptyFocusSlot(region, aDirection);
        }
    }
    else {
        iSelectedRegion = KErrNotFound;
        iSelectedSlot = NextEmptyFocusSlot(region, aDirection);
    }
}

/*!
 \brief CalenDayInfo::MoveInColumn
*/
void CalenDayInfo::MoveInColumn(int aDirection)
{
    CalenTimeRegion& region = iRegionList[iSelectedRegion];
    if (iSelectedColumn < region.iColumns.count()) {
        CalenTimeColumn& column = region.iColumns[iSelectedColumn];
        if (iSelectedColumnEventIndex >= 0) {
            // We currently have an event in a column selected
            CalenSlotInterval curInterval = column.iEventArray[iSelectedColumnEventIndex];
            CalenSlotInterval targetInterval = NextFocusArea(curInterval, aDirection);

            int next = iSelectedColumnEventIndex + aDirection;
            // Check if the target interval overlaps the next event in this column
            if (next >= 0 && next < column.iEventArray.count()) {
                if (column.iEventArray[next].Overlaps(targetInterval)) {
                    iSelectedColumnEventIndex = next;
                    SetSelectionInEvent(aDirection);
                }
                else {
                    iSelectedColumnEventIndex = KErrNotFound;
                    iSelectedSlot = NextEmptyFocusSlot(curInterval, aDirection);
                }
            }
            else {
                // There's no next event in this column, focus an empty area instead.
                iSelectedColumnEventIndex = KErrNotFound;
                iSelectedSlot = NextEmptyFocusSlot(curInterval, aDirection);
            }
        }
        else {
            // No event selected in the current column, find the target interval
            // and check if there's any event there
            CalenSlotInterval curInterval = EmptySelectionInterval();
            CalenSlotInterval targetInterval = NextFocusArea(curInterval, aDirection);
            iSelectedColumnEventIndex = FindEvent(targetInterval, aDirection);
            if (iSelectedColumnEventIndex >= 0) {
                SetSelectionInEvent(aDirection);
            }
            else {
                iSelectedSlot = NextEmptyFocusSlot(curInterval, aDirection);
            }
        }

        // If the new selection lies outside of the region, move out from the region.
        CalenSlotInterval selection = SelectedInterval();
        if (selection.iStartSlot < region.iStartSlot || selection.iEndSlot > region.iEndSlot) {
            MoveOutFromRegion(aDirection);
        }

    }
    else {
        // The empty column was selected, just move up/down in that column
        iSelectedSlot = NextEmptyFocusSlot(EmptySelectionInterval(), aDirection);

        // When moving in the empty column, move out from the region only if
        // the selection moved completely out from the region.
        CalenSlotInterval selection = SelectedInterval();
        if (!selection.Overlaps(region)) {
            // The target selection is completely outside of the current region
            int next = iSelectedRegion + aDirection;
            if (next >= 0 && next < iRegionList.count() && iRegionList[next].Overlaps(selection)) {
                // The target selection overlaps the next region, move into that,
                // stay within the rightmost empty column
                iSelectedRegion = next;
                iSelectedColumn = iRegionList[next].iColumns.count();
                iSelectedColumnEventIndex = KErrNotFound;
            }
            else {
                // Move out from the current region
                iSelectedRegion = KErrNotFound;
                iSelectedColumn = KErrNotFound;
                iSelectedColumnEventIndex = KErrNotFound;
                // iSelectedSlot was update above, keep the same value
            }
        }

    }

}

/*!
 \brief CalenDayInfo::MoveInAlldayEvent
*/
bool CalenDayInfo::MoveInAlldayEvent(TScrollDirection aDirection)
{
    ASSERT( iSelectedAlldayEvent >= 0 );
    bool moved(true);
    switch (aDirection) {
        case EScrollUp: {
            // don't do anything
        }
            break;
        case EScrollDown: {
            // don't do anything
        }
            break;
        case EScrollLeft: {
            if (iSelectedAlldayEvent > 0) {
                // there are more allday events to the left, select next
                iSelectedAlldayEvent--;
            }
            else {
                // move to previous day
                moved = false;
            }
        }
            break;
        case EScrollRight: {
            if (iSelectedAlldayEvent < iAlldayEvents.count() - 1) {
                // there are more allday events to the right, select next
                iSelectedAlldayEvent++;
            }
            else {
                iSelectedAlldayEvent = KErrNotFound;
                // find a region to select
                if (iSelectedRegion >= 0) {
                    // keep using the old selection state
                }
                else {
                    // find a new suitable selection

                    // if iSelectedSlot was moved to the empty untimed slot area,
                    // move it down to a real slot

                    if (iSelectedSlot >= iFirstUntimedSlot && iSelectedSlot < iFirstUntimedSlot
                        + iEmptyUntimedSlots) {
                        iSelectedSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
                    }

                    CalenSlotInterval interval = EmptySelectionInterval();

                    iSelectedRegion = FindRegion(interval, EMoveDirectionDown);
                    int firstRegion = iSelectedRegion;
                    bool found(false);
                    // Iterate over all regions overlapping this interval, see
                    // if any of them has events in this column
                    while (iSelectedRegion >= 0 && iSelectedRegion < iRegionList.count()
                        && iRegionList[iSelectedRegion].Overlaps(interval) && !found) {

                        ASSERT( iSelectedRegion < iRegionList.count() );
                        CalenTimeRegion& region = iRegionList[iSelectedRegion];

                        ASSERT( region.iColumns.count()> 0 );
                        iSelectedColumn = 0;
                        CalenTimeColumn& column = region.iColumns[iSelectedColumn];

                        iSelectedColumnEventIndex = FindEvent(interval, EMoveDirectionDown);
                        if (iSelectedColumnEventIndex >= 0) {
                            ASSERT( iSelectedColumnEventIndex < column.iEventArray.count() );
                            // Keep using the old selected slot also here, just
                            // update it to make sure it's within the actual event
                            UpdateSelectionInEvent();
                            found = true;
                        }
                        else {
                            // Check the next region
                            iSelectedRegion++;
                        }
                    }

                    if (!found) {
                        iSelectedRegion = firstRegion;
                        iSelectedColumn = 0;
                        iSelectedSlot = interval.iStartSlot;
                    }

                }
            }
        }
            break;
        default:
            // Do nothing
            break;
    }

    return moved;
}

/*!
 \brief CalenDayInfo::MoveBetweenColumns
*/
void CalenDayInfo::MoveBetweenColumns(TScrollDirection aDirection)
{
    // Moving to another column
    CalenSlotInterval selection = SelectedInterval();
    if (IsEventSelected()) {
        // If moving from an event, just use the selected
        // slot pos within the event, not the whole event
        // interval.
        selection.iStartSlot = iSelectedSlot;
        selection.iEndSlot = selection.iStartSlot + iSlotsInHour;
        // Crop the selection to be within the event
        selection.Intersect(SelectedInterval());
    }

    switch (aDirection) {
        case EScrollLeft: {
            iSelectedColumn--;
            break;
        }
        case EScrollRight: {
            iSelectedColumn++;
            break;
        }
        default:
            // Do nothing
            break;
    }

    // First try finding events exactly in the selection area
    iSelectedColumnEventIndex = FindEvent(selection, EMoveDirectionDown);
    if (iSelectedColumnEventIndex >= 0) {
        // An event was found in the given interval
        // Keep using the old selection position, just update it to make
        // sure it's within the event
        UpdateSelectionInEvent();
    }
    else {
        // No event found in the interval

        // Expand the interval to even hour endings
        selection.iStartSlot = RoundHourUp(selection.iStartSlot);
        selection.iEndSlot = RoundHourDown(selection.iEndSlot);
        CalenTimeRegion& region = iRegionList[iSelectedRegion];
        // Crop the selection to the current region
        selection.Intersect(region);

        // Try again with the possibly larger interval
        iSelectedColumnEventIndex = FindEvent(selection, EMoveDirectionDown);
        if (iSelectedColumnEventIndex >= 0) {
            // An event was found in the given interval
            // Keep using the old selection position, just update it to make
            // sure it's within the event
            UpdateSelectionInEvent();
        }
        else {
            // Still no events found. Select an empty interval if possible.

            if (!IsHourStartSlot(selection.iStartSlot)) {
                // The starting slot isn't a hour start slot. Rounding directly
                // would move the interval outside of the region. See if we can
                // round the starting slot down instead and still be within
                // the region.
                int start = RoundHourDown(selection.iStartSlot);
                if (start + iSlotsInHour <= region.iEndSlot) {
                    // Ok, an empty selection fits within the region.
                    iSelectedSlot = start;
                }
            }
            else
                if (!IsHourStartSlot(selection.iEndSlot)) {
                    // The ending slot isn't a hour start slot. Rounding directly
                    // could move the interval outside of the region. See if we can
                    // round the upwards instead and still be within
                    // the region.
                    int start = RoundHourUp(selection.iEndSlot - iSlotsInHour);
                    if (start >= region.iStartSlot) {
                        // Ok, an empty selection fits within the region.
                        iSelectedSlot = start;
                    }
                }

            // Make sure the selected interval really is a valid
            // empty interval selection (this forces rounding if needed)
            selection = EmptySelectionInterval();

            // Check the final target interval once more.
            iSelectedColumnEventIndex = FindEvent(selection, EMoveDirectionDown);
            if (iSelectedColumnEventIndex >= 0) {
                // An event was found in the given interval
                UpdateSelectionInEvent();
            }
            else {
                // No even found, use the empty selection

                // If not within the region, move out
                if (selection.iStartSlot < region.iStartSlot) {
                    MoveOutFromRegion(EMoveDirectionUp);
                }
                else
                    if (selection.iEndSlot > region.iEndSlot) {
                        MoveOutFromRegion(EMoveDirectionDown);
                    }
            }
        }

    }

}

/*!
 \brief CalenDayInfo::MoveInRegion
*/
bool CalenDayInfo::MoveInRegion(TScrollDirection aDirection)
{
    ASSERT( iSelectedAlldayEvent < 0 && iSelectedRegion >= 0 );
    bool moved(true);
    switch (aDirection) {
        case EScrollUp: {
            MoveInColumn(EMoveDirectionUp);
        }
            break;
        case EScrollDown: {
            MoveInColumn(EMoveDirectionDown);
        }
            break;

        case EScrollLeft: {
            ASSERT( iSelectedRegion < iRegionList.count() );
            CalenTimeRegion& region = iRegionList[iSelectedRegion];
            ASSERT( iSelectedColumn >= 0 && iSelectedColumn <= region.iColumns.count() );
            if (iSelectedColumn == 0) {
                if (iAlldayEvents.count() > 0) {
                    // remember the last selection state
                    iSelectedAlldayEvent = iAlldayEvents.count() - 1;
                }
                else {
                    // move to previous day
                    moved = false;
                }
            }
            else {
                MoveBetweenColumns(aDirection);
            }
        }
            break;

        case EScrollRight: {
            ASSERT( iSelectedRegion < iRegionList.count() );
            CalenTimeRegion& region = iRegionList[iSelectedRegion];
            if (iSelectedColumn < region.iColumns.count() - 1) {
                MoveBetweenColumns(aDirection);
            }
            else
                if (iSelectedColumn == region.iColumns.count() - 1) {
                    // In the last column, moving to the empty column
                    iSelectedColumn++;
                    iSelectedColumnEventIndex = KErrNotFound;

                    // Round the selection to an empty interval
                    EmptySelectionInterval();
                }
                else {
                    // in the last, empty column
                    // move to next day
                    moved = false;
                }
        }
            break;

        default:
            // Do nothing
            break;
    }

    return moved;
}

/*!
 \brief CalenDayInfo::MoveInEmptyArea
*/
bool CalenDayInfo::MoveInEmptyArea(TScrollDirection aDirection)
{
    ASSERT( iSelectedAlldayEvent < 0 && iSelectedRegion < 0 );
    bool moved(true);
    switch (aDirection) {
        case EScrollUp: {
            CalenSlotInterval curInterval = EmptySelectionInterval();
            CalenSlotInterval targetInterval = NextFocusArea(curInterval, EMoveDirectionUp);
            iSelectedRegion = FindRegion(targetInterval, EMoveDirectionUp);
            if (iSelectedRegion >= 0) {
                EnterRegionFromBelow();
            }
            else {
                iSelectedSlot = NextEmptyFocusSlot(curInterval, EMoveDirectionUp);
            }

        }
            break;
        case EScrollDown: {
            CalenSlotInterval curInterval = EmptySelectionInterval();
            CalenSlotInterval targetInterval = NextFocusArea(curInterval, EMoveDirectionDown);
            iSelectedRegion = FindRegion(targetInterval, EMoveDirectionDown);
            if (iSelectedRegion >= 0) {
                EnterRegionFromAbove();
            }
            else {
                iSelectedSlot = NextEmptyFocusSlot(curInterval, EMoveDirectionDown);
            }
        }
            break;
        case EScrollLeft: {
            if (iAlldayEvents.count() > 0) {
                iSelectedAlldayEvent = iAlldayEvents.count() - 1;
            }
            else {
                // move to previous day
                moved = false;
            }
        }
            break;
        case EScrollRight: {
            // move to next day
            moved = false;
        }
            break;
        default:
            // Do nothing
            break;
    }

    return moved;
}

/*!
 \brief CalenDayInfo::MoveSelection
*/
bool CalenDayInfo::MoveSelection(TScrollDirection aDirection)
{
    bool moved(true);
    StoreOrigSelection();
    if (iSelectedAlldayEvent >= 0) {
        // focused on an allday event
        moved = MoveInAlldayEvent(aDirection);
    }
    else
        if (iSelectedRegion >= 0) {
            // focused on some event region
            moved = MoveInRegion(aDirection);
        }
        else {
            // focused on an empty, plain area
            moved = MoveInEmptyArea(aDirection);
        }
    if (!ValidateSelection()) {
        moved = false;
    }
    return moved;
}

/*!
 \brief CalenDayInfo::MoveSelectionInEvent
*/
void CalenDayInfo::MoveSelectionInEvent(TScrollDirection aDirection)
{
    ASSERT( IsEventSelected() );

    switch (aDirection) {
        case EScrollUp: {
            iSelectedSlot -= KFSCalSlotsInHour;
        }
            break;
        case EScrollDown: {
            iSelectedSlot += KFSCalSlotsInHour;
        }
            break;

        default:
            // Do nothing
            break;
    }

    UpdateSelectionInEvent();

    if (iSelectedAlldayEvent >= 0) {
        // if an allday event is selected, invalidate
        // the old region/event selection
        iSelectedRegion = KErrNotFound;
    }
}

/*!
 \brief CalenDayInfo::UpdateSelectionInEvent
*/
void CalenDayInfo::UpdateSelectionInEvent()
{
    ASSERT( IsEventSelected() );
    CalenSlotInterval event = SelectedInterval();

    if (iSelectedSlot < event.iStartSlot) {
        iSelectedSlot = event.iStartSlot;
    }
    if (iSelectedSlot >= event.iEndSlot) {
        iSelectedSlot = event.iEndSlot - 1;
    }

}

/*!
 \brief CalenDayInfo::SetSelectionInEvent
*/
void CalenDayInfo::SetSelectionInEvent(int aDirection)
{
    CalenSlotInterval event = SelectedInterval();
    if (aDirection > 0) {
        iSelectedSlot = event.iStartSlot;
    }
    else {
        iSelectedSlot = event.iEndSlot - 1;
    }
}

/*!
 \brief CalenDayInfo::StoreOrigSelection
*/
void CalenDayInfo::StoreOrigSelection()
{
    iOrigSelectedAlldayEvent = iSelectedAlldayEvent;
    iOrigSelectedRegion = iSelectedRegion;
    iOrigSelectedColumn = iSelectedColumn;
    iOrigSelectedSlot = iSelectedSlot;
    iOrigSelectedColumnEventIndex = iSelectedColumnEventIndex;
}

/*!
 \brief CalenDayInfo::ValidateSelection
*/
bool CalenDayInfo::ValidateSelection()
{
    bool validate(true);
    if (iSelectedSlot < 0 || iSelectedSlot >= iSlotsInHour * KCalenHoursInDay + iUntimedSlotCount) {
        // tried to move outside of the day
        // revert to the original coords
        iSelectedAlldayEvent = iOrigSelectedAlldayEvent;
        iSelectedRegion = iOrigSelectedRegion;
        iSelectedColumn = iOrigSelectedColumn;
        iSelectedSlot = iOrigSelectedSlot;
        iSelectedColumnEventIndex = iOrigSelectedColumnEventIndex;
        validate = false;
    }
    return validate;
}

/*!
 \brief CalenDayInfo::SelectSlot
*/
void CalenDayInfo::SelectSlot(int aSlot)
{
    // Check for special case: empty untimed slot is selected
    if (aSlot >= iFirstUntimedSlot && aSlot < iFirstUntimedSlot + iEmptyUntimedSlots) {
        aSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
    }

    iSelectedAlldayEvent = KErrNotFound;
    iSelectedSlot = aSlot;
    CalenSlotInterval interval = EmptySelectionInterval();
    iSelectedRegion = FindRegion(interval, EMoveDirectionDown);
    if (iSelectedRegion >= 0) {
        CalenTimeRegion& region = iRegionList[iSelectedRegion];
        int regCount(region.iColumns.count());
        for (int i = 0; i < regCount; i++) {
            CalenTimeColumn& column = region.iColumns[i];
            int colCount(column.iEventArray.count());
            for (int j = 0; j < colCount; j++) {
                if (column.iEventArray[j].Overlaps(interval)) {
                    iSelectedColumn = i;
                    iSelectedColumnEventIndex = j;
                    iSelectedSlot = aSlot;
                    UpdateSelectionInEvent();
                    return;
                }
            }
        }
        // Regions can have empty areas where there aren't any events,
        // due to rounding of the ends of the region to whole hours.
        // A region cannot, however, have a empty complete whole hour.
        // Therefore, this cannot occur.
        User::Invariant();
    }
}

/*!
 \brief CalenDayInfo::RegionList
*/
const QList<CalenTimeRegion>& CalenDayInfo::RegionList() const
{
    return iRegionList;
}

/*!
 \brief CalenDayInfo::GetEventIntervals
*/
void CalenDayInfo::GetEventIntervals(QList<CalenEventInterval>& aArray) const
{
    int regCount(iRegionList.count());
    for (int i = 0; i < regCount; i++) {
        const CalenTimeRegion& region = iRegionList[i];
        for (int j = 0; j < region.iIntervals.count(); j++) {
            aArray.append(region.iIntervals[j]);
        }
    }
}

/*!
 \brief CalenDayInfo::InsertUntimedEventL
*/
/*
void CalenDayInfo::InsertUntimedEventL(const CCalInstance& aInstance)
{
    if (CalenAgendaUtils::IsTimedEntryL(aInstance.Entry().EntryTypeL())) {
        // timed entry added as untimed event
        User::Invariant();
    }

    TCalenInstanceId id = TCalenInstanceId::CreateL(aInstance);
    InsertUntimedEvent(aInstance.Entry().EntryTypeL(), id);
}
*/

/*!
 \brief CalenDayInfo::InsertUntimedEvent
*/
void CalenDayInfo::InsertUntimedEvent(AgendaEntry::Type aType, const TCalenInstanceId& aId)
{
    if (aType == AgendaEntry::TypeTodo) {
        iTodoEvents.append(aId);
    }
    else {
        iUntimedEvents.append(aId);
    }
}

/*!
 \brief CalenDayInfo::SuggestedUntimedSlotPos
*/
int CalenDayInfo::SuggestedUntimedSlotPos()
{
    int slot = KFSCalStartingHour * iSlotsInHour;
    if (iRegionList.count() > 0) {
        CalenTimeRegion& region = iRegionList[0];
        if (region.iStartSlot < slot) {
            slot = RoundHourUp(region.iStartSlot);
        }
    }
    return slot;
}

/*!
 \brief CalenDayInfo::NeededUntimedSlotCount
*/
int CalenDayInfo::NeededUntimedSlotCount()
{
    int count = iUntimedEvents.count();
    if (iTodoEvents.count() > 0) {
        count++;
    }
    return count;
}

/*!
 \brief CalenDayInfo::NeededUntimedSlotCount
*/
int CalenDayInfo::UpdateUntimedPos(int aSlot, int aUntimedCount)
{
    int newValue = NeededUntimedSlotCount();

    // If this method is called many times, first undo the previous modifications
    int regCount(iRegionList.count());
    for (int i = 0; i < regCount; i++) {
        iRegionList[i].AddOffset(-iUntimedSlotCount, iFirstUntimedSlot);
    }
    if (iSelectedSlot >= iFirstUntimedSlot) {
        iSelectedSlot -= iUntimedSlotCount;
    }

    // Reset the untimed slot count
    iUntimedSlotCount = 0;

    // Get the default values
    iFirstUntimedSlot = SuggestedUntimedSlotPos();
    iUntimedSlotCount = newValue;

    // If parameters were given, use them instead of the defaults
    if (aSlot >= 0) {
        ASSERT( aSlot <= iFirstUntimedSlot );
        ASSERT( iUntimedSlotCount <= aUntimedCount );

        iFirstUntimedSlot = aSlot;
        iUntimedSlotCount = aUntimedCount;
    }

    iEmptyUntimedSlots = iUntimedSlotCount - NeededUntimedSlotCount();

    // Add the new offset to all regions and update the selected slot
    regCount = iRegionList.count();
    for (int i = 0; i < regCount; i++) {
        iRegionList[i].AddOffset(iUntimedSlotCount, iFirstUntimedSlot);
    }
    if (iSelectedSlot >= iFirstUntimedSlot) {
        iSelectedSlot += iUntimedSlotCount;
    }
    int eventCount(iAlldayEvents.count());
    for (int i = 0; i < eventCount; i++) {
        iAlldayEvents[i].iEndSlot = iSlotsInHour * KCalenHoursInDay + iUntimedSlotCount;
    }

    // Do rounding of the end of the last region. This should be done when no
    // more events will be added, but there's no real harm if new events are
    // added after this.
    if (iRegionList.count() > 0) {
        // Round the end of the last region down
        CalenTimeRegion& lastRegion = iRegionList[iRegionList.count() - 1];
        lastRegion.iEndSlot = RoundHourDown(lastRegion.iEndSlot);
    }

    return iFirstUntimedSlot;
}

/*!
 \brief Returns the index of the first occupied slot
*/
int CalenDayInfo::FirstOccupiedSlot()
{

    int firstNonemptySlot(KErrNotFound);

    if (iUntimedSlotCount > 0) {
        firstNonemptySlot = iFirstUntimedSlot;
    }
    else {
        if (iRegionList.count() > 0) {
            CalenTimeRegion& region = iRegionList[0];
            firstNonemptySlot = region.iStartSlot;
        }
    }

    return firstNonemptySlot;
}

/*!
 \brief Returns the index of the last occupied slot
*/
int CalenDayInfo::LastOccupiedSlot()
{
    int lastNonemptySlot(KErrNotFound);
    if (iRegionList.count() > 0) {
        int lastIndex = iRegionList.count() - 1;
        CalenTimeRegion& region = iRegionList[lastIndex];
        lastNonemptySlot = region.iEndSlot;
    }
    else {
        if (iUntimedSlotCount > 0) {
            lastNonemptySlot = iFirstUntimedSlot + iUntimedSlotCount - 1;
        }
    }
    return lastNonemptySlot;
}

/*!
 \brief Returns the index of the earliest end slot
*/
int CalenDayInfo::EarliestEndSlot()
{
    int earliestEndSlot(KErrNotFound);
    int untimedEventCount = iUntimedSlotCount - iEmptyUntimedSlots;
    if (untimedEventCount > 0) {
        earliestEndSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
        // add 1, since end slot is actually the one that is right after..
        earliestEndSlot++;
    }
    else {
        if (iRegionList.count() > 0) {
            earliestEndSlot = iEarliestEndSlot + iEmptyUntimedSlots;
        }
    }
    return earliestEndSlot;
}

/*!
 \brief Returns the index of the last start slot
*/
int CalenDayInfo::LastStartSlot()
{
    int lastStartSlot(KErrNotFound);

    if (iRegionList.count() > 0) {
        lastStartSlot = iLastStartSlot + iUntimedSlotCount;
    }
    else {
        if (iUntimedSlotCount - iEmptyUntimedSlots > 0) {
            lastStartSlot = iFirstUntimedSlot + iUntimedSlotCount - 1;
        }
    }
    return lastStartSlot;
}

/*!
 \brief Returns the index of a slot where this event should start drawing,
 based on the start time
*/
int CalenDayInfo::SlotIndexForStartTime(QDateTime aStartTime)
{
    // For example, 1/2 hour accuracy (iSlotsInHour == 2):
    // Start drawing with the previus half hour: 9:15 -> 9:00, 9:59-> 9:30.


    //    TDateTime dt = aStartTime.DateTime();
    //    int minutes = dt.Minute();
    //    int hours = dt.Hour();
    int minutes = aStartTime.time().minute();//dt.Minute();
    int hours = aStartTime.time().hour();//dt.Hour();

    // calculate index based on the hour.
    int slotIndex = hours * iSlotsInHour;

    switch (iSlotsInHour) {
        case EOne: // do nothing
            break;
        case ETwo: {
            if (minutes >= 30) {
                slotIndex++;
            }
        }
            break;
        case EThree: {
            if (minutes >= 20) {
                slotIndex++;
            }
            if (minutes >= 40) {
                slotIndex++;
            }
        }
            break;
        case EFour: // followthrough
        default: {
            if (minutes >= 15) {
                slotIndex++;
            }
            if (minutes >= 30) {
                slotIndex++;
            }
            if (minutes >= 45) {
                slotIndex++;
            }

        }
    }

    if (slotIndex >= iFirstUntimedSlot) {
        slotIndex += iUntimedSlotCount;
    }
    return slotIndex;
}

/*!
 \brief Returns the index of a slot where this event should end drawing,
 based on the end time
*/
int CalenDayInfo::SlotIndexForEndTime(QDateTime aEndTime)
{
    // For example, 1/2 hour accuracy (iSlotsInHour == 2):
    // End drawing with the next half hour: 9:10 -> 9:30, 9:59-> 10:00.

    //    TDateTime dt = aEndTime.DateTime();
    //    int minutes = dt.Minute();
    //    int hours = dt.Hour();
    int minutes = aEndTime.time().minute();
    int hours = aEndTime.time().hour();

    // calculate index based on the hour.
    int slotIndex = hours * iSlotsInHour;

    switch (iSlotsInHour) {
        case EOne: {
            if (minutes > 0) {
                slotIndex++;
            }
        }

            break;
        case ETwo: {
            if (minutes > 0) {
                slotIndex++;
            }
            if (minutes > 30) {
                slotIndex++;
            }
        }
            break;
        case EThree: {
            if (minutes > 0) {
                slotIndex++;
            }
            if (minutes > 20) {
                slotIndex++;
            }
            if (minutes > 40) {
                slotIndex++;
            }
        }
            break;
        case EFour: // followthrough
        default: {
            if (minutes > 0) {
                slotIndex++;
            }

            if (minutes > 15) {
                slotIndex++;
            }
            if (minutes > 30) {
                slotIndex++;
            }
            if (minutes > 45) {
                slotIndex++;
            }

        }
    }

    if (slotIndex >= iFirstUntimedSlot) {
        slotIndex += iUntimedSlotCount;
    }

    return slotIndex;

}

/*!
 \brief CalenDayInfo::IsHourStartSlot
*/
bool CalenDayInfo::IsHourStartSlot(const int& aSlotIndex) const
{
    bool isHourStartSlot(false);
    if (IsExtraSlot(aSlotIndex)) {
        isHourStartSlot = false;
    }
    else {
        int hourIndex(aSlotIndex);
        if (aSlotIndex >= iFirstUntimedSlot + iUntimedSlotCount) {
            hourIndex -= iUntimedSlotCount;
        }
        int hour = hourIndex / iSlotsInHour;
        int startIndOfHour = hour * iSlotsInHour;
        if (hourIndex - startIndOfHour > 0) {
            isHourStartSlot = false;
        }
        else {
            isHourStartSlot = true;
        }
    }
    return isHourStartSlot;

}

/*!
 \brief CalenDayInfo::IsExtraSlot
*/
bool CalenDayInfo::IsExtraSlot(const int& aSlotIndex) const
{
    return (aSlotIndex >= iFirstUntimedSlot) && (aSlotIndex < (iFirstUntimedSlot
        + iUntimedSlotCount));
}

/*!
 \brief CalenDayInfo::HourFromSlotIndex
 Returns the corresponding hour, or KErrNone if index is not hour slot index
*/
int CalenDayInfo::HourFromSlotIndex(const int& aSlotInd) const
{

    int hour(KErrNotFound);

    if (!IsExtraSlot(aSlotInd)) {

        if (aSlotInd < 0) {
            // round downwards, to the previous starting hour
            // e.g. iSlotsInHour = 2, aSlotInd = -1, hour = -1,
            // which by SlotIndexFromHour corresponds to slot -2
            hour = (aSlotInd - iSlotsInHour + 1) / iSlotsInHour;
        }
        else
            if (aSlotInd < iFirstUntimedSlot) {
                hour = aSlotInd / iSlotsInHour;
            }
            else
                if (aSlotInd >= iFirstUntimedSlot + iUntimedSlotCount) {
                    hour = (aSlotInd - iUntimedSlotCount) / iSlotsInHour;
                }
    }
    return hour;
}

/*!
 \brief CalenDayInfo::SlotIndexFromHour
 Returns the corresponding hour, or KErrNone if index is not hour slot index
*/
int CalenDayInfo::SlotIndexFromHour(int aHour)
{

    int slotIndex(KErrNotFound);

    if (aHour >= iFirstUntimedSlot / iSlotsInHour) {
        slotIndex = aHour * iSlotsInHour + iUntimedSlotCount;
    }
    else {
        slotIndex = aHour * iSlotsInHour;
    }

    return slotIndex;
}

/*!
 \brief CalenDayInfo::RoundHourUp
 Rounds the slot number up (towards earlier hours) to an even hour
*/
int CalenDayInfo::RoundHourUp(int aSlot)
{
    if (!IsExtraSlot(aSlot)) {
        aSlot = SlotIndexFromHour(HourFromSlotIndex(aSlot));
    }
    return aSlot;
}

/*!
 \brief CalenDayInfo::RoundHourDown
 Rounds the slot number down (towards later hours) to an even hour
*/
int CalenDayInfo::RoundHourDown(int aSlot)
{
    if (!IsExtraSlot(aSlot) && !IsHourStartSlot(aSlot)) {
        aSlot = SlotIndexFromHour(HourFromSlotIndex(aSlot + iSlotsInHour));
    }
    return aSlot;
}

/*!
 \brief CalenSlotInterval::Overlaps
*/
bool CalenSlotInterval::Overlaps(const CalenSlotInterval& aInterval) const
{
    return aInterval.iStartSlot < iEndSlot && aInterval.iEndSlot > iStartSlot;
}

/*!
 \brief CalenSlotInterval::AddOffset
*/
void CalenSlotInterval::AddOffset(int aOffset, int aPos)
{
    if (iStartSlot >= aPos) {
        iStartSlot += aOffset;
    }
    if (iEndSlot >= aPos) {
        iEndSlot += aOffset;
    }
}

/*!
 \brief CalenSlotInterval::Union
*/
void CalenSlotInterval::Union(const CalenSlotInterval& aInterval)
{
    if (!aInterval.IsEmpty()) {
        iStartSlot = Min(iStartSlot, aInterval.iStartSlot);
        iEndSlot = Max(iEndSlot, aInterval.iEndSlot);
    }
}

/*!
 \brief CalenSlotInterval::Adjacent
*/
bool CalenSlotInterval::Adjacent(const CalenSlotInterval& aInterval) const
{
    bool adjacent(false);
    if (Overlaps(aInterval)) {
        adjacent = true;
    }
    else {
        adjacent = iStartSlot == aInterval.iEndSlot || iEndSlot == aInterval.iStartSlot;
    }
    return adjacent;
}

/*!
 \brief CalenSlotInterval::IsEmpty
*/
bool CalenSlotInterval::IsEmpty() const
{
    return iStartSlot >= iEndSlot;
}

/*!
 \brief CalenSlotInterval::Intersect
*/
void CalenSlotInterval::Intersect(const CalenSlotInterval& aInterval)
{
    if (aInterval.IsEmpty()) {
        iEndSlot = iStartSlot;
    }
    else {
        iStartSlot = Max(iStartSlot, aInterval.iStartSlot);
        iEndSlot = Min(iEndSlot, aInterval.iEndSlot);
    }
}

/*!
 \brief CalenSlotInterval::Subtract
*/
void CalenSlotInterval::Subtract(const CalenSlotInterval& aInterval, CalenSlotInterval& aSecondPart)
{
    aSecondPart.iStartSlot = aSecondPart.iEndSlot = 0;
    if (!aInterval.IsEmpty() && Overlaps(aInterval)) {
        if (aInterval.iStartSlot <= iStartSlot) {
            iStartSlot = aInterval.iEndSlot;
        }
        else
            if (aInterval.iEndSlot >= iEndSlot) {
                iEndSlot = aInterval.iStartSlot;
            }
            else {
                aSecondPart.iStartSlot = aInterval.iEndSlot;
                aSecondPart.iEndSlot = iEndSlot;
                iEndSlot = aInterval.iStartSlot;
            }
    }
}

/*!
 \brief CalenSlotInterval::operator>
*/
bool CalenSlotInterval::operator>(const CalenSlotInterval& aInterval) const
{
    return iStartSlot > aInterval.iStartSlot;
}

/*!
 \brief CalenTimeColumn::Close
*/
void CalenTimeColumn::Close()
{
    iEventArray.clear();
}

/*!
 \brief CalenTimeColumn::AddEvent
*/
void CalenTimeColumn::AddEvent(const CalenTimedEventInfo& aEvent)
{
    ASSERT( aEvent.iStartSlot < aEvent.iEndSlot );
    ASSERT( CanFitEvent( aEvent ) );

    if (iEventArray.count() == 0) {
        iStartSlot = aEvent.iStartSlot;
    }
    iEndSlot = aEvent.iEndSlot;
    iEventArray.append(aEvent);
}

/*!
 \brief CalenTimeColumn::CanFitEvent
*/
bool CalenTimeColumn::CanFitEvent(const CalenTimedEventInfo& aEvent)
{
    return (aEvent.iStartSlot >= iEndSlot) || (iEventArray.count() == 0);
}

/*!
 \brief CalenTimeColumn::ContainsEvent
*/
bool CalenTimeColumn::ContainsEvent(const TCalenInstanceId& aId)
{
    bool contains(false);
    int eventCount(iEventArray.count());
    for (int i = 0; i < eventCount && !contains; i++) {
        if (iEventArray[i].iId == aId) {
            contains = true;
        }
    }
    return contains;
}

/*!
 \brief CalenTimeColumn::AddOffset
*/
void CalenTimeColumn::AddOffset(int aOffset, int aPos)
{
    if (aOffset != 0) {
        CalenSlotInterval::AddOffset(aOffset, aPos);
        int eventCount(iEventArray.count());
        for (int i = 0; i < eventCount; i++) {
            iEventArray[i].AddOffset(aOffset, aPos);
        }
    }
}

/*!
 \brief CalenTimeRegion::Close
*/
void CalenTimeRegion::Close()
{
    int colCount(iColumns.count());
    for (int i = 0; i < colCount; i++) {
        iColumns[i].Close();
    }
    iColumns.clear();
    iIntervals.clear();
}

/*!
 \brief CalenTimeRegion::Overlaps
*/
bool CalenTimeRegion::Overlaps(const CalenSlotInterval& aInterval) const
{
    // the base class implementation would be ok, but we might want the assertion here
    ASSERT( iColumns.count()> 0 );
    return CalenSlotInterval::Overlaps(aInterval);
}

/*!
 \brief CalenTimeRegion::AddOffset
*/
void CalenTimeRegion::AddOffset(int aOffset, int aPos)
{
    if (aOffset != 0) {
        CalenSlotInterval::AddOffset(aOffset, aPos);
        int colCount(iColumns.count());
        for (int i = 0; i < colCount; i++) {
            iColumns[i].AddOffset(aOffset, aPos);
        }
        int intervalCount(iIntervals.count());
        for (int i = 0; i < intervalCount; i++) {
            iIntervals[i].AddOffset(aOffset, aPos);
        }
    }

}

/*!
 \brief CalenTimeRegion::AddEvent
*/
void CalenTimeRegion::AddEvent(const CalenTimedEventInfo& aEvent)
{
    if (iColumns.count() == 0) {
        // This is the first event added to this region
        iStartSlot = aEvent.iStartSlot;
        iEndSlot = aEvent.iEndSlot;
    }
    else {
        // Check that the events actually are added in the correct order
        ASSERT( aEvent.iStartSlot >= iStartSlot );
        ASSERT( aEvent.iStartSlot < iEndSlot );

        if (aEvent.iEndSlot > iEndSlot) {
            iEndSlot = aEvent.iEndSlot;
        }
    }

    AddInterval(aEvent);

    // If the event fits in one of the existing columns, add it to that one
    bool added(false);
    int colCount(iColumns.count());
    for (int i = 0; i < colCount && !added; i++) {
        if (iColumns[i].CanFitEvent(aEvent)) {
            iColumns[i].AddEvent(aEvent);
            added = true;
        }
    }

    if (!added) {
        // otherwise create a new column for it
        iColumns.append(CalenTimeColumn());
        iColumns[iColumns.count() - 1].AddEvent(aEvent);
    }
}

/*!
 \brief CalenTimeRegion::AddInterval
*/
void CalenTimeRegion::AddInterval(const CalenTimedEventInfo& aEvent)
{
    /*
     * Here are a few examples of different possible cases for this method.
     * The first picture of each example shows the initial situation in the
     * array, and the new event to be added. The transformations performed
     * are shown with a few intermediate steps.
     *
     * nO = newOverlap, nE = newEvent
     *
     * Example one:
     *
     *               ------------------
     *               | newEvent       |
     * --------------------------------
     * ... | lastOverlap | tail |
     * --------------------------
     *
     *                   --------------
     *                   | newEvent   |
     * --------------------------------
     * ... | lastOverlap | tail |
     * --------------------------
     *
     *                   --------------
     *                   | nO   | nE  |
     * --------------------------------
     * ... | lastOverlap | tail |
     * --------------------------
     *
     * Result:
     * --------------------------------
     * ... | lastOverlap        | nE  |
     * --------------------------------
     *
     *
     * Example two:
     *                          ------------------
     *                          | newEvent       |
     * -------------------------------------------
     * ... | lastOverlap | tail      |
     * -------------------------------
     *
     *                          ------------------
     *                          | nO | newEvent  |
     * -------------------------------------------
     * ... | lastOverlap | tail      |
     * -------------------------------
     *
     * Result:
     * -------------------------------------------
     * ... | lastOverlap | tail | nO | newEvent  |
     * -------------------------------------------
     *
     *
     * Example three:
     *                          --------------
     *                          | newEvent   |
     * -----------------------------------------------------
     * ... | lastOverlap | tail                            |
     * -----------------------------------------------------
     *
     *                          --------------
     *                          | newOverlap |
     * -----------------------------------------------------
     * ... | lastOverlap | tail                            |
     * -----------------------------------------------------
     *
     *                          --------------
     *                          | newOverlap |
     * -----------------------------------------------------
     * ... | lastOverlap | tail |            | tailPart2   |
     * -----------------------------------------------------
     *
     * Result:
     * -----------------------------------------------------
     * ... | lastOverlap | tail | newOverlap | tailPart2   |
     * -----------------------------------------------------
     */

    // Fill out the new event
    CalenEventInterval newEvent;
    newEvent.iStartSlot = aEvent.iStartSlot;
    newEvent.iEndSlot = aEvent.iEndSlot;
    newEvent.iStatus = aEvent.iStatus;
    //    newEvent.iReplicationStatus = aEvent.iReplicationStatus;
    newEvent.iOverlap = false;

    // a pointer to the last interval which is an overlap interval
    CalenEventInterval* lastOverlap = NULL;
    // If nonempty, this is the last interval after the last overlap
    CalenEventInterval tail;
    tail.iStartSlot = tail.iEndSlot = 0;

    // Find lastOverlap and tail.
    if (iIntervals.count() >= 1) {
        // lastInterval is a pointer to the last interval in the array
        CalenEventInterval* lastInterval = &iIntervals[iIntervals.count() - 1];

        // If this is an overlap interval, we haven't got any tail.
        if (lastInterval->iOverlap) {
            lastOverlap = lastInterval;
        }
        else {
            tail = *lastInterval;
        }

        // If there's at least two intervals, and the last one wasn't an overlap,
        // the second last one must be an overlap.
        if (iIntervals.count() >= 2 && !lastOverlap) {
            lastOverlap = &iIntervals[iIntervals.count() - 2];
            ASSERT( lastOverlap->iOverlap );
        }
    }

    // If we got a tail, remove it from the array (it will be readded
    // at the end if needed)
    if (!tail.IsEmpty()) {
        iIntervals.removeAt(iIntervals.count() - 1);
    }

    CalenEventInterval empty;
    if (lastOverlap) {
        // Remove the part which already is marked as an overlap
        // from the new event. The new event can't start before the
        // last overlap starts since events are added in order, therefore
        // the second subtraction result interval will remain empty.
        newEvent.Subtract(*lastOverlap, empty);
        ASSERT( empty.IsEmpty() );
    }

    // Create a new interval, representing the overlap between the old tail
    // and the new event
    CalenEventInterval newOverlap = newEvent;
    newOverlap.iOverlap = true;
    newOverlap.Intersect(tail);

    CalenEventInterval tailPart2 = tail; // initialize iOverlap and iStatus from tail
    // Remove the new overlap from the old tail, possibly creating two separate intervals.
    tail.Subtract(newOverlap, tailPart2);

    // If the subtraction only yielded one single interval, but it's
    // after newOverlap, make tailPart2 contain that and make tail empty.
    if (tail > newOverlap) {
        tailPart2 = tail;
        tail.iEndSlot = tail.iStartSlot;
    }

    // Remove the new overlap from the new event. Since we already removed the old
    // overlap, this subtraction can't produce two intervals either.
    newEvent.Subtract(newOverlap, empty);
    ASSERT( empty.IsEmpty() );

    // If the new overlap is adjacent to the old one, expand the old one
    // and set the new overlap to be empty.
    if (lastOverlap && newOverlap.Adjacent(*lastOverlap)) {
        lastOverlap->Union(newOverlap);
        newOverlap.iEndSlot = newOverlap.iStartSlot;
    }

    // Add all the new intervals, if they're non-empty.
    if (!tail.IsEmpty()) {
        iIntervals.append(tail);
    }
    if (!newOverlap.IsEmpty()) {
        iIntervals.append(newOverlap);
    }
    if (!tailPart2.IsEmpty()) {
        iIntervals.append(tailPart2);
    }
    if (!newEvent.IsEmpty()) {
        iIntervals.append(newEvent);
    }

}

// End of File
