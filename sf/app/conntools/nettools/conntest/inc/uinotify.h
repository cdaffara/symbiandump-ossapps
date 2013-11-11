/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Interface class for displaying notifications.
*
*/

#ifndef __UINOTIFY_H__
#define __UINOTIFY_H__


/**
* This class specifies the console output functions. It
* can be used in conjunction with any class that want to
* display notifications in output window of the container.
*/
class MUINotify
{
public:
    
    /**
    * Displays the descriptor as text (with attributes) on the console
    * @param the text to be displayed
    * @param attributes e.g. CEikGlobalTextEditor::EItalic
    */
    virtual void PrintNotify(const TDesC& aMessage, TUint aAttributes = 0) = 0;
    
    /**
    * Displays the descriptor as text (with attributes) on the console
    * @param the text to be displayed
    * @param attributes e.g. CEikGlobalTextEditor::EItalic
    */
    virtual void PrintNotify(const TDesC8& aMessage, TUint aAttributes = 0) = 0;

    /**
    * Displays the integer as a decimal number on the console
    * @param aNumber the number to be displayed
    */
    virtual void PrintNotify(TInt aNumber) = 0;
    
    /**
    * Displays an error message
    * @param aErrMessage error message text
    * @param aErrCode error code number
    */
    virtual void ErrorNotify(const TDesC& aErrMessage, TInt aErrCode) = 0;

    /**
    * Notify user with a message
    * @param aMessage message associated
    */
    virtual void PopupNotify(const TDesC& aMessage) = 0;
    
    /**
    * Displays the text in the 'status' window
    * @param aStatus new status description text
    */
    virtual void SetStatus(const TDesC& aStatus) = 0;
};

#endif