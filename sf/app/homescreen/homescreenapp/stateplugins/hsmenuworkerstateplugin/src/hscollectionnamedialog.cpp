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
 * Description: Menu name collection state
 *
 */

#include <hbaction.h>
#include <hbinputdialog.h>
#include <hblineedit.h>
#include <hbvalidator.h>
#include <hsmenuservice.h>
#include <HbParameterLengthLimiter>
#include <QDebug>

#include "hscollectionnamedialog.h"

/*!
 \class HsCollectionNameDialog
 \ingroup group_hsworkerstateplugin
 \brief Rename Collection State.
 Collection name state
 */

/*!
 \var HsCollectionNameDialog::mCollectionsNames
 Collections names list from data model.
 */

/*
 \var HsCollectionNameDialog::mLastCollectionName
 Last found collection name.
 */

/*!
 Maximum length if collection name
 */
const int qMaxStrLength = 248;

/*!
 Constructor
 \param itemId Collection Id.
 \retval void
 */
HsCollectionNameDialog::HsCollectionNameDialog(const int &itemId)
{
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::HsInputDialog");
    mOtherCollectionsNames = HsMenuService::getCollectionNames();


    setInputMode(HbInputDialog::TextInput);
    setPromptText(hbTrId("txt_applib_title_collection_name"));
    lineEdit()->setMaxLength(qMaxStrLength);
    if (itemId) {
        const QString currentCollectionName(
                HsMenuService::getName(itemId));
        setValue(currentCollectionName);
        mOtherCollectionsNames.removeOne(currentCollectionName);
        mSuggestedNames << currentCollectionName;
    } else {
        setValue(hbTrId("txt_applib_dialog_entry_collection"));
    }

    HSMENUTEST_FUNC_EXIT("HsInputDialog::HsInputDialog");
}

/*!
 Destructor
 */
HsCollectionNameDialog::~HsCollectionNameDialog()
{
    mOtherCollectionsNames.clear();
    mSuggestedNames.clear();
}

/*!
 Gets new collection's name.
 \param item_id Item's id.
 \retval Selected action.
 */
void HsCollectionNameDialog::open(QObject* receiver, const char* member)
{
    HSMENUTEST_FUNC_ENTRY("HsCollectionNameDialog::open");
    this->setAttribute(Qt::WA_DeleteOnClose);

    onTextChanged(value().toString());
    makeConnect();

    HbInputDialog::open(receiver, member);

    HSMENUTEST_FUNC_EXIT("HsCollectionNameDialog::open");
}

/*!
  \return User input if it is unique collection name or an unique collection name being
  a combination of user name and a suffix matching following regular expression:
  "\([0-9][1-9]\)|\([1-9][0-9]+\)"
 */
QString HsCollectionNameDialog::uniqueCollectionName() const
{
    return generateUniqueCollectionName(value().toString());
}

/*!
    \reimp 
    Disconnects signals and calls base class impl. which emits finished(HbAction*) 
 */
void HsCollectionNameDialog::closeEvent ( QCloseEvent * event )
{
    qDebug("HsCollectionNameDialog::closeEvent");
    makeDisconnect();
    HbDialog::closeEvent(event); // emits finished(HbAction*) 
}

/*!
  \param name Input string which is to be basename
    of resulting unique collection name.
  \return Unique collection name.
 */
QString HsCollectionNameDialog::generateUniqueCollectionName(
        const QString &name) const
{
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::newName");

    QString newName(name);

    QChar leadingCharacter('0');
    static const int fieldWidth(2);
    static const int base(10);
    unsigned int numToAppend(1);

    
    if (newName == HbParameterLengthLimiter(
        "txt_applib_dialog_entry_collection")) {
        while (mOtherCollectionsNames.contains(newName)) {
            newName = HbParameterLengthLimiter(
                    "txt_applib_dialog_entry_collectionl1").arg(
                numToAppend, fieldWidth, base, leadingCharacter);
            numToAppend++;
        }
    }
    else {
        while (mOtherCollectionsNames.contains(newName)) {
            newName = HbParameterLengthLimiter(
                    "txt_applib_dialog_collection_name_entry_1_l1").arg(
                numToAppend, fieldWidth, base, leadingCharacter).arg(name);
            numToAppend++;
        }
    }

    HSMENUTEST_FUNC_EXIT("HsInputDialog::newName");
    return newName;
}
/*!
  \param name Input string which is to be basename
    for suggested collection name.
  \return Unique collection name if it was not yet proposed during lifetime of the dialog
   or \a name otherwise.
 */
QString HsCollectionNameDialog::suggestedCollectionName(const QString &name)
{
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::processInput");

    QString newName = generateUniqueCollectionName(name);

    if (mSuggestedNames.contains(newName)) {
        newName = name;
    }

    if (newName != name) {
        mSuggestedNames << newName;
    }

    HSMENUTEST_FUNC_EXIT("HsInputDialog::validate");
    return newName;
}

/*!
 Connects edit line signals to slots.
 */
void HsCollectionNameDialog::makeConnect()
{
    /*connect(lineEdit(), SIGNAL(textChanged(QString&text)),
     SLOT(onTextChanged(QString&text)));*/

    connect(lineEdit(), SIGNAL(contentsChanged()),
            SLOT(onContentsChanged()));
}

/*!
 Disconnects edit line signals from slots.
 */
void HsCollectionNameDialog::makeDisconnect()
{
    /*disconnect(lineEdit(), SIGNAL(textChanged(QString&text)),
     this, SLOT(onTextChanged(QString&text)));*/
    disconnect(lineEdit(), SIGNAL(contentsChanged()),
               this, SLOT(onContentsChanged()));
}

/*!
 This slot is received whenever the text changes.
 This slot is also received when the text is
 changed programmatically,
 for example, by calling setText().
 \param text the new text.
 */
void HsCollectionNameDialog::onTextChanged(const QString &text)
{
    qDebug() << QString("HsInputDialog::onTextChanged( %1 )").arg(text);
    HSMENUTEST_FUNC_ENTRY("HsInputDialog::onTextChanged");
    if (text.trimmed() == "" ) {
		actions()[0]->setEnabled(false);
    } else {
        actions()[0]->setEnabled(true);
    }

    QString newText = suggestedCollectionName(text);
    if (newText != text) {
        makeDisconnect();
        lineEdit()->setText(newText);
        lineEdit()->setSelection(text.length(), newText.length()
                                 - text.length());
        makeConnect();
    }
    HSMENUTEST_FUNC_EXIT("HsInputDialog::onTextChanged");
}

/*!
 This slot is received whenever the contents changes.
 */
void HsCollectionNameDialog::onContentsChanged()
{
    qDebug("HsInputDialog::onContentsChanged()");
    onTextChanged(value().toString());
}
