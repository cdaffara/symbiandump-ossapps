/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nmuiheaders.h"

static const QString NmContactsServiceName = "phonebookservices";
static const QString NmContactsInterfaceName = "com.nokia.symbian.IContactsFetch";
static const QString NmContactsOperationName = "multiFetch(QString,QString)";

static const int NmMaxRows = 10000;

/*!
   widget is created using AD/docml
*/
NmRecipientField::NmRecipientField(
        QObject *parent, HbDocumentLoader &docLoader, const QString &objPrefix):
    QObject(parent),
    mDocumentLoader(docLoader),
    mObjectPrefix(objPrefix),
    mLabel(NULL),
    mRecipientsEditor(NULL),
    mLaunchContactsPickerButton(NULL),
    mAiwRequest(NULL)
    
{
    NM_FUNCTION;

    // Load the widgets from nmeditorview.docml. The names match to the definitions in that docml.
    mWidget = qobject_cast<HbWidget *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Field"));

    mLabel = qobject_cast<HbLabel *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Label"));

    mRecipientsEditor = qobject_cast<NmRecipientLineEdit *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Edit"));
    if (mRecipientsEditor) {
        mRecipientsEditor->setMaxRows(NmMaxRows);
    }

    mLaunchContactsPickerButton = qobject_cast<HbPushButton *>
        (mDocumentLoader.findWidget(mObjectPrefix + "Button"));
    if (mLaunchContactsPickerButton) {
        mLaunchContactsPickerButton->setIcon(NmIcons::getIcon(NmIcons::NmIconContacts));
    }

    createConnections();
}

/*!
   Creates connections for this class items
*/
void NmRecipientField::createConnections()
{
    NM_FUNCTION;

    connect(mRecipientsEditor, SIGNAL(textChanged(const QString &)),
        this, SIGNAL(textChanged(const QString &)));
    connect(mRecipientsEditor, SIGNAL(cursorPositionChanged(int, int)),
        this, SIGNAL(cursorPositionChanged(int, int)));
    connect(mRecipientsEditor, SIGNAL(editingFinished()),
        this, SIGNAL(editingFinished()));
    connect(mRecipientsEditor, SIGNAL(selectionChanged()),
        this, SIGNAL(launchContactsPicker()));
    connect(mLaunchContactsPickerButton, SIGNAL(clicked()),
        this, SLOT(launchContactsPicker()));
}


/*!
   Destructor
*/
NmRecipientField::~NmRecipientField()
{
    NM_FUNCTION;
    delete mAiwRequest;
}

/*!
   Widget height
*/
qreal NmRecipientField::height()
{
    NM_FUNCTION;

    return mWidget->geometry().height();
}

/*!
   editor
*/
NmRecipientLineEdit *NmRecipientField::editor() const
{
    NM_FUNCTION;

    return mRecipientsEditor;
}

/*!
   Label widget
*/
HbLabel *NmRecipientField::label() const
{
    NM_FUNCTION;
    
    return mLabel;
}

/*!
   LineEdit contents
*/
const QString NmRecipientField::text() const
{
    NM_FUNCTION;

    return mRecipientsEditor->text();
}


/*!
   Slot setText
*/
void NmRecipientField::setText(const QString &newText)
{
    NM_FUNCTION;

    if (newText != mRecipientsEditor->text()) {
        mRecipientsEditor->setText(newText);
        emit textChanged(newText);
    }
}


/*!
   This Slot launches the contacts-picker
*/
void NmRecipientField::launchContactsPicker()
{
    NM_FUNCTION;

    if (!mAiwRequest) {
        XQApplicationManager mAppmgr;
        
    
        bool isEmbeded = true;
        mAiwRequest = mAppmgr.create(NmContactsServiceName, NmContactsInterfaceName,
                                                     NmContactsOperationName, isEmbeded);
        
        if (mAiwRequest) {
            connect(mAiwRequest, SIGNAL(requestOk(QVariant)),
                    mRecipientsEditor, SLOT(addSelectedContacts(QVariant)));
            connect(mAiwRequest, SIGNAL(requestOk(QVariant)),
                           this, SLOT(requestCompleted()));
            connect(mAiwRequest, SIGNAL(requestError(int, QString &)),
                           this, SLOT(requestCompleted()));
            mAiwRequest->setSynchronous(false);
        }
        else {
            // Failed creating request
            NM_ERROR(1,"XQApplicationManager: failed creating fecth contactspicker request");
            return;
        }
    
        QVariantList args;
        args << hbTrId("txt_mail_select_contacts");
        args << KCntActionEmail;
        mAiwRequest->setArguments(args);
        
        // Send request
        if (!mAiwRequest->send()) {
           //Failed sending request
           NM_ERROR(1,"XQApplicationManager: failed sending request");
        }
    }
}
/*!
   XQAiwRequest calls this if request is ok or an error happens.
*/
void NmRecipientField::requestCompleted()
{
    delete mAiwRequest;
    mAiwRequest = NULL;
}
