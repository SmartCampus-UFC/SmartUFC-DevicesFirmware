#include <SmartUFCManager.h>
#include <DHT.h>

// ############### CONSTANTS ################# //

//General
const int PERIODICITY=5000; // Defined milliseconds

//Device-specific
#define DHTTYPE DHT11 // DHT 11
#define DHTPIN 3 //PINO DIGITAL UTILIZADO PELO SENSOR
const String DEVICE_TYPE = "Temp";

// ############### DEFAULT VARIABLES ################# //

//Device-specific
float temperature = 0; //VARIÁVEL DO TIPO FLOAT
float temperatureAnt = 0;
float humidity = 0; //VARIÁVEL DO TIPO FLOAT
float humidityAnt = 0;
char data[10];
char valueRequest[10];

// ############### OBJECTS ################# //

//Manager
SmartUFCManager manager(DEVICE_TYPE);

//Device-specific
DHT dht(DHTPIN, DHTTYPE);

// ############# PROTOTYPES ############### //

//Device-specific
void initDHT();
void deviceControl();//recebe e envia informações do dispositivo

// ############## SETUP ################# //

void setup() {    
  
  manager.setup();

  // ## JUST CHANGE IN THIS PART ## //
  
  initDHT(); 
  
  // ############################## //
}

// ############### LOOP ################# //

void loop() { 

  manager.beginLoop();

  // ## JUST CHANGE IN THIS PART ## //
  
  deviceControl();

  // ############################## //
  manager.endLoop();
  delay(PERIODICITY);    
}

// ## DEFINE YOUR FUNCTIONS STARTING HERE ## //

void initDHT(){
  dht.begin(); //Inicializa o sensor
}

void deviceControl() 
{
    
  temperatureAnt = temperature;
  temperature = dht.readTemperature(); 

  humidityAnt = humidity;
  humidity = dht.readHumidity();
    
  if(temperature != temperatureAnt && !(isnan(temperature))){
    //Serial.print("Temperatura = "); 
    //Serial.print(temperature); 
    //Serial.print(" °C      |       "); 
    //Serial.print("Umidade = "); 
    //Serial.print(humidity); 
    //Serial.println(); 

    snprintf(data, sizeof(data), "%.2f", temperature); 
    strcpy(valueRequest, "t|");
    strcat(valueRequest, data);
    //Serial.print("===============> "); 
    //Serial.println(valueRequest); 

    manager.mqtt_publish(valueRequest);
  }

  if(humidity != humidityAnt && !(isnan(humidity))){
    
    snprintf(data, sizeof(data), "%.2f", humidity); 
    strcpy(valueRequest, "h|");
    strcat(valueRequest, data);
    //Serial.print("===============> "); 
    //Serial.println(valueRequest); 
    //Serial.println(); 

    manager.mqtt_publish(valueRequest);
         
  }
    
}
