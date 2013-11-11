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

#include "stub/inc/mpxcollectionutility.h"
#include "stub/inc/mpxcollectionpath.h"

/**
 * if true MMPXCollectionUtility::NewL leaves
 */
bool gLeaveNewL = false;

/**
 * if true, MMPXCollection::PathL leaves
 */
bool gLeavePathL = false;

/**
 * if true, MMPXCollection::OpenL(CMPXCollectionPath&) leaves
 */
bool gLeaveOpenPath = false;

/**
 * if true, MMPXCollection::OpenL(CMPXMedia&) leaves
 */
bool gLeaveOpenMedia = false;

/**
 * if true, MMPXCollection::OpenL() leaves
 */
bool gLeaveOpen = false;

/**
 * if true MPXCOllection::CommandL leaves
 */
bool gLeaveCommand = false;

/**
 * if true, MMPXCollection::BackL() leaves
 */
bool gLeaveBack = false;

int gUtilCloseCounter = 0;

// -----------------------------------------------------------------------------
// setNewLLeave
// -----------------------------------------------------------------------------
//
void MMPXCollectionUtility::setNewLLeave(bool leave)
{
    gLeaveNewL = leave;
}

// -----------------------------------------------------------------------------
// resetCloseCounter
// -----------------------------------------------------------------------------
//
void MMPXCollectionUtility::resetCloseCounter()
{
    gUtilCloseCounter = 0;
}
    
// -----------------------------------------------------------------------------
// getCloseCounter
// -----------------------------------------------------------------------------
//
int MMPXCollectionUtility::getCloseCounter()
{
    return gUtilCloseCounter;
}

// -----------------------------------------------------------------------------
// setPathLLeave
// -----------------------------------------------------------------------------
//
void MMPXCollection::setPathLLeave(bool leave)
{
    gLeavePathL = leave;
}

// -----------------------------------------------------------------------------
// setOpenLPathLeave
// -----------------------------------------------------------------------------
//
void MMPXCollection::setOpenLPathLeave(bool leave)
{
    gLeaveOpenPath = leave;       
}

// -----------------------------------------------------------------------------
// setOpenLMediaLeave
// -----------------------------------------------------------------------------
//
void MMPXCollection::setOpenLMediaLeave(bool leave)
{
    gLeaveOpenMedia = leave;
}

// -----------------------------------------------------------------------------
// setOpenLLeave
// -----------------------------------------------------------------------------
//
void MMPXCollection::setOpenLLeave(bool leave)
{
    gLeaveOpen = leave;
}

// -----------------------------------------------------------------------------
// setBackLLeave
// -----------------------------------------------------------------------------
//
void MMPXCollection::setBackLLeave(bool leave)
{
	gLeaveBack = leave;
}

// -----------------------------------------------------------------------------
// setCommandLLeave
// -----------------------------------------------------------------------------
//
void MMPXCollection::setCommandLLeave(bool leave)
{
    gLeaveCommand = leave;
}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
MMPXCollectionUtility* MMPXCollectionUtility::NewL(MMPXCollectionObserver* /*aObs*/,
                                                   const TUid& /*aModeId*/ )
{
    if(gLeaveNewL)
    {
        User::Leave(KErrGeneral);
    }
    static MMPXCollectionUtility collectionUtilility;
    return &collectionUtilility;

}

// -----------------------------------------------------------------------------
// MMPXCollectionUtility
// -----------------------------------------------------------------------------
//
MMPXCollectionUtility::MMPXCollectionUtility()
{
    
}
  
// -----------------------------------------------------------------------------
// ~MMPXCollectionUtility
// -----------------------------------------------------------------------------
//
MMPXCollectionUtility::~MMPXCollectionUtility()
{
    
}

// -----------------------------------------------------------------------------
// Collection
// -----------------------------------------------------------------------------
//
MMPXCollection& MMPXCollectionUtility::Collection()
{
    return mCollection;
}

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
void MMPXCollectionUtility::Close()
{
    gUtilCloseCounter++;
}

// -----------------------------------------------------------------------------
// PathL
// -----------------------------------------------------------------------------
//
CMPXCollectionPath* MMPXCollection::PathL()
{
    if(gLeavePathL)
    {
        User::Leave(KErrGeneral);
    }

    return new CMPXCollectionPath();        
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
//
void MMPXCollection::OpenL(CMPXCollectionPath& /*path*/)
{
    if(gLeaveOpenPath)
    {    
        User::Leave(KErrGeneral);
    }
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
//
void MMPXCollection::OpenL(CMPXMedia& /*media*/)
{
    if(gLeaveOpenMedia)
    {
        User::Leave(KErrGeneral);
    }
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
//
void MMPXCollection::OpenL(TInt /*aIndex*/)
{
    if(gLeaveOpenMedia)
    {
        User::Leave(KErrGeneral);
    }
}

// -----------------------------------------------------------------------------
// OpenL
// -----------------------------------------------------------------------------
//
void MMPXCollection::OpenL()
{
    if(gLeaveOpen)
    {
        User::Leave(KErrGeneral);
    }
}


// -----------------------------------------------------------------------------
// BackL
// -----------------------------------------------------------------------------
//
void MMPXCollection::BackL()
{
    if(gLeaveBack)
    {
        User::Leave(KErrGeneral);
    }
}


// -----------------------------------------------------------------------------
// CommandL
// -----------------------------------------------------------------------------
//
void MMPXCollection::CommandL(CMPXCommand& /*cmd*/)
{
    if(gLeaveCommand)
    {
        User::Leave(KErrGeneral);
    }
}
    
