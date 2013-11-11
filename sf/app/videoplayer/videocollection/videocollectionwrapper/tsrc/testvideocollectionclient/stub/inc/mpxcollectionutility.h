/**
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
* Description:   stub mpxcollectionutiolity for testing CVideoCollectionClient
* 
*/

#ifndef MMPXCOLLECTIONUTILITY_H
#define MMPXCOLLECTIONUTILITY_H

#include <e32cmn.h>
#include <mpxcollectionframeworkdefs.h>
#include <mpxcommand.h>
#include "stub/inc/mpxcollectionpath.h"


class MMPXCollectionObserver;


class MMPXCollection
{
public:
    
    /**
     * set gLeavePathL 
     */
    static void setPathLLeave(bool leave);
    
    /**
     * set gLeaveOpenPath
     */
    static void setOpenLPathLeave(bool leave);
    
    /**
     * set gLeaveOpenMedia
     */
    static void setOpenLMediaLeave(bool leave);

    /**
     * set gLeaveOpen
     */
    static void setOpenLLeave(bool leave);

    /**
     * set gLeaveBack
     */
    static void setBackLLeave(bool leave);

    /**
     * set gLeaveCommand
     */
    static void setCommandLLeave(bool leave);
    
    /**
     * constructor
     */
    MMPXCollection(){};
    
    /**
     * destructor
     */
   ~MMPXCollection(){};
   
   /**
    * if gLeavePathL is true leaves KErrGeneral
    * if gLeavePathL is false returns address of new 
    * CMPXCollectionPath
    * 
    */
   CMPXCollectionPath* PathL();
   
   /**
    * if gLeaveOpenPath is true, leaves KErrGeneral
    */
   void OpenL(CMPXCollectionPath& path);
   
   /**
    * if gLeaveOpenMedia is true, levaes KErrGeneral
    */
   void OpenL(CMPXMedia& media);

   /**
    * if gLeaveOpenMedia is true, levaes KErrGeneral
    */
   void OpenL(TInt aIndex);
   
   /**
    * if gLeaveOpen is true leaves KErrGeneral
    */
   void OpenL();   

   /**
    * if gLeaveBack is true leaves KErrGeneral
    */
   void BackL();   

   /**
    * if gLeaveCommand is true, leaves KErrGeneral
    */
   void CommandL(CMPXCommand& cmd);
   
};



class MMPXCollectionUtility
    {
public:
    
    /**
     * sets gLeaveNewL
     */
    static void setNewLLeave(bool fails);
    
    /**
     * zeroes gUtilCloseCounter
     */
    static void resetCloseCounter();
    
    /**
     * returns gUtilCloseCounter
     */
    static int getCloseCounter();
    
    /**
     * Creates collection utility stub, if gLeaveNewL is true, leaves with KErrGeneral
     */
    static MMPXCollectionUtility* NewL(
                                        MMPXCollectionObserver* aObs = NULL,
                                        const TUid& aModeId = KMcModeDefault);
    
    /**
     * stub contructor
     */
    MMPXCollectionUtility();
        
    /**
     * stub desttructor
     */
    virtual ~MMPXCollectionUtility();

    /**
     *  reference to collection
     */
    MMPXCollection& Collection();

    /**
     * NOP
     */
    void Close();
    
private:
    
    /**
     * concrete collection object
     */
    MMPXCollection mCollection; 

};

#endif
