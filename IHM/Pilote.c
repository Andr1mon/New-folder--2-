#include <ansi_c.h>
#include <userint.h>
#include "Pilote.h"
#include <visa.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>

#define PI 3.14159

static int adresseGb;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((adresseGb = LoadPanel (0, "Pilote.uir", ADRESSE_GB)) < 0)
		return -1;
	
	//ViFindList viListInstrument;
	//char fisrtInstrument[256];
	//viFindRsrc(resourceManagerHandler, , &viListInstrument, VI_NULL, fisrtInstrument);
	
	DisplayPanel (adresseGb);
	RunUserInterface ();
	DiscardPanel (adresseGb);
	return 0;
}

int CVICALLBACK OkCallback (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			// initialisation des variables
			


			
			char bufferWrite[64]; // envoyer l'info au GBF
			char bufferRead[16]; // mesurer frequence en cours
			char adresseGBF[32], adresseMM[32];
			char arr[32], arrA[32];
			int pas;
			double vE, vS, freq_b, freq_h;
			ViSession instrumentHandlerGBF, instrumentHandlerMM;
			ViSession resourceManagerHandlerGBF, resourceManagerHandlerMM;
			ViSession viSessionGBF = viOpenDefaultRM(&resourceManagerHandlerGBF);
			ViSession viSessionMM = viOpenDefaultRM(&resourceManagerHandlerMM);
			
			// recuperation des donnees saisies par l'utilisateur a partir d'interface
			GetCtrlVal(adresseGb, ADRESSE_GB_PAS, &pas);
			GetCtrlVal(adresseGb, ADRESSE_GB_AMPLUTIDE, &vE);
			GetCtrlVal(adresseGb, ADRESSE_GB_FREQ_B, &freq_b);
			GetCtrlVal(adresseGb, ADRESSE_GB_FREQ_H, &freq_h);
			GetCtrlVal(adresseGb, ADRESSE_GB_ADRESSE_GBF, adresseGBF);
			GetCtrlVal(adresseGb, ADRESSE_GB_ADRESSE_MM, adresseMM);
			
			// ouvrir la connexion VISA vers GBF et multimetre
			viOpen(resourceManagerHandlerGBF, adresseGBF, VI_NULL, VI_NULL, &instrumentHandlerGBF);
			viOpen(resourceManagerHandlerMM, adresseMM, VI_NULL, VI_NULL, &instrumentHandlerMM);
			
			// la fin de APPLy avec virgule et amplutide et toujours le meme
			
			strcpy(arrA, ", ");
			sprintf(arr, "%2.1f", vE);
			strcat (arrA, arr);
			strcat (arrA, " VPP");

			double freq[pas], gain[pas];
			
			
			for (int pas_c = 0; pas_c < pas; pas_c++){
				freq[pas_c] = freq_b * pow(freq_h/freq_b, (double)pas_c/(double)(pas-1)); //Fi=Fl*((Fh/Fl)**(i/(N-1)));
				
				strcpy(bufferWrite, "APPL:SIN ");
				sprintf(arr, "%2.0f", freq[pas_c]);
				strcat(bufferWrite, arr);
				strcat (bufferWrite, " HZ");
				strcat(bufferWrite, arrA);
				
				// la solution au problème des caractères à la fin de bufferWrite
				int q = 0;
				while (bufferWrite[q] != 'V') {
					q++;
				}
				q+=3;
				char bufferToSend[q];
				for (int i = 0;  i < q; i++)
					bufferToSend[i] = bufferWrite[i];
				
				
				viWrite(instrumentHandlerGBF, bufferToSend, sizeof(bufferToSend), VI_NULL);
				
				printf("Commande envoyee: %s\n", bufferToSend);
				viWrite(instrumentHandlerMM, "MEAS:VOLT:AC?", sizeof("MEAS:VOLT:AC?"), VI_NULL);
				viRead(instrumentHandlerMM, bufferRead, sizeof(bufferRead), VI_NULL);
				
				printf("vS recue: %s", bufferRead);
				
				// on transfer char to double
				vS = (double) bufferRead[1] - 48;
				for (int i = 3; i < 11; i+=1) {
					vS *= 10;
					vS += (double)bufferRead[i] - 48;
				}
				int exp = 10*((double)bufferRead[13] - 48);
				exp += (double)bufferRead[14] - 48;
				vS /= pow(10, exp);
				for (int i = 0; i < 8; i++)
					vS /= 10;
				
				printf("vS = %f\nvE = %f\n",vS, vE);
				gain[pas_c] = 20*log(vS/vE);
				printf("Gain: %f\n\n", gain[pas_c]);
				
				SetCtrlVal(adresseGb, ADRESSE_GB_FREQ_C, freq[pas_c]); // on renouvelle interface de la fréquence courante
				
				//PlotStripChartPoint(adresseGb, ADRESSE_GB_REPONSE_FREQ, gain);
				
				
				/*
				const double R = 2200, C = 2.2*pow(10, -6); // R = 2.2 kOhm et C = 2.2 µF
				// x = w/wc où w = 2*Pi*f et wc = 1/RC
				double x = 2*PI*freq_c*R*C;
				// Gdb = -10*log(1+x*x)
				double Gdb = -10*log(1 + pow(x, 2));
				
				printf("Gain en DB = %f\n\n", Gdb);
				*/
			}
			
			int graphCtrl;
			
			graphCtrl = NewCtrl (adresseGb, CTRL_GRAPH_LS, "Réponse en fréquence", 275, 100);
			SetCtrlAttribute (adresseGb, graphCtrl, ATTR_XNAME, "Fréquence");
			SetCtrlAttribute (adresseGb, graphCtrl, ATTR_YNAME, "Gain");

			PlotXY(adresseGb, graphCtrl, freq, gain, pas, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			//SetCtrlAttribute (adresseGb, ADRESSE_GB_REPONSE_FREQ, ATTR_NUM_TRACES, pas);
			//PlotStripChart (adresseGb, ADRESSE_GB_REPONSE_FREQ, points, 3, 0, 10, VAL_DOUBLE);
			
			viClose(viSessionGBF);
			viClose(viSessionMM);
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
