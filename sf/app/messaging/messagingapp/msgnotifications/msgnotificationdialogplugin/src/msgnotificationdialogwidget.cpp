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
 * Description: Widget class for Notificaiton Dialog Plugin
 *
 */
#include "debugtraces.h"

#include <ccsdefs.h>
#include <QIcon>
#include <QVariant>
#include <QList>
#include <hbicon.h>
#include <hbpopup.h>


#include "convergedmessage.h"

#include "msgnotificationdialogpluginkeys.h"
#include "msgnotificationdialogwidget.h"

const int NoError = 0;
const int ParameterError = 10000;

static const char NEW_MSG_ICON[] = "qtg_large_new_message";

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::MsgNotificationDialogWidget
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
MsgNotificationDialogWidget::MsgNotificationDialogWidget(
                                                const QVariantMap &parameters)
: HbNotificationDialog(),
mConversationId(-1),
mLastError(NoError),
mShowEventReceived(false)
{
    constructDialog(parameters);
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::~MsgNotificationDialogWidget
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------                                                
MsgNotificationDialogWidget::~MsgNotificationDialogWidget()
{
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::setDeviceDialogParameters
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
bool MsgNotificationDialogWidget::setDeviceDialogParameters(
                                                const QVariantMap &parameters)
{
   return constructDialog(parameters);
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::constructDialog
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
bool MsgNotificationDialogWidget::constructDialog(
                                                const QVariantMap &parameters)
    {
    // Set parameters 
    mLastError = NoError;

    // if conversation id is not proper return false
    mConversationId = parameters.value(KConversationIdKey).toLongLong(); 
    if( mConversationId <= 0)
        {
        mLastError = ParameterError;
        return false;    
        }

    prepareDisplayName(parameters);

    setIcon(HbIcon(NEW_MSG_ICON));

    int messageType = parameters.value(KMessageTypeKey).toInt();
    if( messageType == ECsSMS)
        {
        QString messageBody;
        messageBody = parameters.value(KMessageBodyKey).toString();
        messageBody.replace(QChar::ParagraphSeparator, QChar::LineSeparator);
        messageBody.replace('\r', QChar::LineSeparator);
        setText(messageBody);    
        }
    else
        {
        // No special handling required for other message types.
        // Subject & Body text are both set to description in msgnotifier.
        setText(parameters.value(KMessageSubjectKey).toString());
        }
    
    // enable touch activation and connect to slot
    enableTouchActivation(true);
    connect(this, SIGNAL(activated()), this, SLOT(widgetActivated()));

    // set the standard timeout value, that is used by default notificaitons dialogs
    setTimeout(HbPopup::StandardTimeout);

    return true;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::deviceDialogError
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
int MsgNotificationDialogWidget::deviceDialogError() const
{    
    // Get error
    return mLastError;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::closeDeviceDialog
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::closeDeviceDialog(bool byClient)
{
    // Close device dialog
    Q_UNUSED(byClient);
    close();

    // If show event has been received, close is signalled from hide event. If not,
    // hide event does not come and close is signalled from here.
    if (!mShowEventReceived)
        {
        emit deviceDialogClosed();
        }
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::deviceDialogWidget
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
HbDialog *MsgNotificationDialogWidget::deviceDialogWidget() const
{    
    // Return display widget
    return const_cast<MsgNotificationDialogWidget*>(this);
}


// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::hideEvent
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::hideEvent(QHideEvent *event)
{
    HbNotificationDialog::hideEvent(event);
    emit deviceDialogClosed();
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::showEvent
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::showEvent(QShowEvent *event)
{
    HbNotificationDialog::showEvent(event);
    mShowEventReceived = true;
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::widgetActivated
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::widgetActivated()
{           
    //Emit data to be used by msgnotifier
    QVariantMap data;
    data.insert(KConversationIdKey,mConversationId);
    emit deviceDialogData(data);
    enableTouchActivation(false);    
}

// ----------------------------------------------------------------------------
// MsgNotificationDialogWidget::prepareDisplayName
// @see msgnotificationdialogwidget.h
// ----------------------------------------------------------------------------
void MsgNotificationDialogWidget::prepareDisplayName(
                                                const QVariantMap &parameters)
{
    //Set the Contact Name/Number
    QString displayName = parameters.value(KDisplayNameKey).toString();
    QString contactAddress = parameters.value(KContactAddressKey).toString();
    
    if (displayName.isEmpty())
    {
        setTitle(contactAddress);
    }
    else
    {
        setTitle(displayName);
    }    
}
