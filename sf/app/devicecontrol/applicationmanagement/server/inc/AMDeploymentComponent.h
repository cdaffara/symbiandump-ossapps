/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */


#ifndef __CAMDEPLOYMENTCOMPONENT__
#define __CAMDEPLOYMENTCOMPONENT__

#include <e32base.h>
#include <centralrepository.h>
#include <f32file.h> 
#include <SyncMLDef.h>
#include "ApplicationManagementCommon.h"
#include "aminstalloptions.h"
#include "amdeploymentcomponentdata.h"


namespace NApplicationManagement
    {
    _LIT8( KAMDCDelimiter, "|" );

    // FORWARD DECLARATIONS
    class CDeploymentComponent;
    class CAMDownload;

      class MAMDownloadStateObserver
        {
public:
        /**
         * StatusUpdateL Sets the current download status. This is 
         *	currently HTTP status code 
         *	(200 is success, 400+ failure, 100-199 in progress)
         * @param	aNewStatus	The new download status 
         */
        virtual void StatusUpdateL(TInt aNewStatus) = 0;
        virtual void SuccessStatusUpdateL(const TDesC &aDlFileName,
                const TDesC8& aDlMimeType) = 0;
        };


    class MDownloadCallback
        {
    public:
        virtual void DownloadComplete( class CDeploymentComponent *aComponent, TInt aStatus ) = 0;
        virtual TBool UidExistsL( const TUid &aUid, CDeploymentComponent *& aCompo, CDeploymentComponent *aIgnored ) = 0;
        };

    class CDeploymentComponent : public CBase, public MAMDownloadStateObserver
        {

    public: // From MAMDownloadStateObserver	
        void StatusUpdateL( TInt aNewStatus );
        void SuccessStatusUpdateL( const TDesC &aDlFileName, const TDesC8& aDlMimeType );

        void SetStatusNode(const TSCOMOStatusNodeStates &aStatus) const;
    private:
        // Data
        TDeploymentComponentId iId;
        TDeploymentComponentName iName;
        TDeploymentComponentVersion iVersion;
        TDeploymentComponentState iState;
        TDeploymentComponentDescription iDescription;
        TDeploymentComponentPkgId iPkgID;
        TDeploymentComponentPkgIdRef iPkgIDRef;
        TDeploymentComponentPkgType iPkgType;
        TDeploymentComponentName iMidletName;
        TDeploymentComponentName iMideltVendorName;
        TDeploymentComponentVersion iMidletVersion;
        TUint32 iInternalId;
        TUid iUid;
        TDCUserId iUserId;
        
        CDeploymentComponentData *iMetaData;
        CDeploymentComponentData *iData;
        TAMInstallOptions iInstallOpts;
        TBool iInstallOptsSet;
        TUint32 iPkgVersion;
        TBool iRemovableApp;
        TBool iDriveSelection;
        TInt iComponentId;
        TInt iDownloadStatus;
        mutable TInt iStatus;
        TBuf8<KMaxUrlLength> iDownloadURI;
        TDownloadTarget iDownloadTarget;
        SwiUI::TUninstallOptions iUninstallOpts;

        TInt iOwner; // TCertInfo index in CAMStorage::iCertificates
        TDeploymentComponentState iOldState;
        MDownloadCallback *iCallback; // not owned

        TSmlProfileId iOwnerProfile; // profile stored only runtime until download has completed
        CRepository *iRepository;

        // Constructors	
        CDeploymentComponent( TInt iInternalId ); 
        static CDeploymentComponent* NewL( TInt iInternalId, const TDCUserId &aUserId);
        static CDeploymentComponent* NewLC( TInt iInternalId, const TDCUserId &aUserId );
        static CDeploymentComponent* LoadL( TInt iInternalId, const TDesC8 &aBuffer );
        static CDeploymentComponent* LoadLC( TInt iInternalId, const TDesC8 &aBuffer );

        void ConstructL( const TDCUserId &aUserId );
        void ConstructLoadL( const TDesC8 &aBuffer );

        void SetAttr( TDes8 &aAttr, const TDesC8 &aNewVal ) const;

        /**
         * @param aState The new state
         * @return TInt KErrNone if all ok, system error code if visiblity change failed
         */

        TInt SetState( const TDeploymentComponentState &aState ) ;
        /**
	Returns number of bytes written to aBuffer
         */
        virtual TInt SerializedFormL( RWriteStream &aBuffer ) const;
        /**
	Returns length of serizlied form
         */
        virtual TInt SerializedFormLength() const;
        HBufC8* IntToDes8LC( const TInt aLuid );
    public:
        virtual ~CDeploymentComponent();
        TInt DownloadStatus() const;
        inline const TDeploymentComponentId &ComponentId() const;

        inline const TDeploymentComponentName &ComponentName() const;

        inline const TDeploymentComponentVersion &ComponentVersion() const;

        inline TDeploymentComponentState State() const;

        inline TUint32 InternalId() const;

        inline const TUid & Uid() const;

        //inline const TInt DownloadStatus() const;

        inline const TDesC8 &DownloadURI() const ;

        inline const TDeploymentComponentPkgId& PkgId() const ;

        inline const TDeploymentComponentDescription& Description() const;

        inline const TDeploymentComponentPkgIdRef& PkgIDRef() const;

        inline const TDeploymentComponentPkgType& PkgType() const;

        inline const TMidletnameName& MidletName() const;
        inline const TMidletVendorName& MidletVendor() const;
        inline const TMidletVersion& MidletVersion() const;



        virtual void PersistL( CRepository &aRepository ) const;

        virtual void DestroyL( CRepository &aRepository ) ;
        /**
	Copies current state to argument
         */
        void CopyStateTo( TDeploymentComponent &aInfo ) const;

        void SetUid( const TUid aUid );

        void Set( const TDeploymentComponent &aInfo );

        void SetDownloadURIL( const TDesC8 &aURI );

        void SetDownloadTarget( const TDownloadTarget& aDownloadTarget );

        void SetDescriptionL( const TDesC8 &aVersion );

        void SetPkgIDL( const TDesC8 &aPkgID );

        void SetPkgTypeL( const TDesC8 &aPkgType );

        void SetMidletNameL( const TDesC8 &aMidletName );

        void SetMidletVendorL( const TDesC8 &aMidletVendor );

        void SetMidletVersionL( const TDesC8 &aMidletVersion );

        void SetAppRemovableStatus(const TBool &aRemovable);
        
        void SetDriveSelectionStatus(const TBool &aDriveSelection);
        
        void SetComponentId(const TInt &aComponentId);

        void SetCallback( MDownloadCallback *aListener )
            {
            iCallback = aListener;
            }


        /**
	UserId returns the id specified by user
         */
        inline const TDesC8& UserId() const;
        /*
         * Returns the status Node value of the Component ID.
         * */
        inline TInt Status() const;

        CDeploymentComponent &operator=( const CDeploymentComponent &aData );



        /**
         * SetIAPL Sets the iap
         * @param The new iap id
         */
        void SetIAPL( const TInt aIap );


        /**
         * @return The iap id
         */
        TInt GetIAP() const;

        /**
         * IsJavaMimeL
         * @return ETrue if mime is recognised Java mime
         */
        static TBool IsJavaMimeL( const TDesC8& aMime );
        /**
                 * IsSisMimeL
                 * @return ETrue if mime is recognised Sis mime
                 */
                static TBool IsSisMimeL( const TDesC8& aMime );

        /**
         * DownloadFailed
         * @return ETrue if download status is such that it indicates failure, EFalse otherwise
         */
        TBool DownloadFailed() const;

        /**
         * OwnerProfile returns the owner profile - -1 if there's no such
         */
        const TSmlProfileId& OwnerProfile() const;

        /**
         * OwnerProfile 
         */
        void SetOwnerProfile( TSmlProfileId aOwnerProfile);


        /**
         * MimeType
         * @return Component mimetype. This can be mimetype from data or metadata. Usually metadata mimetype is returned in Java case.
         * In case of Java if both Jad and Jar exist, then it is Jad mimetype.
         * In case of Java if only Jar exist, then it is Jar mimetype.
         * In case of Java if only Jad exist, the it is Jad mimetype.
         */
        const TDesC8& MimeType() const;


    protected:

        static CDeploymentComponent *LoadComponentL (  TInt aInternalId, const TDesC8 &aBuffer ) ;
    public:
        CDeploymentComponentData &Data();
        const CDeploymentComponentData &Data() const;
        CDeploymentComponentData &MetaData();
        const CDeploymentComponentData &MetaData() const;
        const TAMInstallOptions &InstallOpts() const;
        const SwiUI::TUninstallOptions &UninstallOpts() const;
        TBool InstallOptsSet() const;
        TBool AppRemovable() const;
        TBool DriveSelected() const;
        TInt GetComponentId() const;
        void SetDataL( const TDesC8 &aMimeType );
        void SetMetaDataL( const TDesC8 &aMimeType );
        void SetDataL( const TDesC8 &aData, const TDesC8 &aMimeType );
        void SetDataL( const TDesC &aFileName, const TDesC8 &aMimeType );
        void SetMetaDataL( const TDesC8 &aMetaData, const TDesC8 &aMimeType );
        void SetInstallOptsL( const TDesC8 &aInstallOptions );
        void ResetDataL( RFs &aFs );

        void SetIdL( const TDesC8 &aId );
        void SetNameL( const TDesC8 &aName );
        void SetVersionL( const TDesC8 &aVersion );
        void SetMimeTypeL( const TDesC8 &aMimeType );
        void SetOwner( const TInt &aCertInfoId );
        TInt Owner( ) const
        {
        return iOwner;
        }
        TDownloadTarget DownloadTarget() const
        {
        return iDownloadTarget;
        }

        inline const TDeploymentComponentState& OldState() const;

        inline void StateChangeComplete();


        friend class CDeliveryComponentStorage;
        };

#include "amdeploymentcomponent.inl"

    }


#endif	

//  End of File
