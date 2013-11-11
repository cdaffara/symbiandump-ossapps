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
* Description:  stub CVideoCollectionWrapper class for unitesting CVideoListData
* 
*/

#ifndef __VIDEOCOLLECTIONWRAPPER_H__
#define __VIDEOCOLLECTIONWRAPPER_H__

#include <qvariant.h>  

class VideoCollectionWrapper      
{    

public: // Constructor
    
    /**
     * Returns singleton instance for this class.
     * 
     * WARNING! Not safe to call this from destructor of another function scope static object!
     * 
     * @return The singleton instance.
     */
    static VideoCollectionWrapper &instance();
 
	    
    /**
     * stub method for testing, saves mLatestStatusCode and 
     * mLatesAdditional
     */
     void sendAsyncStatus(int statusCode, QVariant &additional);
     
    /**
     * lates error code gotten from senderror
     */
    static int mLatestStatusCode;
    
    /**
     * latest additional data saved from senderror
     */
    static QVariant mLatestAdditional;
    
private:

    
    /**
     * Private contructor.
     */    
    VideoCollectionWrapper(); 
    
    /**
     * Private destructor.
     *
     */
    virtual ~VideoCollectionWrapper();  

};

#endif  // __VIDEOCOLLECTIONWRAPPER_H__
// End of file
    


