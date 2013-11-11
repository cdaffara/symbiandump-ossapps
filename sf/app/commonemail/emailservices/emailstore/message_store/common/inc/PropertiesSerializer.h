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
* Description:  Property serializer.
*
*/



#ifndef __PROPERTIES_SERIALIZER_H__
#define __PROPERTIES_SERIALIZER_H__

// ========
// INCLUDES
// ========

#include <e32std.h>

// =======
// CLASSES
// =======

// ==============================
// CLASS: TPropertiesDeserializer
// ==============================
class TPropertiesDeserializer
    {
    public:
    
        // ==============
        // PUBLIC METHODS
        // ==============
    
        inline TPropertiesDeserializer( const TDesC8& aBuf ) : 
            iReadOnlyBuf( aBuf.Ptr(), aBuf.Length() ) {iPointer = iReadOnlyBuf.Ptr(); iLength=0;}
    
        TBool Find( const TDesC8& aNameToFind );
            
        TBool First() {iPointer = iReadOnlyBuf.Ptr(); return SetFields();}
            
        inline TBool Next() {iPointer += iLength; return SetFields();}    
    
        inline TUint8 Type() {return iType;}
   
        inline const TPtrC8& Name() {return iName;}
    
        inline const TPtrC8& Value() {return iValue;}
            
    protected:
    
        // =================
        // PROTECTED METHODS
        // =================

        TBool SetFields();
        
        // ====================
        // PROTECTED ATTRIBUTES
        // ====================

        TPtrC8        iReadOnlyBuf;
        const TUint8* iPointer;
        TUint         iLength; 
        TPtrC8        iValue;
        TPtrC8        iName;
        TUint8        iType;

    }; // end class TPropertiesDeserializer
    
// ============================
// CLASS: TPropertiesSerializer
// ============================
class TPropertiesSerializer : public TPropertiesDeserializer
    {
    public:

        // ================
        // PUBLIC CONSTANTS
        // ================
    
        enum {EFixedOverhead = 0};
    
        enum {EPerNodeOverhead = 8};  // this is the worst case, when value length > KMaxUint16
    
        // ==============
        // PUBLIC METHODS
        // ==============
    
        inline TPropertiesSerializer( RBuf8& aBuf ) : 
            TPropertiesDeserializer( aBuf ),
            iWriteableBuf( aBuf ) {}
    
        void AddPropertyL( const TDesC8& aName, TUint8 aType, const TDesC8& aValue );
            
        void UpdateValueL( const TDesC8& aNewValue );
        
        // returns ETrue if there are more properties after the removed one.
        TBool RemovePropertyL();
           
    private:
    
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
    
        RBuf8& iWriteableBuf;                
    
    }; // end class TPropertiesSerializer

#endif
