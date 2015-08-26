// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define DHTPIN 2 //Pin that the temperature/humidity sensor is connected to.
#define DHTTYPE DHT22		// DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

double f;//variable for the temperature in fahrenheit.
double t;//variable for the temperature in celcius.
double h;//variable for the humidity percentage from 0-100

// -----------------------------------
// Controlling LEDs over the Internet
// -----------------------------------
int led1 = D7;
String cmd[] = {"Hi", "Temperature", "Humidity", "All", "Help"};

void setup()
{
    
   // Start DHT
   dht.begin();

   // Here's the pin configuration, same as last time
   pinMode(led1, OUTPUT);

   // We are also going to declare a Spark.function so that we can turn the LED on and off from the cloud.
   Particle.function("led",ledToggle);
   
   // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.
   Particle.function("sendMessage", sendMessage);
   
   // For good measure, let's also make sure both LEDs are off when we start:
   digitalWrite(led1, LOW);
   
   Serial.begin(9600);
   Serial.println("Starting...");
}

void loop()
{
   // Nothing to do here
}

int sendMessage(String message){
    String response;
    char publishString[128];
    int i;
    for (i = 0; i <= sizeof(cmd)/sizeof(cmd[0]); i++) {
        if (message.equals(cmd[i])) // compare String message and Array Element
            break;
    }
    switch (i) {
        case 0:
          strcpy (publishString, "Hi, it is nice to meet you");
          break;
        case 1:
	      t = dht.getTempCelcius();
	      strcpy (publishString, "It is currently ");
          strcat (publishString,String(t, 2));
          strcat (publishString,"*C");
          break;
        case 2:
          h = dht.getHumidity();
          strcpy (publishString, "The humidity is currently ");
          strcat (publishString,String(h, 2));
          strcat (publishString,"%");
          break;
        case 3:
          h = dht.getHumidity();
          t = dht.getTempCelcius();
          strcpy (publishString, "The humidity is ");
          strcat (publishString,String(h, 2));
          strcat (publishString,"%");
          strcat (publishString, " and the temperature is ");
          strcat (publishString,String(t, 2));
          strcat (publishString,"*C");
          break;
        case 4:
          strcpy (publishString, "I'm still rather limited but any of the following will work: 'Hi', 'Temperature', 'Humidity', 'All'");
          break;
        default: 
          strcpy (publishString, "I don't know what you mean by that");
    }
    Particle.publish("ipmessaging", publishString, 60, PRIVATE);
    
}

int ledToggle(String command) {
    if (command=="on") {
        digitalWrite(led1,HIGH);
        return 1;
    }
    else if (command=="off") {
        digitalWrite(led1,LOW);
        return 0;
    }
    else {
        return -1;
    }
}