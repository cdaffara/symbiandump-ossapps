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
 * Description: Custom widget derived from HbTextEdit which provides rich text
 *              processing
 *
 */
#ifndef UNI_VIEWER_ADDRESS_WIDGET_H
#define UNI_VIEWER_ADDRESS_WIDGET_H

#include <HbTextEdit>

#include "unidatamodelplugininterface.h"

/**
 * UniViewerAddressWidget provides support for rich text processing
 */
class UniViewerAddressWidget : public HbTextEdit
{
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    UniViewerAddressWidget(QGraphicsItem * parent = 0);

    /**
     * Destructor
     */
    ~UniViewerAddressWidget();
    
    /**
     * Populates the addresses and creates the anchors on them.
     * @param label Label of the address like - "From"
     * @param address Address string to be populated.
     */
    void populate(const QString &label, const QString &address, const QString &alias);

    /**
     * Populates the addresses and creates the anchors on them.
     * @param label Label of the address like - "To", "Cc"
     * @param addressList List of addresses to be populated.
     */
    void populate(const QString &label,ConvergedMessageAddressList addressList);

    /**
     * Clears the contents of the widget.
     */
    void clearContent();

protected:
    /**
     * Gesture event, overridden from base class.
     */
    void gestureEvent(QGestureEvent* event);
    
private:
    /**
     * Helper method to highlight find item on tap.
     * @param highlight, if true highlight else dont.
     */
    void highlightText(bool highlight);
    
    /**
     * short tap handler.
     * @param anchor anchor at cursor position.
     * @param pos tap position.
     */
    void shortTapAction(QString anchor,const QPointF& pos); 
    
    /** Helper method to get contact id against phone number or e-mail id.
     * @param value phone number or email id.
     * @param fieldName name of field to be matched.
     * @param fieldType type of field to be matched.
     */
    int resolveContactId(const QString& value,
                         const QString& fieldName, 
                         const QString& fieldType);
    
    /**
     * Helper method to populate menu items.
     * @param contextMenu menu to be populated.
     * @param data, highlighted number.
     */
    void populateMenu(HbMenu* contextMenu, const QString& data);
    
private slots:

    /**
      * called when aboutToShowContextMenu signal is emitted.
      */
    void aboutToShowContextMenu(HbMenu *contextMenu, const QPointF &pos);
    
    /**
     * Called when option menu is closed.
     */
    void menuClosed();
    
    /**
     * handlers for phone number specific menu items.
     */
    void openContactInfo();
    void call();
    void saveToContacts();
    void sendMessage();
    void copyToClipboard();
    
    /**
     * called after service request is completed.
     */
    void onServiceRequestCompleted();
    
    /**
     * Slot for handling valid returns from the framework.
     * 
     * @param result const QVariant&
     */
    void handleOk(const QVariant& result);
    
    /**
     * Slot for handling errors. Error ids are provided as 
     * 32-bit integers.
     * @param errorCode qint32
     */
    void handleError(int errorCode, const QString& errorMessage);
    

signals:
    /**
     * this signal is emitted when send message is triggered for a highlighted number.
     */
    void sendMessage(const QString& phoneNumber,const QString& alias);
    
private:
    //Current cursor position.
    int mCursorPos;
    
    //char formats for highlight.
    QTextCharFormat mFormatHighlight;
    QTextCharFormat mFormatNormal;

    };

#endif // UNI_VIEWER_ADDRESS_WIDGET_H
// EOF
