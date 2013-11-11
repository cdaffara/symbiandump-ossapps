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
 * Description: Constants used in native messages
 *
 */

#ifndef NATIVE_MESSAGE_CONSTS_H
#define NATIVE_MESSAGE_CONSTS_H

//  CONSTANTS
#define KSenduiMtmSmtpUidValue              0x10001028
#define KSenduiMtmImap4UidValue             0x1000102A
#define KSenduiMtmPop3UidValue              0x10001029
#define KSenduiMtmSmsUidValue               0x1000102C
#define KSenduiMtmMmsUidValue               0x100058E1
#define KSenduiMtmIrUidValue                0x100053A4
#define KSenduiMtmBtUidValue                0x10009ED5
#define KSenduiMtmPushMtm1UidValue          0x10009158
#define KSenduiMtmPushMtm2UidValue          0x10009158
#define KSenduiMtmPushMtm3UidValue          0x10009158
#define KSenduiMtmPushMtmUidValue           0x10009158
#define KSenduiMtmBioUidValue               0x10001262
#define KSenduiMailEditorUidValue           0x101F4CD6
#define KSenduiMMSEditorUidValue            0x100058DE // Deprecated since 3.2
#define KSenduiMMSNotificationUidValue      0x100059C8
#define KSenduiMtmFaxUidValue               0x1000102B
#define KSenduiMtmSyncMLEmailUidValue       0x101f7c5c
#define KMmsUploadService1                  0x101F8835
#define KMmsUploadService2                  0x101F8836
#define KMmsUploadService3                  0x101F8837
#define KMmsUploadService4                  0x101F8838
#define KSenduiMtmRoUidValue                0x101F6DC2
#define KSenduiMtmPostcardUidValue          0x10207245
#define KDrmLinkSenderValue                 0x10207388
#define KSenduiMtmAudioMessageUidValue      0x1020745B
#define KSenduiMtmUniMessageUidValue        0x102072D6
#define KSenduiUniEditorUidValue            0x102072D8
#define KSendUiGenericMtmPluginUidValue     0x102823B7

// Technology types
#define KSenduiTechnologySmsUidValue        0x10008A30
#define KSenduiTechnologyMmsUidValue        0x100058E2
#define KSenduiTechnologyMailUidValue       0x10001671
#define KSenduiTechnologyUniEditorUidValue  0x102072D5
#define KSenduiTechnologyIrUidValue         0x1000539F
#define KSenduiTechnologyBtUidValue         0x10009ED6
#define KSenduiTechnologyIMUidValue         0x2000B4E8

// MTMs
const TUid KSenduiMtmSmtpUid            = { KSenduiMtmSmtpUidValue         };
const TUid KSenduiMtmImap4Uid           = { KSenduiMtmImap4UidValue        };
const TUid KSenduiMtmPop3Uid            = { KSenduiMtmPop3UidValue         };
const TUid KSenduiMtmSmsUid             = { KSenduiMtmSmsUidValue          };
const TUid KSenduiMtmMmsUid             = { KSenduiMtmMmsUidValue          };
const TUid KSenduiMtmIrUid              = { KSenduiMtmIrUidValue           };
const TUid KSenduiMtmBtUid              = { KSenduiMtmBtUidValue           };
const TUid KSenduiMtmPushMtm1Uid        = { KSenduiMtmPushMtm1UidValue     };
const TUid KSenduiMtmPushMtm2Uid        = { KSenduiMtmPushMtm2UidValue     };
const TUid KSenduiMtmPushMtm3Uid        = { KSenduiMtmPushMtm3UidValue     };
const TUid KSenduiMtmPushMtmUid         = { KSenduiMtmPushMtmUidValue      };
const TUid KSenduiMtmFaxUid             = { KSenduiMtmFaxUidValue          };
const TUid KSenduiMtmSyncMLEmailUid     = { KSenduiMtmSyncMLEmailUidValue  };
const TUid KSenduiMtmBioUid             = { KSenduiMtmBioUidValue          };
const TUid KSenduiMtmRoUid              = { KSenduiMtmRoUidValue           };
const TUid KSenduiMMSNotificationUid    = { KSenduiMMSNotificationUidValue };
const TUid KSenduiMtmPostcardUid        = { KSenduiMtmPostcardUidValue     };
const TUid KSenduiMtmAudioMessageUid    = { KSenduiMtmAudioMessageUidValue };
const TUid KSenduiMtmUniMessageUid      = { KSenduiMtmUniMessageUidValue   };


// ECom services
const TUid KMmsDirectUpload             = { 0x101F87F5                     };
const TUid KMmsIndirectUpload           = { 0x101F87F7                     };
const TUid KMmsUploadService1Id         = { KMmsUploadService1             };
const TUid KMmsUploadService2Id         = { KMmsUploadService2             };
const TUid KMmsUploadService3Id         = { KMmsUploadService3             };
const TUid KMmsUploadService4Id         = { KMmsUploadService4             };
const TUid KMeetingRequestSender        = { 0x101FD69A                     };
const TUid KDrmLinkSender               = { KDrmLinkSenderValue            };
const TUid KGenericMtmPlugin            = { KSendUiGenericMtmPluginUidValue};

// Technology types
const TUid KSenduiTechnologySmsUid          = { KSenduiTechnologySmsUidValue        };
const TUid KSenduiTechnologyMmsUid          = { KSenduiTechnologyMmsUidValue        };
const TUid KSenduiTechnologyMailUid         = { KSenduiTechnologyMailUidValue       };
const TUid KSenduiTechnologyUniEditorUid    = { KSenduiTechnologyUniEditorUidValue  };
const TUid KSenduiTechnologyIrUid           = { KSenduiTechnologyIrUidValue         };
const TUid KSenduiTechnologyBtUid           = { KSenduiTechnologyBtUidValue         };
const TUid KSenduiTechnologyIMUid           = { KSenduiTechnologyIMUidValue         };

// Message data types
const TUid KSenduiMsgDataBase           = { 0x101FD695                     };
const TUid KSenduiMsgDataMeetingRequest = { 0x101FD696                     };

// Editors
const TUid KSenduiMailEditorUid         = { KSenduiMailEditorUidValue      };
const TUid KSenduiMMSEditorUid          = { KSenduiMMSEditorUidValue       }; // Deprecated since 3.2
const TUid KSenduiUniEditorUid          = { KSenduiUniEditorUidValue       };

#endif      // NATIVE_MESSAGE_CONSTS_H

// End of File
