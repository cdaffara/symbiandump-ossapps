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

#ifndef UNI_VIEWER_ADDRESS_CONTAINER_H
#define UNI_VIEWER_ADDRESS_CONTAINER_H

// SYSTEM INCLUDES
#include <HbWidget>

// USER INCLUDES
#include "convergedmessageaddress.h"

// FORWARD DECLARATIONS
class QGraphicsLinearLayout;

/**
 * Container widget for all the address widgets
 */

class UniViewerAddressContainer : public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerAddressContainer(QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    virtual ~UniViewerAddressContainer();

    /**
     * Sets From recipients 
     * @param fromRecipient list of From recipients
     */
    void setFromField(const QString& fromRecipient, const QString& alias);

    /**
     * Sets To recipients 
     * @param toRecipients list of To recipients
     */
    void setToField(ConvergedMessageAddressList toRecipients);

    /**
     * Sets Cc recipients 
     * @param ccRecipients list of Cc recipients
     */
    void setCcField(ConvergedMessageAddressList ccRecipients);

    /**
     * Sets Bcc recipients
     * @param bccRecipients list of Bcc recipients
     */
    void setBccField(ConvergedMessageAddressList bccRecipients);

    /**
     * Clears the content of the widget
     */
    void clearContent();

signals:
    /**
     * this signal is emitted when send message is triggered form UniViewerAddressWidget
     */
    void sendMessage(const QString& phoneNumber,const QString& alias);

private:

    /**
     * Main Layout
     */
    QGraphicsLinearLayout* mMainLayout;
};

#endif // UNI_VIEWER_ADDRESS_CONTAINER_H
// EOF
