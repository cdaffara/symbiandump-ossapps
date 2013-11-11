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
* Description:  Property serializer implementation.
*
*/



// ========
// INCLUDES
// ========

#include <e32base.h>

#include "PropertiesSerializer.h"

// =========
// CONSTANTS
// =========

const TUint16 KLongPropertyIndicator = KMaxTUint16;

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: Find
// ==========================================================================
TBool TPropertiesDeserializer::Find( const TDesC8& aNameToFind )
    {
    TBool returnValue = First();
    
    while( returnValue && iName.Compare( aNameToFind ) != 0 )
        {
        returnValue = Next();
        } // end while         
        
    return returnValue;                
    } // end Find
    
// ==========================================================================
// FUNCTION: SetFields
// ==========================================================================
TBool TPropertiesDeserializer::SetFields()
    {
    TBool returnValue;
    
    if( iPointer >= iReadOnlyBuf.Ptr() + iReadOnlyBuf.Length() )
        {
        // Past the end.
        returnValue = EFalse;
        
        iLength = 0;
        iName.Set( 0,0 );
        iValue.Set( 0, 0 );
        }
    else
        {                
        // Not past the end. 
        returnValue = ETrue;

        const TUint8* tempPointer = iPointer;
                   
		TUint8 nameLength = *tempPointer;
		tempPointer++;
		
		iName.Set( tempPointer, nameLength );
		tempPointer += nameLength;

        iType = *tempPointer;
		tempPointer++;

        TUint32 valueLength;

		// Need to pull the integer values out of the buffer to look at them since they
		// might not be properly word aligned.			    			
		TPckgBuf<TUint16> valueLengthPckg;

		valueLengthPckg.Copy( tempPointer, 2 );
		tempPointer += 2;
		
		if( valueLengthPckg() != KLongPropertyIndicator )
		    {
		    // The length fit into 16 bits.
            valueLength = valueLengthPckg();		    
		    }
		else
		    {
		    // Extract the 32 bit length.
    		TPckgBuf<TUint32> longValueLengthPckg;

    		const TUint KTempPointerLength = 4; 
    		longValueLengthPckg.Copy( tempPointer, KTempPointerLength );
    		tempPointer += KTempPointerLength;
    		
		    valueLength = longValueLengthPckg();
		    		    
		    } // end if

		iValue.Set( tempPointer, valueLength );
		tempPointer += valueLength;
		
		// Update the length.
        iLength = tempPointer - iPointer;
    
        } // end if  

    return returnValue;                          
    } // end SetFields
    
// ==========================================================================
// FUNCTION: AddPropertyL
// ==========================================================================
void TPropertiesSerializer::AddPropertyL( const TDesC8& aName, TUint8 aType, const TDesC8& aValue )
    {
    TUint newLength = iWriteableBuf.Length() + aName.Length() + aValue.Length() + EPerNodeOverhead;
                                   
    if( newLength > iWriteableBuf.MaxLength() )
        {
        // Grow the buffer.
        
        TUint offset = iPointer - iWriteableBuf.Ptr();
    
        RBuf8 tempBuf;
        tempBuf.CreateL( newLength );
        CleanupClosePushL( tempBuf );
        
        tempBuf.Copy( iWriteableBuf );
        iWriteableBuf.Swap( tempBuf );
        
        CleanupStack::PopAndDestroy( &tempBuf );

        iPointer = iWriteableBuf.Ptr() + offset;
    
        if( iLength > 0 )
            {            
            iReadOnlyBuf.Set( iWriteableBuf );
            SetFields();
            } // end if
        
        } // end if

    TUint16 valueLength = 0;
    TUint32 longValueLength = 0;
    
    TPckg<TUint16> valueLengthPckg( valueLength );
    TPckg<TUint32> longValueLengthPckg( longValueLength );
        
    if( aValue.Length() < KLongPropertyIndicator )
        {
        valueLength = aValue.Length();
        }
    else
        {
        valueLength = KLongPropertyIndicator;
        longValueLength = aValue.Length();        
        } // end if            

    iWriteableBuf.Append( aName.Length() );
    iWriteableBuf.Append( aName );
    iWriteableBuf.Append( aType );
    iWriteableBuf.Append( valueLengthPckg );
    
    if( longValueLength > 0 )
        {
        iWriteableBuf.Append( longValueLengthPckg );        
        } // end if
    
    iWriteableBuf.Append( aValue );
    
    iReadOnlyBuf.Set( iWriteableBuf );
    
    } // end AddPropertyL
    
// ==========================================================================
// FUNCTION: UpdateValueL
// ==========================================================================
void TPropertiesSerializer::UpdateValueL( const TDesC8& aNewValue )
    {
    TInt valueOffset = iValue.Ptr() - iWriteableBuf.Ptr();
        
    const TUint KBytesCount =4;
    const TUint KLengthBuf = 6;
    
    TUint newLength = iWriteableBuf.Length() + aNewValue.Length() - iValue.Length();

    TInt oldValueLengthLength; // the old length of the "value length" field
    
    if( iValue.Length() < KLongPropertyIndicator )
        {
        oldValueLengthLength = 2;

        if (aNewValue.Length() >= KLongPropertyIndicator )        
            {
            // Value length has grown beyond 16-bits and now takes an extra 32-bits to store.
            newLength += KBytesCount;
            } // end if
        }
    else
        {
        oldValueLengthLength = KLengthBuf;
        } // end if
                    
    if( newLength > iWriteableBuf.MaxLength() )
        {  
        // Grow the buffer.
                          
        TUint offset = iPointer - iWriteableBuf.Ptr();
    
        RBuf8 tempBuf;
        tempBuf.CreateL( newLength );
        CleanupClosePushL( tempBuf );
        
        tempBuf.Copy( iWriteableBuf );
        iWriteableBuf.Swap( tempBuf );
        
        iPointer = iWriteableBuf.Ptr() + offset;
        
        CleanupStack::PopAndDestroy( &tempBuf );

        } // end if        

    // Build the new "value length" field.

    TBuf8<KLengthBuf> newValueLengthBuf;

    TUint16 valueLength = 0;
    TUint32 longValueLength = 0;
    
    TPckg<TUint16> valueLengthPckg( valueLength );
        
    if( aNewValue.Length() < KLongPropertyIndicator )
        {
        valueLength = aNewValue.Length();
        newValueLengthBuf.Append( valueLengthPckg );
        }
    else
        {
        valueLength = KLongPropertyIndicator;

        TPckg<TUint32> longValueLengthPckg( longValueLength );
        longValueLength = aNewValue.Length();        
        
        newValueLengthBuf.Append( valueLengthPckg );
        newValueLengthBuf.Append( longValueLengthPckg );
        } // end if            
        
    // Overwrite the old value field.
    iWriteableBuf.Replace( valueOffset, iValue.Length(), aNewValue );
        
    // Overwrite the old value length field.        
    iWriteableBuf.Replace( valueOffset - oldValueLengthLength, oldValueLengthLength, newValueLengthBuf );
    
    iPointer += (newValueLengthBuf.Length() - oldValueLengthLength);
    iReadOnlyBuf.Set( iWriteableBuf );
    SetFields();
        
    } // end UpdateValueL
               
// ==========================================================================
// FUNCTION: RemovePropertyL
// ==========================================================================
TBool TPropertiesSerializer::RemovePropertyL()
    {
    TUint offset = iPointer - iWriteableBuf.Ptr();
    
    iWriteableBuf.Delete( offset, iLength );
    
    iReadOnlyBuf.Set( iWriteableBuf );

    return SetFields();
                            
    } // end RemovePropertyL
           
