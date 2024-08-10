/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    client.c                                                         */
/*                                                                           */
/* PURPOSE: This is a skeleton program to demonstrate how you would write a  */
/*          a TCP Client application. This program connects to an established*/
/*          TCP server and communicates via a user interface panel.  This    */
/*          sample only communicates with one server, but illustrates how to */
/*          implement a callback function to respond to TCP events.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include <windows.h>
#include <cvirte.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcpsupp.h>
#include <string.h>
#include <utility.h>
#include <userint.h>
#include "client.h"
#include <ansi_c.h>

/*---------------------------------------------------------------------------*/
/* Macros						                                             */
/*---------------------------------------------------------------------------*/
#define tcpChk(f) if ((g_TCPError=(f)) < 0) {ReportTCPError(); goto Done;} else
	
/*---------------------------------------------------------------------------*/
/* Internal function prototypes                                              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ClientTCPCB (unsigned handle, int event, int error,
                             void *callbackData);
static void ReportTCPError (void);

/*---------------------------------------------------------------------------*/
/* Module-globals                                                            */
/*---------------------------------------------------------------------------*/
static unsigned int g_hconversation;
static int g_hmainPanel;
static int g_connected = 0;
static int g_TCPError = 0;
static int pas;
static double freq[255], gain[255];
static int step = 0;
/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
    int  portNum;
    char tempBuf[256] = {0};
    char portNumStr[32];
    
    if (InitCVIRTE (0, argv, 0) == 0)
        return -1;
    if ((g_hmainPanel = LoadPanel(0, "client.uir", MAINPNL)) < 0)
        goto Done;
    DisableBreakOnLibraryErrors();

    /* Prompt for the name of the server to connect to */
    PromptPopup ("Server Name?",
                 "Type the name of the machine running the sample server "
                 "application.\n\n(example: abc.xyz.com or xxx.xxx.xxx.xxx)",
                 tempBuf, 255);

    /* Prompt for the port number on the server */
    PromptPopup ("Port Number?",
                 "Type the port number that was set when starting the sample "
                 "server application.\n\n(example: 10000)",
                 portNumStr, 31);
    portNum = atoi (portNumStr);

    /* Attempt to connect to TCP server... */
    SetWaitCursor (1);
    if (ConnectToTCPServer (&g_hconversation, portNum, tempBuf, ClientTCPCB,
                            NULL, 5000) < 0)
        MessagePopup("TCP Client", "Connection to server failed !");
    else
        {
        SetWaitCursor (0);
        g_connected = 1;
        
        /* We are successfully connected -- gather info */
        SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 1);
  		if (GetTCPHostAddr (tempBuf, 256) >= 0)
        	SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_IP, tempBuf);
        if (GetTCPHostName (tempBuf, 256) >= 0)
	        SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_NAME, tempBuf);
        tcpChk (GetTCPPeerAddr (g_hconversation, tempBuf, 256));
        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_IP, tempBuf);
        tcpChk (GetTCPPeerName (g_hconversation, tempBuf, 256));
        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_NAME, tempBuf);
        
        /* display the panel and run the UI */
        DisplayPanel (g_hmainPanel);
        RunUserInterface ();
        }
    
Done:
    /* Disconnect from the TCP server */
    if (g_connected)
    	DisconnectFromTCPServer (g_hconversation);

    /* Free resources and return */
    DiscardPanel (g_hmainPanel);
    CloseCVIRTE ();
    return 0;
}

/*---------------------------------------------------------------------------*/
/* When the user hits ENTER after typing some text, send it to the server... */
/*---------------------------------------------------------------------------*/

int CVICALLBACK OkCallback (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	char bufferWrite[64];// envoyer l'info au GBF		 

	double vE, freq_b, freq_h;
	char adresseGBF[32], adresseMM[32];
	char arr[32], arrA[32];
	
	switch (event)
	{
		case EVENT_COMMIT:
			step = 0;
			GetCtrlVal(g_hmainPanel, MAINPNL_PAS, &pas);
			GetCtrlVal(g_hmainPanel, MAINPNL_AMPLUTIDE, &vE);
			GetCtrlVal(g_hmainPanel, MAINPNL_FREQ_B, &freq_b);
			GetCtrlVal(g_hmainPanel, MAINPNL_FREQ_H, &freq_h);
			GetCtrlVal(g_hmainPanel, MAINPNL_ADRESSE_GBF, adresseGBF);
			GetCtrlVal(g_hmainPanel, MAINPNL_ADRESSE_MM, adresseMM);
			
			strcpy(arrA, ", ");
			sprintf(arr, "%2.2f", vE);
			strcat (arrA, arr);
			strcat (arrA, " VPP");
			
			
			
			sprintf(bufferWrite, "%d", pas);
			if (ClientTCPWrite (g_hconversation, bufferWrite,
                				strlen (bufferWrite), 1000) < 0)
                SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT,
                            "Transmit Error\n");
			else
			{
				strcat(bufferWrite, "\n");
				SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT, bufferWrite);
			}
			
			
			strcpy(bufferWrite, adresseGBF);
			Sleep(1000);
			
			if (ClientTCPWrite (g_hconversation, bufferWrite,
                				strlen (bufferWrite), 1000) < 0)
                SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT,
                            "Transmit Error\n");
			else 
			{
				strcat(bufferWrite, "\n");
				SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT, bufferWrite);
			}
			
			strcpy(bufferWrite, adresseMM);
			Sleep(1000);
			
			if (ClientTCPWrite (g_hconversation, bufferWrite,
                				strlen (bufferWrite), 1000) < 0)
                SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT,
                            "Transmit Error\n");
			else 
			{
				strcat(bufferWrite, "\n");
				SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT, bufferWrite);
			}
			Sleep(1000);
			
			for (int pas_c = 0; pas_c < pas; pas_c++)
			{
				
				freq[pas_c] = freq_b * pow(freq_h/freq_b, (double)pas_c/(double)(pas-1)); //Fi=Fl*((Fh/Fl)**(i/(N-1)));
				
				strcpy(bufferWrite, "APPL:SIN ");
				sprintf(arr, "%2.0f", freq[pas_c]);
				strcat(bufferWrite, arr);
				strcat(bufferWrite, " HZ");
				strcat(bufferWrite, arrA);
				
				// la solution au problème des caractères à la fin de bufferWrite
				int q = 0;
				while (bufferWrite[q] != 'V') {
					q++;
				}
				q+=3;
				
				SetCtrlVal(g_hmainPanel, MAINPNL_FREQ_C, freq[pas_c]); // on renouvelle interface de la fréquence courante
				
            	
				
				
				if (ClientTCPWrite (g_hconversation, bufferWrite,
                                strlen (bufferWrite), 1000) < 0)
                SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT,
                            "Transmit Error\n");
				else 
				{
					strcat(bufferWrite, "\n");
					SetCtrlVal (g_hmainPanel, MAINPNL_TRANSMIT, bufferWrite);
				}
				// Serveur, GBF et multimèter ne sont pas tellement réactives comme les PC
				Sleep(3000);
			}			
            break;
    }
    return 0;
}


int CVICALLBACK QuitCallback (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/
/* This is the TCP client's TCP callback.  This function will receive event  */
/* notification, similar to a UI callback, whenever a TCP event occurs.      */
/* We'll respond to the DATAREADY event and read in the available data from  */
/* the server and display it.  We'll also respond to DISCONNECT events, and  */
/* tell the user when the server disconnects us.                             */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ClientTCPCB (unsigned handle, int event, int error,
                             void *callbackData)
{
    char receiveBuf[256] = {0};
    ssize_t dataSize         = sizeof (receiveBuf) - 1;

    switch (event)
        {
        case TCP_DATAREADY:
            if ((dataSize = ClientTCPRead (g_hconversation, receiveBuf,
                                           dataSize, 1000))
                < 0)
                {
                SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, "Receive Error\n");
                }
            else
            	{	
					
	            	receiveBuf[dataSize] = '\0';
					SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
					SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, "\n");
					char *tempStr;
					if (step < pas) {
						//printf("%f\n", freq[step]);
						gain[step++] = strtod(receiveBuf, &tempStr);
						
					}
	                if (step == pas) 
					{
						int graphCtrl;
					
						graphCtrl = NewCtrl(g_hmainPanel, CTRL_GRAPH_LS, "Réponse en fréquence", 350, 125);
						SetCtrlAttribute (g_hmainPanel, graphCtrl, ATTR_XNAME, "Fréquence");
						SetCtrlAttribute (g_hmainPanel, graphCtrl, ATTR_YNAME, "Gain");

						PlotXY(g_hmainPanel, graphCtrl, freq, gain, pas, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
						step = 0;
	                }
				}
            break;
        case TCP_DISCONNECT:
            MessagePopup ("TCP Client", "Server has closed connection!");
            SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 0);
            g_connected = 0;
            MainPanelCB (0, EVENT_CLOSE, 0, 0, 0);
            break;
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
/* Respond to the UI and clear the receive screen for the user.              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ClearScreenCB (int panel, int control, int event,
                               void *callbackData, int eventData1,
                               int eventData2)
{
    if (event == EVENT_COMMIT)
        ResetTextBox (panel, MAINPNL_RECEIVE, "");
    return 0;
}

/*---------------------------------------------------------------------------*/
/* Respond to the panel closure to quit the UI loop.                         */
/*---------------------------------------------------------------------------*/
int CVICALLBACK MainPanelCB (int panel, int event, void *callbackData,
                             int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

/*---------------------------------------------------------------------------*/
/* Report TCP Errors if any                         						 */
/*---------------------------------------------------------------------------*/
static void ReportTCPError(void)
{
	if (g_TCPError < 0)
		{
		char	messageBuffer[1024];
		sprintf(messageBuffer, 
			"TCP library error message: %s\nSystem error message: %s", 
			GetTCPErrorString (g_TCPError), GetTCPSystemErrorString());
		MessagePopup ("Error", messageBuffer);
		g_TCPError = 0;
		}
}
/*---------------------------------------------------------------------------*/
