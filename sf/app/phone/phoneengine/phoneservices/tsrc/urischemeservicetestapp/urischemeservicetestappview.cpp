/*!
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

#include <QtGui>
#include <QGraphicsLinearLayout>
#include <QUrl>
#include <HbAction>
#include <HbLabel>
#include <HbMainWindow>
#include <HbPushButton>
#include <HbLineEdit>
#include <HbNotificationDialog>
#include <HbCheckBox>
#include <hbinputeditorinterface.h>
#include <xqservicerequest.h>
#include <xqappmgr.h>
#include "urischemeservicetestappview.h"

const int KMultitapTimeInMs = 800;
const int KNumOfTelMultitapCharacters = 4;
const int KNumOfCtiMultitapCharacters = 2;
const QString KTelUriSchemePrefix = "tel:";
const QString KCtiUriSchemePrefix = "cti:";

UriSchemeServiceTestAppView::UriSchemeServiceTestAppView( 
    HbMainWindow& mainWindow ) 
    :
    m_mainWindow(mainWindow),
    m_lineEdit(NULL),
    m_telUriCheckBox(NULL),
    m_ctiUriCheckBox(NULL),
    m_uriLabel(NULL),
    m_multitapIndex(0),
    m_multitapCount(0)
{
    setTitle("URI Scheme Test App");
    
    m_extraChar.insert(Qt::Key_Comma, ',');
    m_extraChar.insert(Qt::Key_Slash, '/');
    m_extraChar.insert(Qt::Key_Asterisk, '*');
    m_extraChar.insert(Qt::Key_NumberSign, '#');
    m_extraChar.insert(Qt::Key_P, 'p');
    m_extraChar.insert(Qt::Key_W, 'w');
    
    QGraphicsLinearLayout *mainLayout = 
        new QGraphicsLinearLayout(Qt::Vertical, this);
    
    // create URI scheme selection check boxes
    m_telUriCheckBox = new HbCheckBox(this);
    m_telUriCheckBox->setText("TEL URI Scheme");
    m_telUriCheckBox->setChecked(true);
    connect(
        m_telUriCheckBox, SIGNAL(stateChanged(int)), 
        this, SLOT(setupTelUriConfiguration(int)));
    mainLayout->addItem(m_telUriCheckBox);
    m_ctiUriCheckBox = new HbCheckBox(this);
    m_ctiUriCheckBox->setText("CTI URI Scheme");
    connect(
        m_ctiUriCheckBox, SIGNAL(stateChanged(int)), 
        this, SLOT(setupCtiUriConfiguration(int)));
    mainLayout->addItem(m_ctiUriCheckBox);
    
    // create URI input field
    QGraphicsLinearLayout *uriEditLayout = 
        new QGraphicsLinearLayout(Qt::Horizontal);
    m_uriLabel = new HbLabel(this);
    uriEditLayout->addItem(m_uriLabel);
    m_lineEdit = new HbLineEdit(this);
    HbEditorInterface editorInterface(m_lineEdit);
    editorInterface.setMode(HbInputModeNumeric);
    HbAction *specialInputAction = new HbAction(QString("INP"), this);
    editorInterface.addAction(specialInputAction);
    connect(
        specialInputAction, SIGNAL(triggered(bool)), 
        this, SLOT(inputSpecialCharacter(bool)));
    m_lineEdit->setInputMethodHints(
        Qt::ImhPreferNumbers|Qt::ImhNoPredictiveText);
    uriEditLayout->addItem(m_lineEdit);
    mainLayout->addItem(uriEditLayout);
    
    // create Call & Clear buttons
    HbPushButton *callButton = new HbPushButton(this);
    callButton->setText("Call");
    mainLayout->addItem(callButton);
    connect(
        callButton, SIGNAL(clicked(bool)), 
        this, SLOT(makeCall(bool)));
    HbPushButton *clearButton = new HbPushButton(this);
    clearButton->setText("Clear");
    mainLayout->addItem(clearButton);
    connect(
        clearButton, SIGNAL(clicked(bool)), 
        this, SLOT(clearInputField(bool)));
    
    setLayout(mainLayout);
    
    setupTelUriConfiguration(Qt::Checked);
}


UriSchemeServiceTestAppView::~UriSchemeServiceTestAppView()
{
    
}


void UriSchemeServiceTestAppView::makeCall(bool checked)
{
    Q_UNUSED(checked)
    
    QString uriToCall = "";
    uriToCall += m_uriLabel->plainText();
    uriToCall += m_lineEdit->text();
    QUrl callUri(uriToCall);
    
    HbNotificationDialog *notifDialog = new HbNotificationDialog();
    notifDialog->setDismissPolicy(HbPopup::TapAnywhere);
    notifDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    
    XQApplicationManager aiwManager;
    QScopedPointer<XQAiwRequest> request(aiwManager.create(callUri));
    if (request) {
        request->setSynchronous(true);
        request->setEmbedded(false);
        request->setBackground(true);
        
        QList<QVariant> args;
        args << callUri.toString();
        request->setArguments(args);
        
        QVariant uriHandled = false;
        bool requestOk = request->send(uriHandled);
        if (uriHandled.toBool()) {
            notifDialog->setText("URI call accepted");
        } else {
            /* URI is not handled in the following cases:
             * 1. URI is invalid or not supported and parsing has failed. 
             * 2. User has canceled the call request. 
             * 3. Phone has rejected call request with parsed phone number.*/
            notifDialog->setText("URI call rejected");
        }
    } else {
        notifDialog->setText("Unknown scheme");
    }
    notifDialog->show();
}


