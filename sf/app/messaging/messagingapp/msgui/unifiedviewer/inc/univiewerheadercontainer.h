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
 * Description: This widget holds the message header.
 *
 */

#ifndef UNIVIEWER_HEADER_CONTAINER_H
#define UNIVIEWER_HEADER_CONTAINER_H

#include <HbWidget>

#include <convergedmessageaddress.h>

class QGraphicsLinearLayout;
class UniViewerDetailsWidget;
class HbGroupBox;
class HbFrameItem;
class UniViewerAddressContainer;
class UniViewerFeeder;
class UniViewerAttachmentWidget;

class UniViewerHeaderContainer: public HbWidget
{
Q_OBJECT

public:

    /**
     * Constructor
     */
    explicit UniViewerHeaderContainer(UniViewerFeeder* feeder, QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~UniViewerHeaderContainer();

    /**
     * Populates the content on the widget
     */
    void populateContent();

    /**
     * Clears the content on the widget
     */
    void clearContent();

private:

    /**
     * Populates the subject field in Details widget.
     */
    void populateSubject();

    /**
     * Populates the attachment list in Attachment container.
     */
    void populateAttachments();

    /**
     * Populates the from, to & cc fields.
     */
    void populateAddressContainer();

    /**
     * Sets the heading text of the address group box.
     */
    void setAddrGroupBoxHeading();

    /**
     * Creates a comma seperated address list using converged message address list.
     * @param addressList Converged message address list.
     * @return Comma separated address list.
     */
    QString createAddressList(const ConvergedMessageAddressList &addressList);

private slots:

    /**
     * Slot to know address box state.
     */
    void addressBoxToggled(bool state);

signals:
    /**
     * this signal is emitted when send message is triggered form UniViewerAddressWidget
     */
    void sendMessage(const QString& phoneNumber,const QString& alias);

private:

    /**
     * Main layout.
     */
    QGraphicsLinearLayout *mMainLayout;

    /**
     * UniViewerFeeder object
     * Not Owned
     */
    UniViewerFeeder* mViewFeeder;

    /**
     * UniViewerDetailsWidget object
     * Owned
     */
    UniViewerDetailsWidget *mViewerDetails;

    /**
     * HbGroupBox object
     * owned.
     */
    HbGroupBox *mHeaderGroupBox;

    /**
     * Separator line
     * Owned.
     */
    HbFrameItem *mSeparator;

    /**
     * UniViewerAddressContainer object
     * owned.
     */
    UniViewerAddressContainer *mAddressContainer;

    /**
     * List of attachment widgets.
     */
    QList<UniViewerAttachmentWidget *> mUniViewerAttachmentstList;
};

#endif /* UNIVIEWER_HEADER_CONTAINER_H */

// EOF
