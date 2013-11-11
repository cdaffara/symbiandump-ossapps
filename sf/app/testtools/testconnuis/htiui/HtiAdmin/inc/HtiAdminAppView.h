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
* Description:  App view displays 2 labels.
*
*/


#ifndef __HTIADMIN_APPVIEW_H__
#define __HTIADMIN_APPVIEW_H__

// INCLUDES
#include <coecntrl.h>

// CONSTANTS
const static TInt KMaxDisplayText = 64;

// CLASS DECLARATION
/**
    An instance of the Application View object for the HtiAdmin
  example application
  */
class CHtiAdminAppView : public CCoeControl
    {
public:

/**
    Create a CHtiAdminAppView object, which will draw itself to aRect
  @param aRect the rectangle this view will be drawn to
  @return a pointer to the created instance of CHtiAdminAppView
  */
    static CHtiAdminAppView* NewL(const TRect& aRect);

/**
    Create a CHtiAdminAppView object, which will draw itself to aRect
  @param aRect the rectangle this view will be drawn to
  @return a pointer to the created instance of CHtiAdminAppView
  */
    static CHtiAdminAppView* NewLC(const TRect& aRect);


/**
    Destroy the object and release all memory objects
  */
     ~CHtiAdminAppView();

    /**
    Set text for the labels in the view.
    */
    void SetCaption( const TDesC& aText );
    void SetStatus( const TDesC& aText );
    void SetVersionCaption( const TDesC& aText );
    void SetVersion( const TDesC& aText );
    void SetAutoStartCaption( const TDesC& aText );
    void SetAutoStartStatus( const TDesC& aText );
    void SetSelectedCommCaption( const TDesC& aText );
    void SetSelectedComm( const TDesC& aText );
    void SetCommDetails( const TDesC& aText );

public:  // from CCoeControl
/**
    Draw this CHtiAdminAppView to the screen
  @param aRect the rectangle of this view that needs updating
  */
    void Draw( const TRect& aRect ) const;


private:

/**
    Perform the second phase construction of a CHtiAdminAppView object
  @param aRect the rectangle this view will be drawn to
  */
    void ConstructL( const TRect& aRect );

/**
    Perform the first phase of two phase construction
  */
    CHtiAdminAppView();

private:
    /**
    text to show in the view
    */
    HBufC* iHtiCaption;
    HBufC* iHtiStatus;
    HBufC* iHtiVersionCaption;
    HBufC* iHtiVersion;
    HBufC* iHtiAutoStartCaption;
    HBufC* iHtiAutoStartStatus;
    HBufC* iHtiSelectedCommCaption;
    HBufC* iHtiSelectedComm;
    HBufC* iHtiCommDetails;
    };


#endif // __HTIADMIN_APPVIEW_H__


// End of File
