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

#ifndef  CALENDAYINFO_H
#define  CALENDAYINFO_H

//  INCLUDES
#include <e32std.h>
#include <QList>
#include <QDateTime>
#include <QAbstractItemModel>

#include <calinstance.h>
#include "caleninstanceid.h"


//
/** Scrolling directions **/
enum TScrollDirection
    {
    EScrollUp,
    EScrollDown,
    EScrollLeft,
    EScrollRight
    };

//Constants
const int KFSCalMaxDescriptionLength = 100;
const int KFSCalStartingHour = 8;
const int KFSCalSlotsInHour = 2;

/**
 * An interval containing a start and end slot.
 * The start slot belongs to the interval, the end slot
 * is the first slot outside of the interval. If the end slot
 * is before or at the same slot as the start slot, the interval
 * is considered empty.
 */
class CalenSlotInterval
    {
public:
    /**
     * Check if this interval overlaps the second interval.
     */
    bool Overlaps( const CalenSlotInterval& aInterval ) const;

    /**
     * Add aOffset to all slot coordinates later than aPos
     */
    void AddOffset( int aOffset, int aPos );

    /**
     * Set this interval to be the minimum interval
     * containing both this original interval and aInterval.
     */
    void Union( const CalenSlotInterval& aInterval );

    /**
     * Check if aInterval lies directly next to this interval.
     */
    bool Adjacent( const CalenSlotInterval& aInterval ) const;

    /**
     * Check if this interval is empty.
     */
    bool IsEmpty() const;

    /**
     * Set this interval to be the area contained in both
     * this interval and to aInterval.
     */
    void Intersect( const CalenSlotInterval& aInterval );

    /**
     * Remove aInterval from this interval. If aInterval lies
     * within this interval, the result is two separate intervals.
     * This object contains one of them, aSecondPart contains the other one.
     * If the result is just one single interval, this interval contains that
     * and aSecondPart is set to an empty interval.
     */
    void Subtract( const CalenSlotInterval& aInterval, CalenSlotInterval& aSecondPart );

    /**
     * Check if this interval starts later than aInterval.
     */
    bool operator>( const CalenSlotInterval& aInterval ) const;

    /**
     * The starting slot of the interval. This is the first slot
     * that belongs to the interval.
     */
    int iStartSlot;
    
    /**
     * The ending slot of the interval. This is the first slot
     * that doesn't belong to the interval.
     */
    int iEndSlot;
    };


/**
 * Class for storing a calendar instance and the range it occupies.
 */
struct CalenTimedEventInfo : public CalenSlotInterval
    {
public:
    /**
     * The id of the calendar instance
     */
    TCalenInstanceId iId;
    
    /**
     * Status of the entry, needed for setting the displayed color later
     */
    AgendaEntry::Status iStatus;
    
    /**
     * Replication status of the entry, needed for setting the displayed color
     * later.
     */
//    AgendaEntry::TReplicationStatus iReplicationStatus;
    };

/**
 * Class for storing general time intervals and their associated
 * status (needed for displaying the interval).
 */
struct CalenEventInterval : public CalenSlotInterval
    {
public:
    /**
     * The status of this interval, if it represents only one calendar
     * instance.
     */
    AgendaEntry::Status iStatus;
    
    /**
     * The replication status of this interval, if it represents only one
     * calendar instance.
     */
//    AgendaEntry::TReplicationStatus iReplicationStatus;
    
    /**
     * A flag saying that this interval represents a conflict between two or
     * more calendar instances.
     */
    bool iOverlap;
    };



/**
 * A class containing a sequence of non-overlapping events,
 * visualised as a column.
 */
class CalenTimeColumn : public CalenSlotInterval
    {
public:

    /**
     * Explicitly frees the memory used by the event array.
     */
    void Close();

    /**
     * Add a new event to this column. Events must be added sequentially,
     * and must not overlap earlier events in this column.
     */
    void AddEvent( const CalenTimedEventInfo& aEvent );

    /**
     * Check if a new event can be added to this column.
     */
    bool CanFitEvent( const CalenTimedEventInfo& aEvent );

    /**
     * Check if this column contains an event with the id aId.
     */
    bool ContainsEvent( const TCalenInstanceId& aId );

    /**
     * Add aOffset to all slot coordinates later than aPos
     */
    void AddOffset( int aOffset, int aPos );

    /**
     * Event array.
     */
    QList<CalenTimedEventInfo> iEventArray;
    };


/**
 * A class containing one or more columns with events,
 * where every event overlaps at least one event in some other
 * column. (Otherwise that event should be added to a separate region.)
 */
