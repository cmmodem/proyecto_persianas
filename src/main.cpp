/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

#include <Arduino.h>
#include <EEPROM.h>

#define BLYNK_TEMPLATE_ID "TMPLPLcC4NbK"
#define BLYNK_DEVICE_NAME "Pruebas"
#define BLYNK_AUTH_TOKEN "Y1zkZtRYREIJXHlLpTrE2K0w2qhCBkRn"

void cuentapasos();



int pin_subir = 26;
int pin_bajar = 25;
int pin_sensor_hall = 27;

static float luminosidad;
static float temperatura;
static float humedad; 
boolean calibracion= HIGH;
boolean estado_pulsador_subir=HIGH;
boolean estado_pulsador_bajar=HIGH;
boolean subiendo= LOW;
boolean bajando = LOW;
int pasos_totales=10;
int pasos =0;
float valor_paso=0;
int posicion_actual = 0;
int posicion_destino = 0;
boolean sinprog=HIGH;

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <BH1750.h>
#include "SparkFunBME280.h"

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "proyecto_persiana";
char pass[] = "abc1234.";

BlynkTimer timer;
BH1750 lightMeter;
BME280 bme280Sensor;

// This function is called every time the Virtual Pin 0 state changes
// BLYNK_WRITE(V0)
// {
//   // Set incoming value from pin V0 to a variable
//   int value = param.asInt();

//   // Update state
//   Blynk.virtualWrite(V1, value);
// }

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  luminosidad = lightMeter.readLightLevel();
  temperatura = bme280Sensor.readTempC();
  humedad = bme280Sensor.readFloatHumidity();
  Blynk.virtualWrite(V0, luminosidad);
  Blynk.virtualWrite(V1, temperatura);
  Blynk.virtualWrite(V2, humedad);
}

void setup()
{
  // Sensor HALL
  pinMode(pin_sensor_hall,INPUT);
  attachInterrupt(digitalPinToInterrupt(pin_sensor_hall), cuentapasos, CHANGE);
  pinMode(pin_subir, OUTPUT);
  pinMode(pin_bajar, OUTPUT);
  digitalWrite(pin_subir,HIGH);
  digitalWrite(pin_bajar,HIGH);
  EEPROM.begin(512);
  // Debug console
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
  bme280Sensor.setI2CAddress(0x76);
  if (bme280Sensor.beginI2C()==false)
  {
    Serial.println("The sensor did not respond. Please check wiring.");

  }
  bme280Sensor.setReferencePressure(101200); //Adjust the sea level pressure used for altitude calculations
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(5000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

// Nos da el numero de pulsos del sensor hall
void cuentapasos()
{
  
  if (calibracion==HIGH)
  {
    pasos++;
  }

  if (subiendo==HIGH)
  {
   if (posicion_actual<pasos_totales)
   {
   posicion_actual++;
   }
   int pos =map(posicion_actual,0,pasos_totales,0,100);
  }
  
  if (bajando==HIGH)
  {
  if (posicion_actual>0)
  {
   posicion_actual--;
  }
   int pos =map(posicion_actual,0,pasos_totales,0,100);
  }   
}

// Las funciones que reciben el valor enviado desde la app
BLYNK_WRITE(V4)
{
  // any code you place here will execute when the virtual pin value changes
  Serial.print("Se ha pulsado virtualmente el boton subir\n");
  Serial.printf("El valor: %d \n",param.asInt());
  estado_pulsador_subir=param.asInt();
  digitalWrite(pin_subir,!estado_pulsador_subir);
}
BLYNK_WRITE(V5)
{
  // any code you place here will execute when the virtual pin value changes
  Serial.print("Se ha pulsado virtualmente el boton bajar\n");
  Serial.printf("El valor: %d\n",param.asInt());
  estado_pulsador_bajar=param.asInt();
  digitalWrite(pin_bajar,!estado_pulsador_bajar);
    
}

