/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QCoreApplication>
#include <HbAction>
#include <HbDialog>
#include <HbMessageBox>
#include <HbLabel>
#include <HbPushButton>

#include "cxutils.h"
#include "cxeerror.h"
#include "cxeengine.h"
#include "cxuienums.h"
#include "cxuierrormanager.h"
#include "cxuidocumentloader.h"
#include "cxecameradevicecontrol.h"


/*!
* Constructor
*/
CxuiErrorManager::CxuiErrorManager(CxuiDocumentLoader *documentLoader) :
    mDocumentLoader(documentLoader),
    mErrorMsgPopup(NULL),
    mErrorId(CxeError::None),
    mErrorSeverity(CxuiErrorManager::None)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxuiErrorManager::~CxuiErrorManager()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Check the error code and show either error popup, warning popup or do nothing,
* if "no error" code is given.
* @param error Error id. If CxeError::None, no action is taken. Otherwise
* either warning or error popup is shown based on the severity of error.
*
*/
void CxuiErrorManager::check(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    mErrorSeverity = CxuiErrorManager::None;

    if (error != CxeError::None) {
        mErrorId = error;

        // start evaluating the error.
        QString errorText;
        QString buttonText;
        getErrorDetails(errorText, buttonText);

        if (mErrorSeverity != CxuiErrorManager::None) {
            // Clear the old error if one for some reason exists.
            clear();

            // show the error note to the user.
            launchPopup(errorText, buttonText);
        } else {
            // ignore
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Close the open error popup.
*/
void CxuiErrorManager::clear()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mErrorMsgPopup) {
        mErrorMsgPopup->close();
        mErrorMsgPopup = NULL;
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot that gets called when error note is closed.
*/
void CxuiErrorManager::popupClosed(HbAction *action)
{
    Q_UNUSED(action)

    CX_DEBUG_ENTER_FUNCTION();
    // Dialog or action instance cannot be used anymore.
    mErrorMsgPopup = NULL;

    // handle any use-cases when the error can be recovered
    emit errorPopupClosed();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Helper method for closing the application.
*/
void CxuiErrorManager::closeApp()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::quit();
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Helper method to get the error message to use for showing note to user,
* and set the severity level, based on given error code.
* @param error Error code to be analyzed.
*/
void CxuiErrorManager::getErrorDetails(QString &errorText, QString &buttonText)
{
    CX_DEBUG_ENTER_FUNCTION();
    switch (mErrorId) {
        case CxeError::MemoryNotAccessible:
            mErrorSeverity = CxuiErrorManager::Error;
            errorText = hbTrId("txt_cam_info_error_usb_disconnected");
            buttonText = hbTrId("txt_cam_info_error_usb_disconnected_button");
            break;
        case CxeError::InUse:
            mErrorSeverity = CxuiErrorManager::Error;
            errorText = hbTrId("txt_cam_info_camera_already_in_use");
            buttonText = hbTrId("txt_common_button_close");
            break;
        case CxeError::DiskFull:
            mErrorSeverity = CxuiErrorManager::Warning;
            errorText = hbTrId("txt_cam_info_memory_full");
            break;
        case CxeError::OutOfMemory:
            mErrorSeverity = CxuiErrorManager::Error;
            errorText = hbTrId("txt_cam_info_error_ram_full");
            buttonText = hbTrId("txt_common_button_ok");
            break;
        case CxeError::Died:
        case CxeError::InitializationFailed:
        case CxeError::HwNotAvailable:
        case CxeError::NotReady:
        default:
            mErrorSeverity = CxuiErrorManager::Error;
            errorText = hbTrId("txt_cam_info_error");
            buttonText = hbTrId("txt_common_button_close");
            break;
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show warning or error popup.
* @param errorText Message to be shown in the popup.
* @param buttonText Button text to be shown in the action button of the popup. Not used on warning popup.
*/
void CxuiErrorManager::launchPopup(const QString &errorText, const QString &buttonText)
{
    CX_DEBUG_ENTER_FUNCTION();

    switch (mErrorSeverity) {
    case CxuiErrorManager::None:
        break;
    case CxuiErrorManager::Warning:
        showWarningPopup(errorText);
        break;
    default:
        showErrorPopup(errorText, buttonText);
        break;
    }

    mErrorSeverity = CxuiErrorManager::None;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show error note for severe error.
*/
void CxuiErrorManager::showErrorPopup(const QString &errorText, const QString &buttonText)
{
    // we always prepare the popup for the new message and hence we load the
    // popup everytime from document loader

    CX_ASSERT_ALWAYS(mDocumentLoader);
    bool ok = false;

    // Use document loader to create popup
    mDocumentLoader->load(CxUiLayout::ERROR_POPUP_XML, &ok);
    CX_DEBUG(("mErrorMsgPopup load ok=%d", ok));

    mErrorMsgPopup = qobject_cast<HbDialog*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_POPUP));
    CX_ASSERT_ALWAYS(mErrorMsgPopup);
    mErrorMsgPopup->setAttribute(Qt::WA_DeleteOnClose, true);
    mErrorMsgPopup->setTimeout(HbDialog::NoTimeout);

    // HbDialog's default background item is replaced with black rectangle
    QGraphicsRectItem *backgroundItem = new QGraphicsRectItem();
    QBrush blackBrush = QBrush(Qt::black);
    backgroundItem->setBrush(blackBrush);
    QGraphicsItem *origBgItem = mErrorMsgPopup->backgroundItem();
    backgroundItem->setRect(origBgItem->boundingRect());
    mErrorMsgPopup->setBackgroundItem(backgroundItem);


    // color of standby text is set in the code. It cannot be done in docml
    HbLabel* label = qobject_cast<HbLabel*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_TEXT_WIDGET));
    label->setTextColor(Qt::white);
    label->setPlainText(errorText);

    HbPushButton *exitButton = qobject_cast<HbPushButton*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_BUTTON_WIDGET));
    if (!buttonText.isEmpty()) {
        // inform ui about error recovery
        exitButton->setText(buttonText);
        connect(exitButton, SIGNAL(released()), this, SLOT(closeApp()));
        exitButton->show();
    }

    emit errorPopupShown();
    mErrorMsgPopup->open(this, SLOT(popupClosed(HbAction*)));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show warning note for low severity error.
*/
void CxuiErrorManager::showWarningPopup(const QString &errorText)
{
    CX_DEBUG_ENTER_FUNCTION();
    HbMessageBox::warning(errorText);
    CX_DEBUG_EXIT_FUNCTION();
}
