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
 * Description:  Implementation of applicationmanagement components
 *
*/


#ifndef __AMC_H__
#define __AMC_H__


// Successful - The Request has Succeeded

const TInt KStatusSuccess = 1200;

//User chose not to accept the operation when prompted

const TInt KStatusUserCancelled = 1401;

//The Software Component download failed 

const TInt KStatusDownloadFailed = 1402;

//The download failed due to insufficient memory in the Device 
//to save the Delivery Package.

const TInt KStatusDowloadFailedOOM = 1404;

//Software Component  installation failed in the Device

const TInt KStatusInstallFailed = 1405;

//The install failed because there wasn't sufficient memory to install 
//the Software Component in the Device.

const TInt KStatusInstallFailedOOM = 1406;

//Failure to positively validate digital signature of the Delivery Package

const TInt KStatusPkgValidationFailed = 1407;

//The Software Component Remove operation failed

const TInt KStatusRemoveFailed = 1408;

//The Software Component Activate operation failed

const TInt KStatusActivationFailed = 1409;

//The Software Component Deactivate operation failed

const TInt KStatusDeactivationFailed = 1410;

//The Operation has been rejected because the device does not support the target environment type

const TInt KStatusUnSupportedEnvironment = 1413;

//Alternate Download Server Error Encountered

const TInt KStatusAltDowldError = 1500;

//The Alternate Download Server is unavailable or does not respond

const TInt KStatusAltDowldUnavailable = 1501;

//Authentication was Required but Authentication Failure was encountered when downloading Software Component

const TInt KStatusAlternateDownldAuthFail = 1403;




    

#endif
//  End of File
