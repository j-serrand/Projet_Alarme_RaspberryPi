
/*************************************************************************************************************************

Nom ......... : Projet_Alarme_Emission_433.ino
Role ........ : Transmet en 433MHz grâce à la bibliothèque VirtualWire une chaîne de caractère sur la pin 12 de l'Arduino
                Mega 2560, utilise également la bibliothèque FlexiTimer2 pour faire des pauses entre les émssions des
                messages de repos en "non bloquant"
                * Dans le cas normal envoi :"Repos_capteur_ILS"
                * Dans le cas d'un front montant sur CapteurPin envoi : "Alerte_capteur_ILS"

                Liens vers les bibliothèques :
                https://www.pjrc.com/teensy/td_libs_VirtualWire.html
                http://playground.arduino.cc/Main/FlexiTimer2
                
Auteur ...... : J.Serrand
Mail ........ : joris.serrand@rascol.net
Version ..... : V1.0 du 17/02/16
Licence ..... :  Copyright (C) 2016  Joris SERRAND
                 
                 This program is free software: you can redistribute it and/or modify
                 it under the terms of the GNU General Public License as published by
                 the Free Software Foundation, either version 3 of the License, or
                 (at your option) any later version.
    
                 This program is distributed in the hope that it will be useful,
                 but WITHOUT ANY WARRANTY; without even the implied warranty of
                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                 GNU General Public License for more details.
              
                 You should have received a copy of the GNU General Public License
                 along with this program.  If not, see <http://www.gnu.org/licenses/>

Compilation . : Avec l'IDE Arduino
 
**************************************************************************************************************************/
#include <VirtualWire.h>//Bibliothèque pour utiliser transmission 433Mhz
#include <FlexiTimer2.h>//Bibliothèque pour utiliser timer

const int CapteurPin=2; //Déclaration de la pin d'interruption où est connecté le capteur 
const char* msg_alerte = "Alerte_capteur_ILS";
const char* msg_repos = "Repos_capteur_ILS";

volatile boolean capteur_actif=false; //Déclaration des flags en type volatile boolean pour acces dans les interruptions
volatile boolean fin_timer=false;
/**************************************************************************************************************************/
void setup()
{
    Serial.begin(9600);	  // Pour le debuggage

    // Initialisation de la transmission VirtualWire
    vw_set_ptt_inverted(true); // Inversion de la polarité pour le DR3100
    vw_setup(2000);	 // Vitesse de transmission à l'emetteur
    
    pinMode(CapteurPin,INPUT); //Déclaration de la pin du capteur en entrée

    //Déclaration de l'interrupt sur la pin 2 lors d'un front montant
    attachInterrupt(digitalPinToInterrupt(2), macro_interrupt_capteur, RISING);
    
    FlexiTimer2::set(5000, macro_interrupt_timer); // Déclaration du timer pour envoi mess de repos toute les 30s
    FlexiTimer2::start();//Démarrage du timer de messages de repos
}
/**************************************************************************************************************************/
void macro_interrupt_capteur()//Macro d'interruption quand ILS actif
{
  capteur_actif=true;//mise à 1 du flag
}
/**************************************************************************************************************************/
void macro_interrupt_timer()//Macro d'interruption quand fin timer
{
  fin_timer=true;//mise à 1 du flag
}
/**************************************************************************************************************************/
void loop()
{
   
   if(capteur_actif)
   {
     FlexiTimer2::stop();//Arrêt du timer de messages de repos 
     for(int i=0;i<5;i++)
     {
        digitalWrite(13, true); // Allume la led 13 pour montrer la transmission
        vw_send((uint8_t *)msg_alerte, strlen(msg_alerte));
        vw_wait_tx(); // Attend que le message soit entièrement envoyé
        digitalWrite(13, false); 
        delay(200);
     }    
     capteur_actif=false; //mise à 0 du flag
     FlexiTimer2::start();//Reprise du timer de messages de repos
   }   
   else if(fin_timer)
        {
          for(int i=0;i<5;i++)
           {
            digitalWrite(13, true); // Flash a light to show transmitting
            vw_send((uint8_t *)msg_repos, strlen(msg_repos));
            vw_wait_tx(); // Attend que le message soit entièrement envoyé
            digitalWrite(13, false);
            delay(200);
           }
          fin_timer=false;//mise à 0 du flag
        }
     
}
