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

#ifndef IRLASTPLAYEDSONGS_H
#define IRLASTPLAYEDSONGS_H

#include <e32base.h>

class RReadStream;
class RWriteStream;

class CIRLastPlayedSongs:public CBase
    {
    public:
        
    /**
     * Function : NewL
     * Function returns an instance of CIRLastPlayedSongs
     * Two phase constructor
     * @return instance of CIRLastPlayedSongs
     */
    IMPORT_C static CIRLastPlayedSongs* NewL();
    
    /**
     * CIRLastPlayedSongs::NewLC()
     * Static function
     * standard two phased constructor
     * @return *CIRLastPlayedSongs
     */
    IMPORT_C static CIRLastPlayedSongs* NewLC();
    
    /**
     * CIRLastPlayedSongs::~CIRLastPlayedSongs()
     * standard C++ destructor
     */
    ~CIRLastPlayedSongs();
    
    /**
     * CIRLastPlayedSongs::SetSongName()
     * Function to set song name from a TDesC
     * @param TDesC
     */
    IMPORT_C void SetSongName( const TDesC&aSongName );
    
    /**
     * CIRLastPlayedSongs::GetSongName()
     * Function to get song name from a TDesC
     * @return TDesc instance
     */
    IMPORT_C TDesC& GetSongName();

    /**
     * CIRLastPlayedSongs::SetArtistName()
     * Function to set artist name from a TDesC
     * @param TDesc
     */
    IMPORT_C void SetArtistName( const TDesC& aArtistName );
    
    /**
     * CIRLastPlayedSongs::GetArtistName()
     * Function to get artist name from a TDesC
     * @return TDesc instance
     */
    IMPORT_C TDesC& GetArtistName();
     
    /**
     * CIRLastPlayedSongs::ExternalizeL()
     * externalizes the data
     * @param RWriteStream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aWriteStream );
    
    /**
     * CIRLastPlayedSongs::InternalizeL()
     * internalizes the data
     * @param RWriteStream
     */
    IMPORT_C void InternalizeL( RReadStream& aReadStream );        
    
    /**
     * CIRLastPlayedSongs::=()
     * standard C++ copy constructor(deep copy) 
     * @param CIRLastPlayedSongs
     * @return CIRLastPlayedSongs reference
     */
    IMPORT_C CIRLastPlayedSongs& operator=( const CIRLastPlayedSongs& aSng );
    
    protected:
    /**
     * CIRLastPlayedSongs::ConstructL()
     * standard second phase constructor
     */
    void ConstructL();
    
private:
    HBufC* iSongName;
    HBufC* iArtistName;    
    };
    
#endif //IRLASTPLAYEDSONGS_H
