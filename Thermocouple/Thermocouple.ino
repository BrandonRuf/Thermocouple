/*
 * For use in the Hall Effect Experiment in McGill University physics course(s) PHYS-359/439.
 * Written by Brandon Ruffolo in 2024.
 */

#include "Vrekrer_scpi_parser.h"
#include "Adafruit_MAX31856.h"

/* Serial COM parameters */
#define SKETCH_VERSION "0.0.1"
#define BAUD 115200 
#define LINEFEED "\n"

bool _debug = false;

SCPI_Parser Controller;
Adafruit_MAX31856 thermocouple = Adafruit_MAX31856(10, 11, 12, 13);

// CHECK CONVERSION READY FLAG in ADAFRUIT CODE, IT"S NOT RESETTING
// ADD FAULT functions

void setup() {
  Controller.RegisterCommand(F("*IDN?")             , &Identify);
  
  Controller.SetCommandTreeBase("THERMOcouple");
    Controller.RegisterCommand(F("TEMPerature?"), &getThermocoupleTemperature);
    Controller.RegisterCommand(F("ONESHOT")     , &setOneShot);
    Controller.RegisterCommand(F("STATUS?")     , &getStatus);
    Controller.RegisterCommand(F("TYPE?")       , &getThermocoupleType);
    Controller.RegisterCommand(F("TYPE" )       , &setThermocoupleType);
    Controller.RegisterCommand(F("MODE?")       , &getConversionMode);
    Controller.RegisterCommand(F("MODE" )       , &setConversionMode);
    
  Controller.SetCommandTreeBase("COLDJunction");
    Controller.RegisterCommand(F("TEMPerature?"), &getCJTemperature);
    //Controller.RegisterCommand(F("FAULT:CJT"),  &setCJTThreshholds);

  Controller.SetCommandTreeBase("DEBUG");
    Controller.RegisterCommand(F("INFO?"), &getDebugInfo);

  Serial.begin(BAUD);    // Enable Serial COM
  thermocouple.begin();  // Enable MAX31856 

  if(_debug) Controller.PrintDebugInfo(Serial);
}

void loop() {Controller.ProcessInput(Serial, LINEFEED);}

void Identify(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  interface.println(F("Ugrad Labs, Thermocouple Controller, v" SKETCH_VERSION ", " __DATE__));
  // "<vendor>,<model>,<serial number>,<firmware>"
}

void getDebugInfo(SCPI_C commands, SCPI_P parameters, Stream& interface){
  Controller.PrintDebugInfo(Serial);
}

void getThermocoupleTemperature(SCPI_C commands, SCPI_P parameters, Stream& interface){
  float temperature = thermocouple.readThermocoupleTemperature();
  interface.println( String(temperature,4) );
}

void getCJTemperature(SCPI_C commands, SCPI_P parameters, Stream& interface){
  float temperature = thermocouple.readCJTemperature();
  interface.println( String(temperature,4) );
}

void getStatus(SCPI_C commands, SCPI_P parameters, Stream& interface){
  bool val = thermocouple.conversionComplete();
  interface.println(val);
}

void setOneShot(SCPI_C commands, SCPI_P parameters, Stream& interface){
  thermocouple.triggerOneShot();
}

void getConversionMode(SCPI_C commands, SCPI_P parameters, Stream& interface){
  max31856_conversion_mode_t mode = thermocouple.getConversionMode();
  if (mode == MAX31856_ONESHOT)             interface.println("ONESHOT");
  else if (mode == MAX31856_ONESHOT_NOWAIT) interface.println("ONESHOT_NOWAIT");
  else if (mode == MAX31856_CONTINUOUS)     interface.println("CONTINUOUS");
  
}

void setConversionMode(SCPI_C commands, SCPI_P parameters, Stream& interface){
  if(String(parameters[0]) == "ONESHOT")             thermocouple.setConversionMode(MAX31856_ONESHOT);
  else if(String(parameters[0]) == "ONESHOT_NOWAIT") thermocouple.setConversionMode(MAX31856_ONESHOT_NOWAIT);
  else if(String(parameters[0]) == "CONTINUOUS")     thermocouple.setConversionMode(MAX31856_CONTINUOUS);
}

void getThermocoupleType(SCPI_C commands, SCPI_P parameters, Stream& interface){
  max31856_thermocoupletype_t type = thermocouple.getThermocoupleType();
  if(type == MAX31856_TCTYPE_K)      interface.println("K");
  else if(type == MAX31856_TCTYPE_T) interface.println("T"); 
  else if(type == MAX31856_TCTYPE_J) interface.println("J"); 
  else if(type == MAX31856_TCTYPE_B) interface.println("B"); 
  else if(type == MAX31856_TCTYPE_N) interface.println("N"); 
  else if(type == MAX31856_TCTYPE_R) interface.println("R"); 
  else if(type == MAX31856_TCTYPE_S) interface.println("S");
  else if(type == MAX31856_TCTYPE_E) interface.println("E"); 
}

void setThermocoupleType(SCPI_C commands, SCPI_P parameters, Stream& interface){
  if(String(parameters[0]) == "K")      thermocouple.setThermocoupleType(MAX31856_TCTYPE_K);
  else if(String(parameters[0]) == "T") thermocouple.setThermocoupleType(MAX31856_TCTYPE_T);
  else if(String(parameters[0]) == "J") thermocouple.setThermocoupleType(MAX31856_TCTYPE_J);
  else if(String(parameters[0]) == "B") thermocouple.setThermocoupleType(MAX31856_TCTYPE_B);
  else if(String(parameters[0]) == "N") thermocouple.setThermocoupleType(MAX31856_TCTYPE_N);
  else if(String(parameters[0]) == "R") thermocouple.setThermocoupleType(MAX31856_TCTYPE_R);
  else if(String(parameters[0]) == "S") thermocouple.setThermocoupleType(MAX31856_TCTYPE_S);
  else if(String(parameters[0]) == "E") thermocouple.setThermocoupleType(MAX31856_TCTYPE_E);
}

void setFaultThreshholds(SCPI_C commands, SCPI_P parameters, Stream& interface){
  //
}

void getFault(SCPI_C commands, SCPI_P parameters, Stream& interface){
  uint8_t fault = thermocouple.readFault();
  interface.println(String(fault,DEC));
}




