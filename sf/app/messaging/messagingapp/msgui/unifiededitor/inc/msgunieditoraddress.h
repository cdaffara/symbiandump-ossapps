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
 * Description:Header for entering To field (phone number, contact name).
 *
 */

#ifndef MSG_UNIFIED_EDITOR_ADDRESS_H
#define MSG_UNIFIED_EDITOR_ADDRESS_H

#include <msgunieditorbasewidget.h>

#include <convergedmessageaddress.h>
#include <QMap>

class HbTextItem;
class HbPushButton;
class HbLineEdit;
class HbAction;
class MsgUnifiedEditorLineEdit;

class MsgUnifiedEditorAddress : public MsgUnifiedEditorBaseWidget
    {
    Q_OBJECT

public:

    /**
     * Constructor
     */
    MsgUnifiedEditorAddress(const QString& label,
                            QGraphicsItem *parent = 0);

    /**
     * Destructor
     */
    ~MsgUnifiedEditorAddress();

    /**
     * Seeker method to return back data to editor's view
     * By default, does not remove duplicates.
     * @param removeDuplicates, True if duplicate removal is needed
     */
    ConvergedMessageAddressList addresses(bool removeDuplicates=false);

    /**
     * Get total number of recipient's (including duplicates)
     */
    int addressCount();

    /**
     * setter method to set address
     * @param addrlist, list of addresses to be set in address editor
     */
    void setAddresses(ConvergedMessageAddressList addrlist,bool aSkipCheck=FALSE);

    /**
     * Get amount of digits to be used in contact matching
     */
    static int contactMatchDigits();

    /**
     * setter method to enable flag to skip max recipient limit query
     */
    void skipMaxRecipientQuery(bool skip);

    /**
     * validate contacts
     */
    bool validateContacts();

public slots:
    /**
     * To set focus on editable field.
     */
    void setFocus();

signals:
    /**
     * Emitted when send button from virtual ITUT is pressed.
     */
    void sendMessage();

    /**
     * Emitted when content is added or removed
     */
    void contentChanged();

private slots:

    /**
     * called after selection from pbk.
     */
    void fetchContacts();
    /**
     * Slot for handling valid returns from the phonebook contacts fetched.
     */
    void handleOk(const QVariant& result);

    /**
     * Slot for handling errors. Error ids are provided as
     */
    void handleError(int errorCode, const QString& errorMessage);

    /**
     * Called when contentsChanged signal is emitted by the line edit
     */
    void onContentsChanged(const QString&);

    /**
     * launch query for recipient limit usecase
     */
    void handleRecipientLimitReached();
   
	/**
     * This slot is called when max recipients reached dialog is launched.
     * @param action selected action (yes or no).
     */
    void onMaxRecipientsReached(HbAction*);

    /**
     * Handle invalid contact dialog useraction
     */
    void handleInvalidContactDialog(HbAction* act);
    
    /**
     * Unblocks signals for pbk buttion after some delay.
     */
    void unblockSignals();

private:
    /**
     * Remove edit-field's user-deleted addresses from Map
     */
    void syncDeletionsToMap();

    /**
     * Add edit-field's user-added addresses to Map
     */
    void syncAdditionsToMap();

    /**
     * Removes duplicate addresses and gives unique address list
     */
    QStringList uniqueAddressList();

    /**
     * Reset the addresslist to previous list
     */
    void resetToPrevious();

private:

    /**
     * Push button to launch phone book.
     */
    HbPushButton* mLaunchBtn;

    /**
     * line edit field.
     */
    MsgUnifiedEditorLineEdit* mAddressEdit;

    /**
     * address map.
     */
    QMap<QString, QString> mAddressMap;

    /**
     * holds the previous buffer inside address field
     */
    QString mPrevBuffer;

    /**
     * flag to skip max recipient limit query
     */
    bool mSkipMaxRecipientQuery;

    /**
     * flag to indicate that the SMS recipient limit is about
     * to be exceeded by a bulk insertion of addresses e.g. multiple
     * selection from contact selection dialog
     */
    bool mAboutToExceedMaxSmsRecipients;

    /**
     * flag to indicate that the MMS recipient limit is about
     * to be exceeded by a bulk insertion of addresses e.g. multiple
     * selection from contact selection dialog
     */
    bool mAboutToExceedMaxMmsRecipients;

    /**
     * count by which a bulk-insertion will exceed max MMS recipient
     * limit e.g. multiple selection from contact selection dialog
     */
    int mExceedsMaxMmsRecipientsBy;
    };

#endif //MSG_UNIFIED_EDITOR_ADDRESS_H