class CalenTimeRegion : public CalenSlotInterval
    {
public:
    
    /**
     * Explicitly frees the memory used by data structures.
     */
    void Close();

    /**
     * Check if the given interval overlaps with this region.
     */
    bool Overlaps( const CalenSlotInterval& aInterval ) const;

    /**
     * Add an event to this region. Events must be added sequentially,
     * and must overlap this region (unless it's the first event of the region).
     */
    void AddEvent( const CalenTimedEventInfo& aEvent );

    /**
     * Add aOffset to all slot coordinates later than aPos
     */
    void AddOffset( int aOffset, int aPos );

private:
    
    /**
     * Add the event to the bookkeeping of overlapping/nonoverlapping
     * intervals.
     */
    void AddInterval( const CalenTimedEventInfo& aEvent );
    
public:

    QList<CalenTimeColumn> iColumns;
    QList<CalenEventInterval> iIntervals;
    };


/**
 * A container struct, used by the clients of the slot info storage,
 * to provide data in.
 */
struct SCalenApptInfo
    {
    QModelIndex iIndex;
    QDateTime iStartTime;
    QDateTime iEndTime;
    bool iAllDay;
    TCalenInstanceId iId;
    AgendaEntry::Status iStatus;
//    AgendaEntry::TReplicationStatus iReplicationStatus;
    TBufC<KFSCalMaxDescriptionLength> iSummary;
    TUint32 iColor;
    };


/**
 * Storage class for storing all calendar instances within one day. This
 * class organises the data according to the way it will be needed in the
 * day and week view.
 */
