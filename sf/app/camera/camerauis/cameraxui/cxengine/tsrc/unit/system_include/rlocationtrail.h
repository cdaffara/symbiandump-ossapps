/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An interface to Location Trail.
*
*/

#ifndef R_RLOCATIONTRAIL_H
#define R_RLOCATIONTRAIL_H


/**
 *  Fake implementation of RLocationManager class which is used for creating a Location Manager session.
 *  Location Manager is used to start and stop the location trail, retrieve 
 *  location information and write the location information to images.
*/
class  RLocationTrail
    {
public:
    /**
     * Location trail states.
     */
    enum TTrailState
        {
        ETrailStopped,
        EWaitingGPSData,
        ESearchingGPS,
        ETrailStarted,
        ETrailStopping,
        ETrailStarting
        };

    enum TTrailCaptureSetting
    	{
    	EOff,
    	ECaptureNetworkInfo,
    	ECaptureAll 
    	};    

public:
	RLocationTrail();
	
	~RLocationTrail();

    /**
     * Starts recording location information to location trail.
     * @since S60 3.2
     * @param aState, an enumeration of ECaptureNetworkInfo  
     * (only cell ID stored) and ECaptureAll 
     * (GPS coordinates and cell ID stored).
     * @return KErrNone if successful, otherwise one of the other 
     *         system-wide error codes.
     */
    int StartLocationTrail(TTrailCaptureSetting aState);
    
    /**
     * Stops recording location information to location trail.
     * @since S60 3.1
     * @param None.
     * @return KErrNone if successful, otherwise one of the other 
     *         system-wide error codes.
     */
    int StopLocationTrail();
    
    
    /**
    * Connect to location trail server
    * @return KErrNone if successful, otherwise one of the other 
    *         system-wide error codes.
    **/
    int Connect();
    
    void Close();

private:
    bool mTrailConnected;
    bool mTrailStarted;
    };
    
#endif // R_RLOCATIONTRAIL_H

//End of File
