/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CCUSTOMNOTEDIALOG_H
#define CCUSTOMNOTEDIALOG_H

#include <aknnotedialog.h>
#include <AknProgressDialog.h> // mprogressdialogcallback

class CCustomNoteDialog : public CAknNoteDialog                                                           
    {
    public:
        static CCustomNoteDialog* NewL( CEikDialog** aSelfPtr );
        static CCustomNoteDialog* NewLC( CEikDialog** aSelfPtr );
        ~CCustomNoteDialog();
        
        CCustomNoteDialog( CEikDialog** aSelfPtr );
        
    public:    // Methods derived from MProgressDialogCallback  
        void SetCallback( MProgressDialogCallback* aCallback );
        
    private:  
        void ConstructL();
        
    private:
        MProgressDialogCallback* iCallback;
    
    };
    
#endif  // CCUSTOMNOTEDIALOG_H

//  End of File
