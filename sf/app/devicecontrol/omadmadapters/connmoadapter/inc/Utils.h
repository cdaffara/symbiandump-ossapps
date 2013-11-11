/*
* ==============================================================================
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
* ==============================================================================
*/

#ifndef UTILS_H_
#define UTILS_H_

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "ConnMoAdapter.h"

class TLuid;

/**
 * Utilities class for ConnMo adapter needs
 * 
 * Class contains useful helper functions etc. for parsing URI and TLuid
 */
class CUtils : public CBase
    {
public:
    
    virtual ~CUtils();
    
    /**
     * Returns first segment of URI.
     * 
     * @param aUri URI to be searched.
     * @return First segment of URI.
     */
    static TPtrC8 GetFirstURISeg( const TDesC8& aURI );
    
    /**
     * Returns second segment of URI.
     * 
     * @param aUri URI to be searched.
     * @return Second segment of URI.
     */
    static TPtrC8 GetSecondURISeg( const TDesC8& aURI );
    
    /**
     * Returns last segment of URI.
     * 
     * @param aUri URI to be searched.
     * @return Last segment of URI.
     */
    static TPtrC8 GetLastURISeg( const TDesC8& aURI );
    
    /**
     * Returns the end of URI starting from passed segment.
     * 
     * @param aUri URI to be parsed.
     * @param aName Segment to start.
     * @return End of URI starting from aName.
     */
    static TPtrC8 GetEndOfURIFromSeg( const TDesC8& aURI, const TDesC8& aName );
    
    /**
     * This function converts string value to 8-bit value.
     * 
     * @param aSource String to convert.
     * @return Pointer to 8-bit value.
     */
    static TDesC8& ConvertTo8LC( const TDesC& aSource );
    
    /**
     * This function converts string value to 16-bit value.
     * 
     * @param aSource String to convert.
     * @return Pointer to 16-bit value.
     */
    static TDesC16& ConvertTo16LC( const TDesC8& aSource );
    
    /**
     * This function sets given information to given node.
     * 
     * @param aNode Target node.
     * @param aAccTypes Node's access types.
     * @param aOccurrence Amount of possible nodes of same type.
     * @param aScope Describes if node is static or dynamic.
     * @param aFormat Node's format.
     * @param aDescription String containing description of the node.
     * @return Pointer integer value.
     */
    static void FillNodeInfoL( MSmlDmDDFObject& aNode,
                               TSmlDmAccessTypes aAccTypes,
                               MSmlDmDDFObject::TOccurence aOccurrence, 
                               MSmlDmDDFObject::TScope aScope, 
                               MSmlDmDDFObject::TDFFormat aFormat,
                               const TDesC8& aDescription);
        
    /**
     * This function removes characters that are not allowed in URI from given descriptor.
     * 
     * @param aString String to be modified.
     */
    static void RemoveIllegalCharsFromURI( TDes8& aString );
        
    /**
     * This function returns string as bool value.
     * 
     * @param aSource String to convert.
     * @param aBool conversion result.
     * @return TInt32 KErrNone if succesful conversion.
     */
    static TInt32 DesToBool( const TDesC8& aString, TBool& aBool );
    
    /**
     * This function convert integer value to string presentation.
     * 
     * @param aObject Int value to convert.
     * @return TDesC8& Int as string.
     */
    static TDesC8& IntToCharL( const TInt& aObject );
    
    /**
     * This function takes first segment from uri and adds it to target
     * 
     * @param aURI Contains source uri.
     * @param aTarget Target string.
     * @return TInt Index of first uri delimiter.
     */
    static TInt AppendFirstURISeg( TDes8& aURI, TDes8& aTarget );
    
    static TInt IntLUID(const TDesC8& aLUID);
    
private:
    
    //-----------------------------------------------------------------------------
    // Private functions
    //-----------------------------------------------------------------------------
    CUtils();
              
    //-----------------------------------------------------------------------------
    // Private member variables
    //-----------------------------------------------------------------------------
       
    };

#endif /* UTILS_H_ */
