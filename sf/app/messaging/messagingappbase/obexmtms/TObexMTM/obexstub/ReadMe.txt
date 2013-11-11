
OBEX MTM Test Stub Setup
------------------------

[1] Bluetooth stub:

     (a) Build "sdpagent.dll" from the folder
         "\common\messaging\obex\tobexmtm\obexstub\bluetoothsdpstub\sdp".
         Note: "abld build" MUST be used NOT "abld test build".


[2] Infra red stub:

     (a) The "irda" code MUST be downloaded from p4, found in folder
         "\EPOC\master\common\generic\infra-red\irda\".

     (b) The stub files "bld.inf" and "irobexstub.mmp" in folder
         "tobexmtm\obexstub\irobexstub\group", will need to be updated to ensure
         the correct files from the code in "\EPOC\master\common\generic\infra-red\irda\"
         are built.
         The required updates are detailed in the "bld.inf" and "irobexstub.mmp" files.

     (c) Build "irobex.dll" from the folder
         "\common\messaging\obex\tobexmtm\obexstub\irobexstub\group".
         Note: "abld build" MUST be used NOT "abld test build".


[3] Create a "stub" folder in "c:\msgtest\obexsmoke\".


[4] Refer to the readme.txt for information on how to build obex test harness.



The following comments have been made by Oscar Gonzalez, when he wrote the initial stub.

This is a list of contents of this directory. Please note that for some (most) 
files I took the original source code as starting point. I specify here the files
I actually modified.

-----------------------------------------------------------------------
\bluetoothsdpstub ----> Bluetooth SDP agent stub
      Modified following files:
         \sdp\agent\engine.cpp
         \sdp\agent\protocolwrapper.cpp
         \sdp\agent\requester.cpp
         \sdp\agent\sdpagentstub.mmp
         \sdp\bld.inf
-----------------------------------------------------------------------
\IrobexStub ---> Bluetooth/Infrared OBEX stub
      Modified following files:
         \group\bld.inf
         \group\irobexstub.mmp
         \irobex\obex.cpp
         \irobex\obex_client.cpp
         \irobex\obex_server.cpp
-----------------------------------------------------------------------
