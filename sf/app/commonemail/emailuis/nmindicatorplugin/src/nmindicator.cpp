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
#include "nmindicator.h"

#include <QThreadPool>
#include <QRunnable>
#include <QByteArray>
#include <QProcess>
#include <QTimer>
#include <QVariant>
#include <QCoreApplication>

#include <HbStringUtil>

const int NmMailboxInfoItemCount = 8;
const int NmMaxOutboxCount = 99;
const int NmActivationDelay = 100; // 0.1s

/*!
    \class NmMailboxInfo
    \brief Stores info that indicator displays
*/

/*!
     Class constructor.
*/
NmMailboxInfo::NmMailboxInfo()
{
    NM_FUNCTION;

    mId = 0;
    mSyncState = SyncComplete;
    mConnectState = Disconnected;
    mUnreadMails = 0;
    mOutboxMails = 0;
}

/*!
    \class NmIndicator
    \brief Handles client request and showing the indications.
*/

/*!
     Class constructor.
*/
NmIndicator::NmIndicator(const QString &indicatorType)
:HbIndicatorInterface(indicatorType,
        HbIndicatorInterface::NotificationCategory,
        InteractionActivated)
{
    NM_FUNCTION;
    mActive = false;
}

/*!
    Class destructor.
*/
NmIndicator::~NmIndicator()
{
    NM_FUNCTION;
}

/*!
    called by the framework, when the user interacts with the indicator.
    \a type is a type of the interaction.

    The method should be reimplemented, when interaction type is set to other than
    HbIndicatorInterface::NoInteraction.

    should return true, if interaction was handled, false otherwise. Default implementation
    returns false.

    \sa interactionTypes
*/
bool NmIndicator::handleInteraction(InteractionType type)
{
    NM_FUNCTION;

    bool handled = false;
    if (type == InteractionActivated) {
        mActive = false;
        handled = true;

        QVariantMap data;
        emit userActivated(data);
        
        // The action will be delayed to improve the usability
        QTimer::singleShot(NmActivationDelay, this, SLOT(delayedAction()));
    }
    return handled;
}

/*!
     Deactivate the indicator  
*/
void NmIndicator::delayedAction() 
{
    emit deactivate();
}

/*!
    \fn QVariant HbIndicatorInterface::indicatorData(int role) const = 0

    Returns the data stored under the given role. The indicator should at least have data elements for
    HbIndicatorInterface::TextRole and HbIndicatorInterface::DecorationNameRole. If data element for
    HbIndicatorInterface::IconNameRole doesn't exist, the same icon retrieved by \c DecorationNameRole
    is used both in status indicator area and universal indicator popup.  If the indicator is
    low priority indicator, it is shown only in universal indicator popup, and data element for
    \c IconNameRole is ignored

    \a role the data role for the indicator. A value from the HbIndicatorInterface::DataRole - enum.

    For the roles not supported by the plugin, empty QVariant should be returned.
*/
QVariant NmIndicator::indicatorData(int role) const
{
    NM_FUNCTION;

    switch(role) {
        case PrimaryTextRole:
            if (mActive) {                
                return mMailbox.mName;
            }
            break;
        case SecondaryTextRole:
            if (mActive) {
                if (mMailbox.mOutboxMails>0) {
                    int outboxCount = mMailbox.mOutboxMails;
                    if (outboxCount>NmMaxOutboxCount) {
                        outboxCount = NmMaxOutboxCount;
                    }
                    QString text = QString(hbTrId("txt_mail_status_menu_waiting_to_send")).
                        arg(HbStringUtil::convertDigits(QString::number(outboxCount)));
                    return text;
                }
                else if (mMailbox.mUnreadMails>0) {
                    return hbTrId("txt_mail_status_menu_new_mail");
                }
            }
            break;
        case DecorationNameRole:
            {
			// Icon for the mailbox in the menu
            if (mActive) {
                if (!mMailbox.mMailboxIconName.isEmpty()) {
                    return mMailbox.mMailboxIconName;
                }
                else {
                    return QString("qtg_large_email");
                }
            }
            break;
		    }
        default:
        	break;
    }
    return QVariant();
}

/*!
    Handles the requests sent from the client
    \a type is a type of the request.
    \a parameter is extra parameter from the client. Can be invalid, if not given.

    Should return true, when the request is handled.
    Default implementation returns false.

    \sa RequestType, HbIndicator
*/
bool NmIndicator::handleClientRequest( RequestType type,
        const QVariant &parameter)
{
    NM_FUNCTION;

    bool handled(false);
    switch (type) {
        case RequestActivate:
            {
            mActive = true;
			storeMailboxData(parameter);

            emit dataChanged();
            handled =  true;
            }
            break;
        case RequestDeactivate:
            {
            mActive = false;

            // also the deactivation may give updated data
			storeMailboxData(parameter);

            emit deactivate();
            }
            break;
        default:
            break;
    }
    return handled;
}

/*!
	Stores mailbox data to member variable mMailbox.
 */
void NmIndicator::storeMailboxData(QVariant mailboxData)
{
    NM_FUNCTION;

    QList<QVariant> infoList = mailboxData.toList();

    if (infoList.count() >= NmMailboxInfoItemCount) {
        mMailbox.mId.setId(infoList.at(0).value<quint64>());
        mMailbox.mName = infoList.at(1).toString();
        mMailbox.mUnreadMails = infoList.at(2).toInt();
        mMailbox.mSyncState = infoList.at(3).value<NmSyncState>();
        mMailbox.mConnectState = infoList.at(4).value<NmConnectState>();
        mMailbox.mOutboxMails = infoList.at(5).toInt();
        mMailbox.mMailboxIconName = infoList.at(6).toString();
        bool sendingState = infoList.at(7).toInt();
    }
}
