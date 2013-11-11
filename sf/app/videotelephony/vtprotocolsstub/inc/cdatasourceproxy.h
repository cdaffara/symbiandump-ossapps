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
* Description:  Data Source proxy interface
*
*/

#ifndef CDATASOURCEPROXY_H
#define CDATASOURCEPROXY_H

NONSHARABLE_CLASS( CVideoSourceProxy ) : public CBase
    {
    public: //new
        /**
         * Constructor.
         */
        CVideoSourceProxy(MVTVideoSource* aVideoSource);
        /**
         * ConstructL.
         */
        void ConstructL( );
        
        /**
         * NewL.
         */
        static CVideoSourceProxy* NewL(MVTVideoSource* aVideoSource);
        
        /**
         * Get member variant.
         * @return memeber variant:iVideoSource
         */
        MVTVideoSource* VideoSource() { return iVideoSource; }
    private:
        MVTVideoSource* iVideoSource;
    };

NONSHARABLE_CLASS( CAudioSourceProxy ) : public CBase, 
                                         public MVTAudioSource
    {
    };
#endif
//  End of File 
