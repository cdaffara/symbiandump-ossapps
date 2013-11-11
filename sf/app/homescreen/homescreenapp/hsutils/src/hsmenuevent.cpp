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
* Description:  Menu Event.
*
*/

#include "hsmenuevent.h"


/*!
    \class HsMenuEvent
    \ingroup group_hsutils
    \brief Represents class of events in menu application state machine.

    \sa HsMenuEventFactory
    \sa HsMenuEventTransition
*/

/*!
    \enum HsMenuEvent::OperationType

    Enumerates specific types of events within the class.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::AddToHomeScreen
    Adding to home screen.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::OpenCollection
    Opening collection.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::RenameCollection
    Renaming collection.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::DeleteCollection
    Removing collection.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::CollectionDeleted
    Removing collection completed.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::AddAppsToCollection
    Adding applications to collection.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::RemoveAppFromCollection
    Removing application from collection.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::OpenApplicationLibrary
    Opening application library from shortcut.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::ArrangeCollection
    Switch to arrange mode.
*/
/*! \var HsMenuEvent::OperationType HsMenuEvent::Unknown
    Unknown menu event.
*/
/*!
    \return Id identyfing the class of the events.
*/
QEvent::Type HsMenuEvent::eventType()
{
    return QEvent::Type(QEvent::User + 100);
}


/*!
    Constructor.
    \param aOperation Specific type of event ususally mapping to an user level operation.
    \param aData Data associated with the event.
*/
HsMenuEvent::HsMenuEvent(OperationType aOperation,	const QVariantMap &aData):
    QEvent(HsMenuEvent::eventType()),	mOperation(aOperation), mData(aData)
{
}

/*!
    \return Specific type of the event.
 */
HsMenuEvent::OperationType HsMenuEvent::operation() const
{
    return mOperation;
}

/*!
    \return Data associated with the event.
 */
QVariantMap HsMenuEvent::data() const
{
    return mData;
}

