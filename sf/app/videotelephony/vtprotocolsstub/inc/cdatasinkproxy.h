/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data Sink Proxy interface
*
*/

#ifndef CDATASINKPROXY_H
#define CDATASINKPROXY_H

NONSHARABLE_CLASS( CDisplaySinkProxy ) : public CBase
    {
    public:
        /**
         * Constructor.
         */
        CDisplaySinkProxy(MVTVideoSink* aDisplaySink);
        
        /**
         * ConstructL
         */
        void ConstructL();

        /**
         * NewL
         */
        static CDisplaySinkProxy* NewL(MVTVideoSink* aDisplaySink);
        /**
         * Get pointer object of data sink.
         * @return member variant:iDisplaySink
         */
        MVTVideoSink* DisplaySink() { return iDisplaySink; }
    private:
        MVTVideoSink* iDisplaySink;        
    };

NONSHARABLE_CLASS( CAudioSinkProxy ) : public CBase,
                                       public MVTAudioSink
    {
    };
    
#endif
//  End of File 
