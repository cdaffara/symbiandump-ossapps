/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PC Connectivity client
*
*/


#ifndef __SCONPCCONNCLIENT_H__
#define __SCONPCCONNCLIENT_H__

// -----------------------------------------------------------------------------
// class RSConPCConnSession - the client session
// -----------------------------------------------------------------------------
class RSConPCConnSession : public RSessionBase
	{
public:
	IMPORT_C RSConPCConnSession();
	IMPORT_C ~RSConPCConnSession();

        /**
        * Starts the server and connects to it
        * @param none
        * @return KerrNone or an error code
        */
	IMPORT_C TInt Connect();
	
	/**
        *  Disconnects from the server and terminates the server
        * @param none
        * @return none
        */
	IMPORT_C void Close();
	
	/**
        *  Passes an OBEX Put -message to the server
        * @param none
        * @return none
        */
	IMPORT_C TInt PutPacketL( const TDesC& aNameHeader, 
	                          const TDesC8& aTypeHeader, 
	                          CBufFlat*& aBuffer );
	
	/**
        *  Passes an OBEX Get -message to the server
        * @param none
        * @return none
        */
	IMPORT_C TInt GetPacketL( const TDesC& aNameHeader, 
	                          const TDesC8& aTypeHeader, 
	                          CBufFlat*& aBuffer );
	
	/**
        *  Passes an reset -message to the server
        * @param none
        * @return none
        */
	IMPORT_C TInt ResetServer();	

private:
	TVersion Version() const;
	TInt LaunchServer();
	TInt CreateAndSendChunkHandle();

private:
	RChunk iChunk;
	};
	
#endif