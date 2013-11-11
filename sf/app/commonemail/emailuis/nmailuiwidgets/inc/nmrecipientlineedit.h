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
* Description: implementation of recipient field (to, cc, bcc) line editor
*
*/

#ifndef NMRECIPIENTLINEEDIT_H_
#define NMRECIPIENTLINEEDIT_H_

#include <nmhtmllineedit.h>

class QGraphicsItem;
class QModelIndex;

class HbPopup;
class HbListView;

class NmContactHistoryModel;
class NmAutoFillListViewItem;
class NmPopupBackground;
class NmAddress;

class NMAILUIWIDGETS_EXPORT NmRecipientLineEdit : public NmHtmlLineEdit
{
    Q_OBJECT

public:
    NmRecipientLineEdit(QGraphicsItem *parent = NULL);
    virtual ~NmRecipientLineEdit();
    QList<NmAddress> emailAddressList();
    void addContacts(QList<NmAddress *> contacts);
	
public slots:
    void addSelectedContacts(const QVariant &selectedContacts);
    void addSelectedContactFromHistory(const QModelIndex &modelIndex);

protected:	// from HbLineEdit
    void keyPressEvent(QKeyEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);

private:
    void generateEmailAddressList();
    void createAutofillComponents();
    void createAutoFillPopup();
    void showAutofillPopup();
    void hideAutofillPopup();
    void currentTextPart(int& startIndex, int& length);
    void getChosenAddressFromModel(const QModelIndex &modelIndex, NmAddress &address);
    void setHighlight(int currentPos);
    void gestureEvent(QGestureEvent* event);
    void handleTap();
    void insertContactText(const QString &text);
    void keyPressEventSemicolon(QKeyEvent *event);
    void keyPressEventDelete(QKeyEvent *event);
    void keyPressEventLeft(QKeyEvent *event);
    void keyPressEventRight(QKeyEvent *event);
    
private slots:
    void handleTextChanged(const QString &text);
    void modelCompleted(int err);

private:
    // mEmailAddressList contains all emailaddress that appear in the lineedit and 
    // all emailaddress corresponding to the contact's name added from Contacts,
    // including invalid emailaddress user typed or invalid emailaddress added from Contacts.
    // Emailaddress validation check is conducted in nmeditorview.
    QList<NmAddress> mEmailAddressList; 	
    QList<NmAddress> mRecipientsAddedFromContacts;
	
    // Flag "need to generate mEmailAddressList" is used whenever editing made after 
    // the emailaddress validation check failed.
    bool mNeedToGenerateEmailAddressList;

    NmContactHistoryModel *mContactHistoryModel; // owned

    HbPopup *mAutoFillPopup; // owned

    HbListView *mAutofillListView; // owned
    NmAutoFillListViewItem *mAutofillListViewItem; //owned
    NmPopupBackground *mPopupBackground; // pwned
};

class NmPopupBackground : public QGraphicsItem
{
public:
    explicit NmPopupBackground(HbPopup *popup, QGraphicsItem *parent = 0);
    bool isModal() { return true; }
    QRectF boundingRect() const { return QRect(-100, -100, 1000, 1000); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
protected:
    bool sceneEvent(QEvent *event);
private:
    mutable QRectF mRect;
    HbPopup *mPopup; //not owned
};

#endif   // NMRECIPIENTLINEEDIT_H_
