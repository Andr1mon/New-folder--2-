/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ADRESSE_GB                       1
#define  ADRESSE_GB_AMPLUTIDE             2       /* control type: scale, callback function: (none) */
#define  ADRESSE_GB_FREQ_H                3       /* control type: scale, callback function: (none) */
#define  ADRESSE_GB_FREQ_B                4       /* control type: scale, callback function: (none) */
#define  ADRESSE_GB_PAS                   5       /* control type: scale, callback function: (none) */
#define  ADRESSE_GB_FREQ_C                6       /* control type: scale, callback function: (none) */
#define  ADRESSE_GB_SPLITTER_V1           7       /* control type: splitter, callback function: (none) */
#define  ADRESSE_GB_SPLITTER_V2           8       /* control type: splitter, callback function: (none) */
#define  ADRESSE_GB_SPLITTER_H            9       /* control type: splitter, callback function: (none) */
#define  ADRESSE_GB_ADRESSE_GBF           10      /* control type: ring, callback function: (none) */
#define  ADRESSE_GB_ADRESSE_MM            11      /* control type: ring, callback function: (none) */
#define  ADRESSE_GB_OKBUTTON              12      /* control type: command, callback function: OkCallback */
#define  ADRESSE_GB_QUITBUTTON            13      /* control type: command, callback function: QuitCallback */
#define  ADRESSE_GB_TEXTMSG               14      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OkCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif