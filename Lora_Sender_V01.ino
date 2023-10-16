#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A3
#define DO_PIN A4
#define VREF 5000  
#define ADC_RES 1024 
#define CAL1_V (1700) 
#define CAL1_T (25)  

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int pinTurbidity = A1;
const int pinPH = A2;

int suhu;
int senTurbi;
float volt;
int ntu;
float teg;
float calibration_value = 21.34 + 0.3;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;
int ph_act;
String dt;
unsigned long prev;
uint8_t temperature_c;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
int DO;

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c){
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
}

void setup(){
  Serial.begin(9600);
  sensors.begin();
  
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa begin successed");
  LoRa.beginPacket();
  LoRa.print("Ready!!!");
  LoRa.endPacket();
}

void loop(){
  cekTemp();
  cekDO();
  cekturbi();
  cekPH();

    dt = String(suhu) + "," + String(ntu) +  "," + String(ph_act) +  "," + String(DO);
    Serial.print("Data Send : ");
    Serial.println(dt);    
    LoRa.beginPacket();
    LoRa.print(dt);
    LoRa.endPacket();
  
  delay(10000);
}


void cekTemp(){
  sensors.requestTemperatures(); 
  suhu = sensors.getTempCByIndex(0);
    Serial.print("Temperature : ");
    Serial.print(suhu);
    Serial.println("*C");
 
}

void cekturbi(){
  senTurbi = analogRead(pinTurbidity);
  volt = senTurbi * (5.0 / 1024.0); 
  if(volt > 4.20)volt = 4.20;
  if(volt < 2.50)volt = 2.50;
  ntu = -1120.4*(sq(volt)) + (5742.3*(volt)-4352.9);
  // Serial.print("Volt : ");Serial.print(volt);
  Serial.print("NTU : ");Serial.println(ntu);
}

void cekPH(){
  for(int i=0;i<10;i++) { 
    buffer_arr[i]=analogRead(pinPH);
  }
  for(int i=0;i<9;i++){
      for(int j=i+1;j<10;j++){
        if(buffer_arr[i]>buffer_arr[j]){
          temp=buffer_arr[i];
          buffer_arr[i]=buffer_arr[j];
          buffer_arr[j]=temp;
        }
      }
  }
  avgval = 0;
  for(int i=2;i<8;i++){
      avgval+=buffer_arr[i];
  } 
  teg = (float)avgval*5.0/1024/6; 
  ph_act = -5.70 * teg + calibration_value;
  Serial.print("pH Val: ");
  Serial.println(ph_act);
  Serial.println("\n");
}

void cekDO(){
  ADC_Raw = analogRead(DO_PIN);
  ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;
  DO = readDO(ADC_Voltage, suhu)/1000;
  Serial.print("DO:\t");
  Serial.println(DO);
}
