/* LYR Signal Processing
 * A Division of LYRtech Inc
 *	
 * Project:  smdetect DLL for signalmaster autodetection under WIN32.
 * File:	 smdetect_exp.h
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
 * Function:	SmDetect
 *
 * Arguments:
 *
 *	(In)		char*			szFilter	:   -> Contains platforms to be detected *
 *	(In/Out)	smTransport*	transport	: 
 *												In:  Contains the Transport Type to be detected
 *												 (ANY_TRANSPORT,ETHERNET_TRANSPORT,
 *												  PCI_TRANSPORT,SERIAL_TRANSPORT)
 *												Out: Transport Structure pointing to selected SM
 *	(Out)		char		szUserName[32]:		Returns the selected SM username 
 *
 ************************************************************************************************/


__declspec(dllimport) int SmDetect(char* szFilter, smTransport* transport,char szUserName[32]);

/**********************************************************************************************
 * Function:	SmDetect2
 *
 * Arguments:
 *
 *	(In)		char*			szFilter	:   -> Contains platforms to be detected *
 *	(In/Out)	smTransport*	transport	: 
 *												In:  Contains the Transport Type to be detected
 *												 (ANY_TRANSPORT,ETHERNET_TRANSPORT,
 *												  PCI_TRANSPORT,SERIAL_TRANSPORT)
 *												Out: Transport Structure pointing to selected SM
 *	(In/Out)	smDetectInfo*	info		:	In: Contains the DspType and FpgaType to be detected
 *												Out: Contains detailed information on the connected device
 *
 ************************************************************************************************/

__declspec(dllexport) int SmDetect2(char* szFilter, smTransport* pTransport, smDetectInfo* pDetectInfo);

#ifdef __cplusplus
}
#endif
