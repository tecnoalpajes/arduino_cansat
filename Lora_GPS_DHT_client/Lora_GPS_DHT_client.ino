/*
In this project,we'll show how to get GPS data from a remote Arduino via Wireless LoRa Protocol 
Hardware Set up
by Dragino Tech<support@dragino.com>
Dragino Technology Co., Limited
*/
#include <DHT.h>
#define DHTTYPE DHT22
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <String.h>

TinyGPS gps;
RH_RF95 rf95;
SoftwareSerial ss(4, 3); // Arduino RX, TX to conenct to GPS module.
static void smartdelay(unsigned long ms);

String datastring1="";
String datastring2="";
String datastring3="";
uint8_t datasend[50];    //Storage  longtitude,latitude and altitude

char gps_lon[20]="\0";  //Storage GPS info
char gps_lat[20]="\0"; //Storage latitude
char gps_alt[20]="\0"; //Storage altitude

#define DHTPIN 5 // Use A0 pin to connect the data line of DHT11
DHT dht(DHTPIN, DHTTYPE);

float hum;    // Stores humidity value in percent
float temp;   // Stores temperature value in Celcius
String str_humid;
String str_temp;
String str_out; // Define output strings 


void setup(){
  dht.begin();
  // initialize both serial ports:
  Serial.begin(9600);  // Serial to print out GPS info in Arduino IDE
  ss.begin(9600);       // SoftSerial port to get GPS data. 
  while (!Serial);
   if (!rf95.init()) {  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Serial.println("Starting LoRa failed!");
    while (1);
  } 
  /* Set frequency is 868MHz,SF is 7,SB is 125KHz,CR is 4/5 and Tx power is 20dBm */
  rf95.setFrequency(868);
  rf95.setTxPower(13);
  Serial.println("Ready to send!");
}

void loop(){
  delay(2000);
  hum = dht.readHumidity();
  temp = dht.readTemperature(); 
  
  str_humid = String(hum);  // Convert Humidity to string  
  str_temp = String(temp);  // Convert Temperature to string 
  //dtostrf(hum, 0, 6, str_humid)
  //dtostrf(temp,0, 6, str_temp)
  str_out = "$" + str_humid + "," + str_temp + "\n";  // Combine Humidity and Temperature
          
  static char *msg = str_out.c_str(); // Compose output character
  rf95.send((uint8_t *)msg, strlen(msg));
  rf95.waitPacketSent();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  uint8_t buf[50];
  Serial.print("S.1:");
  Serial.print(hum);
  Serial.print(",");
  Serial.print(temp);
  Serial.println("");
 
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  
  float flat, flon,falt;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  falt=gps.f_altitude();  //get altitude       
  flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;//save six decimal places 
  flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
  falt == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : falt, 2;//save two decimal places
  datastring1 +=dtostrf(flat, 0, 6, gps_lat); 
  datastring2 +=dtostrf(flon, 0, 6, gps_lon);
  datastring3 +=dtostrf(falt, 0, 2, gps_alt);
  //if(flon!=1000.000000){
  strcat(datasend,gps_lon);
  strcat(gps_lon,",");
  strcat(gps_lon,gps_lat); 
  strcat(gps_lon,","); 
  strcat(gps_lon,gps_alt);
  strcpy((char *)datasend,gps_lon);//the format of datasend is longtitude,latitude,altitude,DeviceID,
  Serial.println((char *)datasend);
  
  // send data
  rf95.send(datasend, sizeof(datasend));  
  rf95.waitPacketSent();
  
  // Now wait for a reply
  receivepacket();
  //}
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;){
    while (ss.available()){
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  //smartdelay(1000);
}

//If the packet arrive LG01, LG01 will send a ACK and here will receive it and turn on the led.  
void receivepacket(){
    uint8_t indatabuf[50];
    uint8_t len = sizeof(indatabuf);
    
    if (rf95.waitAvailableTimeout(3000)){ 
       // Should be a reply message for us now   
       if (rf95.recv(indatabuf, &len)){
         Serial.println((char*)indatabuf);        
         }
         else {
          Serial.println("receive failed!");
         }
    }
    else{
      Serial.println("No reply, is track_GPS_Server running or overranging the receive distance?");
    }
}
static void smartdelay(unsigned long ms){
  unsigned long start = millis();
  do {
    while (ss.available()){
      ss.print(Serial.read());
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}
