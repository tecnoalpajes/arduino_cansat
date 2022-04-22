/*
In this project,we'll show how to get GPS data from a remote Arduino via Wireless LoRa Protocol
Hardware Set up
1 Client Side: Arduino + LoRa/GPS Shield (868Mhz).
    In the client side, we will use the hardware serial to print out debug data in PC and use software
    serial to get GPS data. 
    1/ Arduino UNO + LoRa GPS Shield . if use other Arduino board, please check the softserial connection and modify the code
    2/ Wire GPS_RXD to Arduino D3
    3/ Wire GPS_TXD to Arduino D4
2 Gateway Side: LG01(868Mhz)
External Library Require:
  TinyGPS:  http://arduiniana.org/libraries/tinygps/
  RadioHead Library: https://github.com/dragino/RadioHead 
Client side will get GPS data and keep sending out to the gateway via LoRa wireless. Gateway side 
will listen on the LoRa wireless frequency, 
by Dragino Tech<support@dragino.com>
Dragino Technology Co., Limited
*/
#include <String.h>
#include <SPI.h>
#include <RH_RF95.h>
#ifdef LG01_GATEWAY
#include <Console.h>
#include <Process.h>
#define SerialPrint Console
#else
#define SerialPrint Serial
#endif

RH_RF95 rf95;

String str_humid;
String str_temp;
String str_out; // Define output strings

char gps_lon[20]="\0";           //Storage longtitude
char gps_lat[20]="\0";           //Storage latitude
char gps_alt[20]="\0";           //Storage altitude

//void receivepacket();    //Processing receive message and store it in the appropriate array

void setup() { 
  #ifdef LG01_GATEWAY
    Bridge.begin(9600);
    SerialPrint.begin();
  #else
    SerialPrint.begin(9600);
  #endif 
  
   if (!rf95.init()) {// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  /* Set frequency is 868MHz,SF is 7,SB is 125KHz,CR is 4/5 and Tx power is 20dBm */
  rf95.setFrequency(868);
  rf95.setTxPower(13);
  
  Serial.println("Ready to receive!");
}
void loop(){
 receivepacket();
}

//Receiver LoRa packets and forward it 
void receivepacket() {
   if (rf95.available()){
    //Serial.println();
    // received a packet
    int i =0,j=0,code[4];
    int m1=0,m2=0,m3=0;   
    uint8_t buf[52];
    char message[52]="\0";
    uint8_t len = sizeof(buf);
    // Check if received packet is correct size
    if (rf95.recv(buf, &len)){
      
      Serial.println((char*)buf);
      Serial.println();
      /*
      // Convert received data into string
      str_out = String((char*)buf);
      // Split string into two values
      int comma_at;
      int newline_at;
      for (int h = 0; h < str_out.length(); h++) {
        if (str_out.substring(h, h+1) == "\n") {
          newline_at = h;
          break;
        }
        if (str_out.substring(h, h+1) == ",") {
          comma_at = h;
        }
      }
      
      str_humid = str_out.substring(1, 10);
      str_temp = str_out.substring(11, 20);
      
      // Print values to Serial Monitor
       */
     delay(500);
     /*
      strcpy(message,(char*)buf);
      while(i<52){
        if(message[i]==','){
          code[j]=i;
          j++;
        }
        i++;
      }
      for(int k=0;k<code[0];k++){
        gps_lon[m1]=message[k];//get longtitude
        m1++;
      }
      for(int k=code[0]+1;k<code[1];k++){
        gps_lat[m2]=message[k];//get latitude
        m2++;
      }
      for(int k=code[1]+1;k<code[2];k++){
        gps_alt[m3]=message[k];//get altitude
        m3++;
      }*/
      
      uint8_t data[] = "Gateway receive GPS data";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
    }
  }
}