void UriSchemeServiceTestAppView::clearInputField(bool checked)
{
    Q_UNUSED(checked)
    
    m_lineEdit->setText("");
}


void UriSchemeServiceTestAppView::inputSpecialCharacter(bool checked)
{
    Q_UNUSED(checked)
    
    QString textBeforeTapCharInsertion = m_lineEdit->text();
    
    QString tapCharacter = "";
    if (m_multitapTimer.elapsed() <= KMultitapTimeInMs) {
        m_multitapIndex = ++m_multitapIndex % m_multitapCount;
        textBeforeTapCharInsertion.chop(1);
    } else {
        m_multitapIndex = 0;
    }
    
    int key = m_multitapCharacters.at(m_multitapIndex);
    tapCharacter = m_extraChar.value(key);
    
    QString textAfterTapCharInsertion = 
        textBeforeTapCharInsertion + tapCharacter;
    m_lineEdit->setText(textAfterTapCharInsertion);
    
    m_multitapTimer.start();
}


void UriSchemeServiceTestAppView::setupTelUriConfiguration(int state)
{
    if (Qt::Checked == state) {
        m_uriLabel->setPlainText(KTelUriSchemePrefix);

        disconnect(
            m_ctiUriCheckBox, SIGNAL(stateChanged(int)), 
            this, SLOT(setupCtiUriConfiguration(int)));
        m_ctiUriCheckBox->setChecked(false);
        connect(
            m_ctiUriCheckBox, SIGNAL(stateChanged(int)), 
            this, SLOT(setupCtiUriConfiguration(int)));
        
        m_multitapIndex = 0;
        m_multitapCount = KNumOfTelMultitapCharacters;
        m_multitapCharacters.clear();
        m_multitapCharacters.append(Qt::Key_Asterisk);
        m_multitapCharacters.append(Qt::Key_NumberSign);
        m_multitapCharacters.append(Qt::Key_P);
        m_multitapCharacters.append(Qt::Key_W);
    } else {
        m_uriLabel->setPlainText("");
    }
}


void UriSchemeServiceTestAppView::setupCtiUriConfiguration(int state)
{
    if (Qt::Checked == state) {
        m_uriLabel->setPlainText(KCtiUriSchemePrefix);

        disconnect(
            m_telUriCheckBox, SIGNAL(stateChanged(int)), 
            this, SLOT(setupTelUriConfiguration(int)));
        m_telUriCheckBox->setChecked(false);
        connect(
            m_telUriCheckBox, SIGNAL(stateChanged(int)), 
            this, SLOT(setupTelUriConfiguration(int)));
        
        m_multitapIndex = 0;
        m_multitapCount = KNumOfCtiMultitapCharacters;
        m_multitapCharacters.clear();
        m_multitapCharacters.append(Qt::Key_Comma);
        m_multitapCharacters.append(Qt::Key_Slash);
    } else {
        m_uriLabel->setPlainText("");
    }
}
