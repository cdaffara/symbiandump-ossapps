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

/*glxmlgenericobserver.h*/

#ifndef C_GLXMLGENERICOBSERVER_H
#define C_GLXMLGENERICOBSERVER_H

// Photos Headers
#include <mglxmedialistobserver.h>

// FORWARD DECLARATIONS
class CGlxMediaList;
class GlxMLWrapperPrivate;

// CLASS DECLARATION
class CGlxMLGenericObserver: public CBase,
                              public MGlxMediaListObserver

    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
   static CGlxMLGenericObserver* NewL(MGlxMediaList& aMediaList,
            GlxMLWrapperPrivate* aMLWrapperPrivate);

    /**
     * Destructor.
     */
    virtual ~CGlxMLGenericObserver();

public: // from MGlxMediaListObserver
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList*/* aList */);
    void HandleAttributesAvailableL( TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );    
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, 
    TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList );
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
    void HandleError( TInt aError );
    void HandleCommandCompleteL( CMPXCommand* aCommandResult, TInt aError, 
        MGlxMediaList* aList );
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
    void HandlePopulatedL(MGlxMediaList* aList);

private:

    /**
     * C++ default constructor.
     */
    CGlxMLGenericObserver(MGlxMediaList& aMediaList,GlxMLWrapperPrivate* iMLWrapperPrivate);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
private:
    // Instance of Media List
    MGlxMediaList& iMediaList;
    // Instance of GlxMLWrapperPrivate
    GlxMLWrapperPrivate* iMLWrapperPrivate;
   
    TSize iGridIconSize; //?
    };


#endif  // C_GLXMLGENERICOBSERVER_H

// End of File
