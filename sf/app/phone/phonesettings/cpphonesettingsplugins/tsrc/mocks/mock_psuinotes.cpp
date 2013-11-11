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
#include <QDebug>
#include <QString>
#include <smcmockclassincludes.h>
#include <psuinotes.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PsUiNotes::PsUiNotes
// -----------------------------------------------------------------------------
//
PsUiNotes::PsUiNotes(  )
    //:
    //QObject( /**/ )
    {
    
    }


// -----------------------------------------------------------------------------
// PsUiNotes::instance
// -----------------------------------------------------------------------------
//
PsUiNotes * PsUiNotes::instance(  )
    {
    static PsUiNotes theInstance;
    return &theInstance; 
    }
// -----------------------------------------------------------------------------
// PsUiNotes::~PsUiNotes
// -----------------------------------------------------------------------------
//
PsUiNotes::~PsUiNotes(  )
    {

    }

// -----------------------------------------------------------------------------
// PsUiNotes::showGlobalProgressNote
// -----------------------------------------------------------------------------
//
void PsUiNotes::showGlobalProgressNote( 
        int & noteId,
        const QString & text )
    {
    SMC_MOCK_METHOD2( void, int &, noteId, 
        const QString &, text )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::showGlobalNote
// -----------------------------------------------------------------------------
//
void PsUiNotes::showGlobalNote( 
        int & noteId,
        const QString & text,
        HbMessageBox::MessageBoxType noteType )
    {
    SMC_MOCK_METHOD3( void, int &, noteId, 
        const QString &, text, 
        HbMessageBox::MessageBoxType, noteType )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::showGlobalErrorNote
// -----------------------------------------------------------------------------
//
void PsUiNotes::showGlobalErrorNote( 
        int & noteId,
        int errorCode )
    {
    SMC_MOCK_METHOD2( void, int &, noteId, 
        int, errorCode )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::showCallDivertDetails
// -----------------------------------------------------------------------------
//
void PsUiNotes::showCallDivertDetails( 
        const QList<PSCallDivertingStatus *> & divertingStatusList )
    {
    SMC_MOCK_METHOD1( void, const QList<PSCallDivertingStatus *> &, divertingStatusList )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::showPasswordQueryDialog
// -----------------------------------------------------------------------------
//
void PsUiNotes::showPasswordQueryDialog( 
        const QString & title,
        const QValidator & validator,
        int maxPasswordLength )
    {
    SMC_MOCK_METHOD3( void, const QString &, title, 
        const QValidator &, validator, 
        int, maxPasswordLength )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::cancelNote
// -----------------------------------------------------------------------------
//
void PsUiNotes::cancelNote( 
        int noteId )
    {
    SMC_MOCK_METHOD1( void, int, noteId )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::noteShowing
// -----------------------------------------------------------------------------
//
bool PsUiNotes::noteShowing(  )
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::finishedPasswordQueryDialog
// -----------------------------------------------------------------------------
//
void PsUiNotes::finishedPasswordQueryDialog( 
        HbAction * action )
    {
//    SMC_MOCK_METHOD1( void, HbAction *, action )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::showNotificationDialog
// -----------------------------------------------------------------------------
//
void PsUiNotes::showNotificationDialog( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::showGlobalNotificationDialog
// -----------------------------------------------------------------------------
//
void PsUiNotes::showGlobalNotificationDialog( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// PsUiNotes::activeNoteAboutToClose
// -----------------------------------------------------------------------------
//
void PsUiNotes::activeNoteAboutToClose()
{
    SMC_MOCK_METHOD0( void )
}


// -----------------------------------------------------------------------------
// PsUiNotes::handleProgressNoteCanceled
// -----------------------------------------------------------------------------
//
void PsUiNotes::handleProgressNoteCanceled()
{
    SMC_MOCK_METHOD0( void )
}


// -----------------------------------------------------------------------------
// PsUiNotes::passwordTextChanged
// -----------------------------------------------------------------------------
//


void PsUiNotes::passwordTextChanged()
{
    SMC_MOCK_METHOD0( void )
}