class CalenDayInfo
    {
public:

    enum TSlotsInHour
        {
        EOne = 1,
        ETwo,
        EThree,
        EFour
        };

public:  // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CalenDayInfo( TSlotsInHour aSlotsInHour );
    
    /**
     * Destructor
     */
    virtual ~CalenDayInfo();


public:     // New functions

    /**
     * Reset the storage, remove all data and set the state back to normal.
     */
    void Reset();

    /**
     * Add a timed event. All timed events must be added in increasing
     * order (sorted by starting time).
     */
    void InsertTimedEvent( const SCalenApptInfo& aItemInfo );
    
    /**
     * Add an untimed event.
     */
//    void InsertUntimedEventL( const CCalInstance& aInstance );
    
    /**
     * Add an untimed event. (Nonleaving version, useful for testing.)
     */
    void InsertUntimedEvent( AgendaEntry::Type aType,
                             const TCalenInstanceId& aId );
    /**
     * Add an allday event.
     */
    void InsertAlldayEvent( const SCalenApptInfo& aItemInfo );

    /**
     * Is the given event allday event
     * @param aStart time to be checked
     * @param aEnd time to be checked
     * @return true if this is allday event, false otherwise
     */
    static bool IsAlldayEvent( QDateTime aStart, QDateTime aEnd );

    /**
     * Is the given event allday event
     * @param aInstance Instance to be checked
     * @return true if this is allday event, false otherwise
     */
//    static bool IsAlldayEvent( const CCalInstance& aInstance );

    /**
     * Return the slot number where this class would insert the
     * untimed events if nothing else is specified.
     */
    int SuggestedUntimedSlotPos();
    
    /**
     * Return how many untimed slots is needed for this day.
     */
    int NeededUntimedSlotCount();

    /**
     * Update the indexing to take the current amount of untimed slots
     * into account. This must be called after all untimed events
     * have been added.
     *
     * @param aSlot the slot where the untimed events are to be added.
     *              If negative, uses the default, otherwise aSlot must
     *              be less than or equal to the default position as
     *              returned by SuggestedUntimedSlotPos().
     * @param aUntimedCount the number of slots to insert for untimed events. If
     *                      aSlot is specified, this must be larger or equal
     *                      to NeededUntimedSlotCount().
     */
    int UpdateUntimedPos( int aSlot = -1, int aUntimedCount = 0 );

    /**
     * Return the first slot containing a non-allday event.
     * If this class doesn't contain any data, returns KErrNotFound.
     */
    int FirstOccupiedSlot();
    
    /**
     * Return the last slot containing a non-allday event.
     * If this class doesn't contain any data, returns KErrNotFound.
     */
    int LastOccupiedSlot();

    int EarliestEndSlot();
    int LastStartSlot();


    /**
     * Convert a starting time into a slot index.
     */
    int SlotIndexForStartTime( QDateTime aStartTime );
    
    /**
     * Convert an ending time into a slot index.
     */
    int SlotIndexForEndTime( QDateTime aStartTime );

    /**
     * Get information about where the item aItemInfo
     * should be displayed. The parameters are filled
     * on return.
     *
     * @param aStartSlot    the first slot of the event
     * @param aEndSlot      the first slot after the event
     * @param aColumnIndex  the column in which this event is located
     * @param aColumns      the total number of columns in the region
     *                      this event belongs to
     */
    void GetLocation( const SCalenApptInfo& aItemInfo,
                      int& aStartSlot,
                      int& aEndSlot,
                      int& aColumnIndex,
                      int& aColumns );

    /**
     * Get the number of allday events
     */
    int AlldayCount();
    
    /**
     * Get the number of todo events
     */
    int TodoCount();

    /**
     * Check if a slot is the first slot of an hour.
     */
    bool IsHourStartSlot( const int& aSlotIndex ) const;
    
    /**
     * Check if a slot is an extra slot (for untimed events).
     */
    bool IsExtraSlot( const int& aSlotIndex ) const;
    
    /**
     * Convert a slot index into a hour
     */
    int HourFromSlotIndex( const int& aSlotIndex ) const;
    
    /**
     * Convert a hour into a slot index.
     */
    int SlotIndexFromHour( int aHour );

    /**
     * Rounds the slot number up (towards earlier hours) to an even hour
     */
    int RoundHourUp( int aSlot );

    /**
     * Rounds the slot number down (towards later hours) to an even hour
     */
    int RoundHourDown( int aSlot );

    /**
     * Get the starting slot of the current selection
     */
    void GetSelectedSlot( int& aSlot, int& aRegion, int& aColumnIndex, int& aColumns );
    
    /**
     * Try to move the selection in the given direction
     *
     * @return true if able to move the selection, false if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    bool MoveSelection( TScrollDirection aDirection );

    /**
     * Move the selected slot within the currently selected event.
     */
    void MoveSelectionInEvent( TScrollDirection aDirection );

    /**
     * Make sure the selected slot within the currently selected event is valid.
     */
    void UpdateSelectionInEvent();

    /**
     * Check if any event currently is selected.
     */
    bool IsEventSelected() const;
    
    /**
     * Check if the current selection actually denotes
     * more than one event (the todo event slot is selected,
     * containing more than one todo).
     */
    bool IsMultipleEventsSelected() const;
    
    /**
     * Check if an allday event currently is selected.
     */
    bool IsAlldayEventSelected() const;

    /**
     * Get the instance id of the currently selected event.
     */
    TCalenInstanceId SelectedEvent();

    /**
     * Update the state to make the given calendar instance selected
     *
     * @return KErrNotFound if the instance isn't found, KErrNone otherwise.
     */
    int SelectEvent( const TCalenInstanceId& aId );

    /**
     * Get the instance id of an untimed event. Maximally one
     * todo event is counted into this, i.e. aIndex = 1 never returns
     * a todo event even though there are more than one.
     */
    TCalenInstanceId UntimedEvent( int aIndex );

    /**
     * Get info about an allday event.
     */
    const CalenTimedEventInfo& AlldayEvent( int aIndex );

    /**
     * Move the selection to the given slot, possibly selecting
     * an event.
     */
    void SelectSlot( int aSlot );

    /**
     * Return the list of regions.
     */
    const QList<CalenTimeRegion>& RegionList() const;

    /**
     * Get the list of event intervals (for use in week view and ribbon).
     */
    void GetEventIntervals( QList<CalenEventInterval>& aArray ) const;

    /**
     * Return the interval which is selected currently.
     */
    CalenSlotInterval SelectedInterval();
    
    /**
     * Sets selection within a region
     * 
     * @param aRegion Region index.
     * @param aColumn Column index.
     * @param aSlot   Slot number (has to be aligned to full hour).
     */
    bool SetSelectionInRegion( int aRegion, int aColumn, int aSlot );

private:

    enum TMoveDirection
        {
        EMoveDirectionUp = -1,
        EMoveDirectionDown = 1
        };

    /**
     * See if any region overlaps the given interval. Regions are searched
     * in the direction specified by aDirection, e.g. if aDirection < 0,
     * this returns the last overlapping region, if aDirection > 0, returns
     * the first overlapping instead.
     *
     * @return the index of the found overlapping region, or -1 if no
     *         matching region was found.
     */
    int FindRegion( const CalenSlotInterval& aInterval, int aDirection );

    /**
     * See if any event overlaps the given interval within the current column.
     * Events are searched in the direction specified by aDirection,
     * e.g. if aDirection < 0, this returns the last overlapping event,
     * if aDirection > 0, returns the first overlapping instead.
     *
     * @return the index within the column of the overlapping event, or -1 if no
     *         matching event was found.
     */
    int FindEvent( const CalenSlotInterval& aInterval, int aDirection );

    /**
     * Update the selection state by selecting the first event which ends at
     * the end of the current region.
     */
    void EnterRegionFromBelow();
    
    /**
     * Update the selection state by selecting the first event which starts
     * at the start of the current region.
     */
    void EnterRegionFromAbove();

    /**
     * Try to move the selection in the given direction, when an
     * empty area is selected.
     *
     * @return true if able to move the selection, false if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    bool MoveInEmptyArea( TScrollDirection aDirection );
    
    /**
     * Try to move the selection in the given direction, when the
     * selection is in a region.
     *
     * @return true if able to move the selection, false if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    bool MoveInRegion( TScrollDirection aDirection );
    
    /**
     * Try to move the selection in the given direction, when an
     * allday event is selected
     *
     * @return true if able to move the selection, false if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    bool MoveInAlldayEvent( TScrollDirection aDirection );

    /**
     * Update the selection state by moving from one ordinary event column
     * to another, in the given direction
     */
    void MoveBetweenColumns( TScrollDirection aDirection );

    /**
     * Update the selection state by moving in the given direction
     */
    void MoveInColumn( int aDirection );
    
    /**
     * The selection should be moved out of the current region (in the given
     * direction), update the selection state according to what there is
     * outside of the region.
     */
    void MoveOutFromRegion( int aDirection );

    /**
     * Set the selected slot within the current event according to the selection
     * direction.
     */
    void SetSelectionInEvent( int aDirection );

    /**
     * Determines how large area to scan for new events/regions when moving in the
     * given direction.
     *
     * I.e., if moving upwards, returns the whole interval from the start of aInterval
     * up to the next slot which can be focused as an empty slot. If aInterval doesn't
     * start on an even hour, the returned interval is from the start of the first whole
     * hour before the interval, to the start of aInterval.
     *
     * If moving downwards, returns the whole interval from the end of aInterval to the
     * end of the next whole hour after the interval.
     *
     */
    CalenSlotInterval NextFocusArea( const CalenSlotInterval& aInterval, int aDirection );

    /**
     * Return the next slot to focus if moving in the given direction from the interval
     * and focusing an empty slot.
     */
    int NextEmptyFocusSlot( const CalenSlotInterval& aInterval, int aDirection );
    
    /**
     * Return the interval which the current selection state represents, if
     * an empty area is selected.
     */
    CalenSlotInterval EmptySelectionInterval();

    /**
     * Backup the whole selection state.
     */
    void StoreOrigSelection();
    
    /**
     * Check if the current selection state is valid, if not, reset it
     * to the backed up copy
     * @return true if selection state is valid, false if not.
     */
    bool ValidateSelection();

private:    // New data

    QList<CalenTimeRegion> iRegionList;
    QList<TCalenInstanceId> iUntimedEvents;
    QList<TCalenInstanceId> iTodoEvents;
    QList<CalenTimedEventInfo> iAlldayEvents;

    int iLastStartSlot;
    int iEarliestEndSlot;
    /**
     * The total number of untimed slots.
     */
    int iUntimedSlotCount;
    /**
     * The slot index of the first untimed slot.
     */
    int iFirstUntimedSlot;
    /**
     * The number of empty untimed slots.
     */
    int iEmptyUntimedSlots;
    /**
     * The number of slots per hour.
     */
    TSlotsInHour iSlotsInHour;

    /**
     * Chooses which allday event is selected. If none currently is selected,
     * this is negative. This variable overrides the rest selection variables.
     * If this points to an allday event, the other variables are ignored,
     * except iSelectedSlot which is used for returning to the original place
     * if moving back to the ordinary events.
     */
    int iSelectedAlldayEvent;

    /**
     * Chooses which region currently is selected. If none currently is selected,
     * this is negative. If this points to a region, iSelectedColumn and
     * iSelectedColumnEventIndex are taken into account.
     */
    int iSelectedRegion;

    /**
     * Chooses which column is selected within the currently selected region.
     * If this is equal to the number of columns, the last, implicit, empty column
     * is selected.
     */
    int iSelectedColumn;
    /**
     * Chooses which event is selected within the currently selected column. If
     * none is selected, this is negative.
     */
    int iSelectedColumnEventIndex;
    /**
     * Chooses which slot in the day currently is in focus. This must always point
     * to a valid slot. If an event is selected within a column, it points to an
     * slot within that event. If no event is selected, it points to the start
     * of the currently selected empty time region (which is iSlotsInHour slots long).
     * If an allday event is selected, this points to the last selected slot in the
     * ordinary day area.
     */
    int iSelectedSlot;

    // copies of the current selection state, to be used for reverting to the original
    // state if the selection is moved to an invalid position
    int iOrigSelectedAlldayEvent;
    int iOrigSelectedRegion;
    int iOrigSelectedColumn;
    int iOrigSelectedSlot;
    int iOrigSelectedColumnEventIndex;

    };

#endif // CALENDAYINFO_H


// End of File
