
OBEX MTM Test Harness Setup
---------------------------

[1] Build "msvtestutils.dll" from "\common\messaging\test\testutils\base\group".
    Build "t_obexmtm.exe" and it's support dlls from "\common\messaging\obex\tobexmtm\group".

    Note, on the emulator platforms it is necessary to build the test version of
        "mailinit.dll", from "\common\messaging\framework\server\test\base\group".
        There can be a conflict with the production version of "mailint" and the
        messaging test harnesses.

[2] Copy the following folders to "c:\msgtest\obexsmoke folder"

    \tobexmtm\mediaobjects
    \tobexmtm\scripts

    For the wins + winscw platforms these folders are setup automatically at
    build time.

    The "script.txt" file in the "scripts" folder is used to tell "t_obexmtm.exe" which
    test scripts to actually run. Each line the the "script.txt" refers to a path
    and a filename of the test to carry out where the path is relative to the 
    "c:\msgtest\obexsmoke\scripts" folder.

[3] The commdb will need updating to use the correct Infrared modem. The second entry
    (Commdb_id = 2) in the [ModemBearer] table must be changed to use a Infrared Modem driver.

[4] To execute the harness, run "t_obexmtm.exe" from the cmd line or the development IDE,
    a single test can be executed by using the "-i" parameter.

    To run all tests held in the "script.txt" file:
        type t_obexmtm

    To run a single test script, for example "mtm_registering_script.txt" held in the
    "common" folder:
        type t_obexmtm -i common\mtm_registering_script.txt

    The result log for each test can be found in "c:\msglogs" where each log name is of
    the format "T_OBEXMTM_XXXXXX.PLATFORM.BUILD.LOG", where XXXXXX is the name of test
    script.

[5] Ensure the IR and BT OBEX MTMs are registered before any tests are run. Do this by
    running the "mtm_registering_script.txt" script,
        type t_obexmtm -i common\mtm_registering_script.txt

    Also the MTMs can be un-registered by using the "mtm_unregistering_script.txt" script.
