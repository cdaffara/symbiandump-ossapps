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
* Description:
*
*/

#include "nmuieventsnotifier.h"
#include <QDataStream>
#include <XQPublishAndSubscribeUtils>

const long int NmSettingsKeyCategoryId(0x2002C326); // NMailAgent UID3
const unsigned long int NmSettingsKeyUiEvents(0x00000100);
const XQPublishAndSubscribeSettingsKey NmUiEventsKey(NmSettingsKeyCategoryId,
                                                     NmSettingsKeyUiEvents);

#define NmUiEventTypeArrayType qint16
#define NmUiViewIdArrayType qint16
#define NmMailboxIdArrayType quint64

const size_t NmUiEventByteArraySize(sizeof(NmUiEventTypeArrayType) +
                                    sizeof(NmUiViewIdArrayType) +
                                    sizeof(NmMailboxIdArrayType));


/*!
    \class NmUiEventsNotifier
    \brief API class for notifying other processes about email UI events.
*/


/*!
    Class constructor.
*/
NmUiEventsNotifier::NmUiEventsNotifier(QObject *parent /* = NULL */)
: QObject(parent)
{
    XQPublishAndSubscribeUtils psUtils(mSettingsManager);

    // The following will only work if this instance is constructed inside the
    // nmailagent process since the category ID of the key is the UID3 of the
    // process. Other processes do not have the permission to define the
    // property.
    psUtils.defineProperty(NmUiEventsKey, XQSettingsManager::TypeByteArray);

    mSettingsManager.startMonitoring(NmUiEventsKey);

    connect(&mSettingsManager, SIGNAL(valueChanged(const XQSettingsKey &, const QVariant &)),
            this, SLOT(valueChanged(const XQSettingsKey &, const QVariant &)),
            Qt::UniqueConnection);
}


/*!
    Class destructor.
*/
NmUiEventsNotifier::~NmUiEventsNotifier()
{
    XQPublishAndSubscribeUtils psUtils(mSettingsManager);
    psUtils.deleteProperty(NmUiEventsKey);
}


/*!
    Notifies observers about view state changed event.

    \param eventType The type of state change.
    \param viewId The ID of the view which was shown.
    \param mailboxId The ID of the mailbox related to the view.

    \return True if success, false otherwise.
*/
bool NmUiEventsNotifier::notifyViewStateChanged(const NmUiEventType eventType,
                                                const NmUiViewId viewId,
                                                const NmId &mailboxId)
{
    QByteArray array;
    array.resize((int)NmUiEventByteArraySize);
    
    // Use a data stream to write the values into the array.
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << (NmUiEventTypeArrayType)eventType <<
              (NmUiViewIdArrayType)viewId <<
              (NmMailboxIdArrayType)mailboxId.id();

    XQSettingsManager settingsManager;
    return settingsManager.writeItemValue(NmUiEventsKey, array);
}


/*!
    Compares the two given keys.

    \param settingsKey XQSettingsKey
    \param psSettingsKey XQPublishAndSubscribeSettingsKey

    \return <code>true</code> if the target, uid and key values match, otherwise
            returns <code>false</code>
*/
bool NmUiEventsNotifier::keysEqual(
    const XQSettingsKey& settingsKey,
    const XQPublishAndSubscribeSettingsKey& psSettingsKey) const
{
    return ((settingsKey.target() == psSettingsKey.target()) &&
            (settingsKey.uid() == psSettingsKey.uid()) &&
            (settingsKey.key() == psSettingsKey.key()));
}


/*!
    Parses the value from the given byte array.

    \param array The array to parse.
    \param eventType Where the event type is stored to.
    \param viewId Where the view ID is stored to.
    \param mailboxId Where the mailbox ID is stored to.

    \return True if success, false otherwise.
*/
bool NmUiEventsNotifier::parseKeyValue(const QByteArray &array,
                                       NmUiEventType &eventType,
                                       NmUiViewId &viewId,
                                       NmId &mailboxId) const
{
    bool success(false);

    if (array.size() >= (int)NmUiEventByteArraySize) {
        NmUiEventTypeArrayType type(0);
        NmUiViewIdArrayType vId(0);
        NmMailboxIdArrayType mbId(0);
        
        // Use a data stream to read the values from the array.
        QDataStream stream(array);
        stream >> type >> vId >> mbId;
        
        // Convert the values into proper types.
        eventType = (NmUiEventType)type;
        viewId = (NmUiViewId)vId;
        mailboxId.setId(mbId);

        success = true;
    }

    return success;
}


/*!
    Handles the occured UI events. Processes the value changed events of
    settings keys.

    \param key The settings key of which value was changed.
    \param value The new value of the key. 
*/
void NmUiEventsNotifier::valueChanged(const XQSettingsKey &key,
                                      const QVariant &value)
{
    if (keysEqual(key, NmUiEventsKey)) {
        QByteArray array = value.toByteArray();
        
        NmUiEventType eventType(NmInvalidUiEvent);
        NmUiViewId viewId(NmUiViewNone);
        NmId mailboxId(0);
        
        if (parseKeyValue(array, eventType, viewId, mailboxId)) {
            emit viewStateChanged(eventType, viewId, mailboxId);
        }
    }
}


// End of file.
