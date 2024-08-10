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

#define  MAINPNL                          1       /* callback function: MainPanelCB */
#define  MAINPNL_AMPLUTIDE                2       /* control type: scale, callback function: (none) */
#define  MAINPNL_FREQ_H                   3       /* control type: scale, callback function: (none) */
#define  MAINPNL_FREQ_B                   4       /* control type: scale, callback function: (none) */
#define  MAINPNL_PAS                      5       /* control type: scale, callback function: (none) */
#define  MAINPNL_FREQ_C                   6       /* control type: scale, callback function: (none) */
#define  MAINPNL_SPLITTER_V1              7       /* control type: splitter, callback function: (none) */
#define  MAINPNL_SPLITTER_V2              8       /* control type: splitter, callback function: (none) */
#define  MAINPNL_SPLITTER_H_2             9       /* control type: splitter, callback function: (none) */
#define  MAINPNL_SPLITTER_H               10      /* control type: splitter, callback function: (none) */
#define  MAINPNL_ADRESSE_GBF              11      /* control type: ring, callback function: (none) */
#define  MAINPNL_ADRESSE_MM               12      /* control type: ring, callback function: (none) */
#define  MAINPNL_OKBUTTON                 13      /* control type: command, callback function: OkCallback */
#define  MAINPNL_QUITBUTTON               14      /* control type: command, callback function: QuitCallback */
#define  MAINPNL_TRANSMIT                 15      /* control type: textBox, callback function: (none) */
#define  MAINPNL_RECEIVE                  16      /* control type: textBox, callback function: (none) */
#define  MAINPNL_CONNECTED                17      /* control type: LED, callback function: (none) */
#define  MAINPNL_CLEAR                    18      /* control type: command, callback function: ClearScreenCB */
#define  MAINPNL_CLIENT_NAME              19      /* control type: string, callback function: (none) */
#define  MAINPNL_CLIENT_IP                20      /* control type: string, callback function: (none) */
#define  MAINPNL_SERVER_NAME              21      /* control type: string, callback function: (none) */
#define  MAINPNL_SERVER_IP                22      /* control type: string, callback function: (none) */
#define  MAINPNL_DECORATION               23      /* control type: deco, callback function: (none) */
#define  MAINPNL_TEXTMSG                  24      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ClearScreenCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OkCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif