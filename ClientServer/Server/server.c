/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    server.c                                                         */
/*                                                                           */
/* PURPOSE: This is a skeleton program to demonstrate how you would write a  */
/*          a TCP Server application. This sample registers a server on a    */
/*          speciable port number and waits for connections.  It will allow  */
/*          a connection from one client, and let the user send data back    */
/*          and forth.  For simplicity, the server refuses more than 1       */
/*          simultaneous client connection.                                  */
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
#include "server.h"
#include <visa.h>
#include <math.h>
#include <ctype.h>


/*---------------------------------------------------------------------------*/
/* Macros						                                             */
/*---------------------------------------------------------------------------*/
#define tcpChk(f) if ((g_TCPError=(f)) < 0) {ReportTCPError(); goto Done;} else
#define PI 3.14159
	
/*---------------------------------------------------------------------------*/
/* Internal function prototypes                                              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error,
							 void *callbackData);
static void ReportTCPError (void);

/*---------------------------------------------------------------------------*/
/* Module-globals                                                            */
/*---------------------------------------------------------------------------*/
static unsigned int g_hconversation;
static int g_hmainPanel = 0;
static int g_TCPError = 0;
static double gain[256];
static char adresseGBF[256], adresseMM[256];
static char bufferWrite[256]; // envoyer l'info au GBF		 
static char bufferRead[256]; // mesurer frequence en cours
static double vE, vS;
static int pas;
static ViSession instrumentHandlerGBF, instrumentHandlerMM;
static ViSession resourceManagerHandlerGBF, resourceManagerHandlerMM;
static int count;

/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
	int      portNum;
	char     tempBuf[256] = {0};
	int		 registered = 0;

	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;
	if ((g_hmainPanel = LoadPanel(0, "server.uir", MAINPNL)) < 0)
		return -1;
	DisableBreakOnLibraryErrors();

	/* Prompt for the port number on which to receive connections */
	PromptPopup ("Port Number?",
				 "Type the port number on which you would like to register "
				 "this server application.\n\n(example: 10000)",
				 tempBuf, 31);
	portNum = atoi (tempBuf);

	/* Attempt to register as a TCP server... */
	SetWaitCursor (1);
	if (RegisterTCPServer (portNum, ServerTCPCB, 0) < 0)
		MessagePopup("TCP Server", "Server registration failed!");
	else
		{
		registered = 1;
		SetWaitCursor (0);
		
		/* We are successfully registered -- gather info */
		SetCtrlVal (g_hmainPanel, MAINPNL_ONLINE, 1);
		if (GetTCPHostAddr (tempBuf, 256) >= 0)
	        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_IP, tempBuf);
		if (GetTCPHostName (tempBuf, 256) >= 0)
	        SetCtrlVal (g_hmainPanel, MAINPNL_SERVER_NAME, tempBuf);
		SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 0); 
		
		/* Display the panel and run the UI */
		DisplayPanel (g_hmainPanel);
		RunUserInterface ();
		}
	
	if (registered)
		UnregisterTCPServer (portNum);
		
	/* Free resources and return */
	DiscardPanel (g_hmainPanel);
	CloseCVIRTE ();
	return 0;
}

/*---------------------------------------------------------------------------*/
/* When the user hits ENTER after typing some text, send it to the client... */
/* Note that the string control will be dimmed unless there is a client      */
/* connected.                                                                */
/*---------------------------------------------------------------------------*/
void TransmitCB (unsigned panelHandle)
{
	// transmission des données
	char transmitBuf[512] = {0};
	for(int j = 0; j<pas; j++) {
		sprintf(transmitBuf, "%f", gain[j]);
		if (ServerTCPWrite (g_hconversation, transmitBuf, 
							strlen (transmitBuf), 1000) < 0)
			SetCtrlVal (panelHandle, MAINPNL_TRANSMIT, "Transmit Error\n");
		else {
			SetCtrlVal (panelHandle, MAINPNL_TRANSMIT, transmitBuf);
			SetCtrlVal (panelHandle, MAINPNL_TRANSMIT, "\n");
		}
		Sleep(500);
	}
}

/*---------------------------------------------------------------------------*/
/* This is the TCP server's TCP callback.  This function will receive event  */
/* notification, similar to a UI callback, whenever a TCP event occurs.      */
/* We'll respond to CONNECT and DISCONNECT messages and indicate to the user */
/* when a client connects to or disconnects from us.  when we have a client  */
/* connected, we'll respond to the DATAREADY event and read in the available */
/* data from the client and display it.                                      */
/*---------------------------------------------------------------------------*/
int CVICALLBACK ServerTCPCB (unsigned handle, int event, int error,
							 void *callbackData)
{
	char receiveBuf[256] = {0};
	ssize_t dataSize        = sizeof (receiveBuf) - 1;
	char addrBuf[31];

	switch (event)
		{
		case TCP_CONNECT:
			if (g_hconversation)
				{
				/* We already have one client, don't accept another... */
				tcpChk (GetTCPPeerAddr (handle, addrBuf, 31));
				sprintf (receiveBuf, "-- Refusing connection request from "
									 "%s --\n", addrBuf); 
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
				tcpChk (DisconnectTCPClient (handle));
				}
			else
				{
				/* Handle this new client connection */
				g_hconversation = handle;
				SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 1);
				tcpChk (GetTCPPeerAddr (g_hconversation, addrBuf, 31));
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_IP, addrBuf);
				tcpChk (GetTCPPeerName (g_hconversation, receiveBuf, 256));
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_NAME, receiveBuf);
				sprintf (receiveBuf, "-- New connection from %s --\n",
						 addrBuf); 
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
				
				/* Set the disconect mode so we do not need to terminate */
				/* connections ourselves. */
				tcpChk (SetTCPDisconnectMode (g_hconversation, 
											  TCP_DISCONNECT_AUTO));
				}
			break;
		case TCP_DATAREADY:
			if ((dataSize = ServerTCPRead (g_hconversation, receiveBuf,
										   dataSize, 1000))
				< 0)
				{
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, "Receive Error\n");
				}
			else
			{
				if (isdigit(receiveBuf[0]))
				{
					// initialisation
					count = 0;
					pas = atoi(receiveBuf);
				}
				else
				{
					if (count == 1) 
					{
						strcpy(adresseGBF, receiveBuf);
						//printf("adresseGBF: %s\n", adresseGBF);
					}
					if (count == 2)
					{
						strcpy(adresseMM, receiveBuf);
						//printf("adresseMM: %s\n", adresseMM);
						// ouvrir la connexion VISA vers GBF et multimetre
						ViSession viSessionGBF = viOpenDefaultRM(&resourceManagerHandlerGBF);
						ViSession viSessionMM = viOpenDefaultRM(&resourceManagerHandlerMM);
						viOpen(resourceManagerHandlerGBF, adresseGBF, VI_NULL, VI_NULL, &instrumentHandlerGBF);
						viOpen(resourceManagerHandlerMM, adresseMM, VI_NULL, VI_NULL, &instrumentHandlerMM);
					}
					if (count > 2 && count <= pas+2)
					{
						strcpy(bufferWrite, receiveBuf);
						int q = 0;
						while (bufferWrite[q] != 'V') {
							q++;
						}
						q+=3;
						
						// vE
						char temp[6], temp2[6];
						int o = 0;
						while (bufferWrite[q-6-o] != ' ') {
							o++;
						}
						for (int l=0; l<o; l++)
							temp[l] = bufferWrite[q-6-o+l];
						temp[l+1] = '\0';
						//printf("temp = %s\n", temp);
						vE = strtod(temp, temp2);
						
						
						char bufferToSend[q];
						for (int i = 0;  i < q; i++)
							bufferToSend[i] = bufferWrite[i];
						
						//printf("Commande envoyee: %s\n", bufferToSend);
						viWrite(instrumentHandlerGBF, bufferToSend, sizeof(bufferToSend), VI_NULL);
						viWrite(instrumentHandlerMM, "MEAS:VOLT:AC?", sizeof("MEAS:VOLT:AC?"), VI_NULL);
						viRead(instrumentHandlerMM, bufferRead, sizeof(bufferRead), VI_NULL);
						//printf("vS recue: %s", bufferRead);
						// on transfer char to double
						
						vS = (double) bufferRead[1] - 48;
						for (int j = 3; j < 11; j+=1) 
						{
							vS *= 10;
							vS += (double)bufferRead[j] - 48;
						}
						
						int exp = 10*((double)bufferRead[13] - 48);
						exp += (double)bufferRead[14] - 48;
						vS /= pow(10, exp);
						for (int k = 0; k < 8; k++)
							vS /= 10;
						//printf("vS = %f\nvE = %f\n\n",vS, vE);
						gain[count-3] = 20*log(vS/vE);
						//printf("Gain: %f\n\n", gain[count-2]);
					}
				}
				//printf("Count: %d\n", count);
				count++;
				receiveBuf[dataSize] = '\0';
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, receiveBuf);
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE, "\n");
				if (count == pas+3) {
					Sleep(3000);
					TransmitCB(g_hmainPanel);
				}
			}
			break;
		case TCP_DISCONNECT:
			if (handle == g_hconversation)
				{
				/* The client we were talking to has disconnected... */
				SetCtrlVal (g_hmainPanel, MAINPNL_CONNECTED, 0);
				g_hconversation = 0;
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_IP, "");
				SetCtrlVal (g_hmainPanel, MAINPNL_CLIENT_NAME, "");
				SetCtrlVal (g_hmainPanel, MAINPNL_RECEIVE,
							"-- Client disconnected --\n");
				SetCtrlAttribute (g_hmainPanel, "TEST3", ATTR_DIMMED,
								  1);
				
				/* Note that we do not need to do any more because we set the*/
				/* disconnect mode to AUTO. */
				}
			break;
	}
	
Done:    
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
static void ReportTCPError (void)
{
	if (g_TCPError < 0)
		{
		char	messageBuffer[1024];
		sprintf(messageBuffer, 
			"TCP library error message: %s\nSystem error message: %s", 
			GetTCPErrorString (g_TCPError), GetTCPSystemErrorString());
		MessagePopup ("Error", messageBuffer);
		}
}
