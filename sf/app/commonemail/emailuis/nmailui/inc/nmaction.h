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

#ifndef NMACTION_H_
#define NMACTION_H_

#include <hbaction.h>

/*!
    \class NmAction
    \brief NMail action object. Derived from HbAction. Includes priority
    \value indicating the priority of the command in the list.
*/
class NmAction : public HbAction
{
public:
    /*!
     * \enum NmAvailabilityCondition
     * \brief Defines conditions when action might not be available.
     */
    enum NmAvailabilityCondition
    {
        NmAlways = 0,   // Action is always available
        NmSendable,     // Action is not available if the related message cannot be send
        NmAttachable    // Action is not available if attachment cannot be added        
    };
    
    inline NmAction(int priorityIndex);
    inline void setPriorityIndex(const int priorityIndex);
    inline int priorityIndex() const;
    inline void setAvailabilityCondition(NmAvailabilityCondition condition);
    inline NmAvailabilityCondition availabilityCondition() const;

private:
    int mIndex;
    NmAvailabilityCondition mAvailabilityCondition;
};

/*!
     Constructor \a priorityIndex defines place of action in menus
 */
inline NmAction::NmAction(int priorityIndex) 
    : HbAction(), mIndex(priorityIndex), mAvailabilityCondition(NmAlways)
{
    
}

/*!
     Sets priority index. \a priorityIndex defines action place in menus
 */
inline void NmAction::setPriorityIndex(int priorityIndex)
{
    mIndex = priorityIndex;
}

/*!
    Returns priority index
 */
inline int NmAction::priorityIndex() const
{
    return mIndex;
}

/*!
    Sets condition is action available or not \a condition is condition state
 */
inline void NmAction::setAvailabilityCondition(NmAction::NmAvailabilityCondition condition)
{
    mAvailabilityCondition = condition;
}

/*!
    returns availibility condition
 */
inline NmAction::NmAvailabilityCondition NmAction::availabilityCondition() const
{
    return mAvailabilityCondition;
};



#endif  //NMACTION_H_
