/*
* Copyright (c) 2005,2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides abstract ECOM interface for 3D ringing tone 
*
*/



#ifndef C3DRINGINGTONEINTERFACE_H
#define C3DRINGINGTONEINTERFACE_H

// INCLUDES
#include    <e32base.h>

// CONSTANTS
const TInt32 K3DRingingToneInterfaceUid = 0x1020299A;


enum T3DRingingToneInterfaceAttr
    {
    E3DRTIAttrProfileId = 0,
    E3DRTIAttr3DEffect = 1,
    E3DRTIAttr3DEcho = 2,
    E3DRTIAttr3DRingingTone = 3
    };

enum T3DRingingToneInterfacePlayerAttr
    {
    E3DRTIAttrAudioPlayerUtility = 10,
    E3DRTIAttrAudioToneUtility = 11,
    E3DRTIAttrDrmPlayerUtility = 12,
    E3DRTIAttrVideoPlayerUtility = 13
    };




// CLASS DECLARATION

/**
*  Abstract ECOM interface for playing 3D ringing tones.
*
*  @since 3.0
*/
class C3DRingingToneInterface : public CBase
    {
    public:
            
        /**
        * Two-phased constructor.
        *
        * @param aImplementationUid  For selection when there are more than one
        *                            implementations. Not used yet, for future.
        */
        inline static C3DRingingToneInterface* NewL( TUid aImplementationUid );
        
        /**
        * Destructor.
        */
        virtual ~C3DRingingToneInterface();
        
        /**
        * Set 3D ringing tone interface attribute with specific 
        * integer value.
        * @param aAttributeKey An enum of T3DRingingToneInterfaceAttr.
        * @param aValue Attribute integer value.
        * @return KErrNone, attribute set, no errors
        *         KErrArgument, attribute not set, argument error
        *         Some system wide Symbian error, attribute not set
        */
        virtual TInt SetAttr( T3DRingingToneInterfaceAttr aAttributeKey, 
                              TInt aValue ) = 0;

        /**
        * Set 3D ringing tone interface attribute with specific 
        * string value.
        * @param aAttributeKey An enum of T3DRingingToneInterfaceAttr.
        * @param aValue Attribute string value.
        * @return KErrNone, attribute set, no errors
        *         KErrArgument, attribute not set, argument error
        *         Some system wide Symbian error, attribute not set
        */
        virtual TInt SetAttr( T3DRingingToneInterfaceAttr aAttributeKey, 
                              const TDesC& aValue ) = 0;

        /**
        * Set 3D ringing tone interface attribute value any of type.
        * @param aAttributeKey This can be for example one of the 
        * player utility classes (see T3DRingingToneInterfacePlayerAttr).
        * This method is left extendable.
        * @param aValue Attribute value can be any.
        * @return KErrNone, attribute set, no errors
        *         KErrArgument, attribute not set, argument error
        *         Some system wide Symbian error, attribute not set
        */
        virtual TInt SetAttr( TInt aAttributeKey, TAny* aValue ) = 0;

        /**
        * Start playing 3D ringing tone.
        * Method can leave with specific Symbian system wide error code.
        * @param None.
        * @return None.
        */        
        virtual void PlayL() = 0;
        
        /**
        * Stop playing 3D ringing tone.
        * @param None.        
        * @return None.
        */        
        virtual void Stop() = 0;
            
    private:    // Data
        
        // Uid required by ECOM.
        TUid iDtor_ID_Key;
    };


#include    <c3dringingtoneinterface.inl>

#endif      // C3DRINGINGTONEINTERFACE_H
            
// End of File
