#include <Arduino.h>
#include <vector>
#include <iostream>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <string>
#include <SD.h>
#include <SdFat.h>
using namespace std;
#define BUFFER_SIZE 256
byte buffer_hsp24[BUFFER_SIZE];
vector<int> BufferRX;
const char *SDFILENAME = "keycardsTID.txt";
static const char *const Startframe[4] = {"FD", "FC", "FB", "FA"};
const int chipSelect = BUILTIN_SDCARD;
void start_up()
{
  /*N0,00
  readRFIDReaderpower
  N1,<value>
  setRFIDReaderpower
  (-2~25dBm)
  <value>00~1B*/
  byte Power[] = {0x0A, 0x4E, 0x31, 0x2C, 0x1B, 0x0D};
  int sendPower = sizeof(Power);
  // Serial1.write(sendData, sendDataLength);
  memset(buffer_hsp24, 0, sizeof(buffer_hsp24));
  Serial1.write(Power, sendPower);
}
File Keys;

byte TID[16];
struct IDs
{
  byte ID_value[16];
  int Number;
  // IDs(int Id_number, byte TID_Value[16]) : Number(Id_number), ID_value(TID_Value[16]) {}
};
void setup()
{
  while (!Serial)
  {
  }
  SD.begin(chipSelect);
  if (!SD.begin(chipSelect))
  {
    Serial.print("SD CARD FAILED TO INITILIZE");
  }
  // SD.remove(SDFILENAME);
  if (!SD.exists(SDFILENAME))
  {
    Keys = SD.open(SDFILENAME, FILE_WRITE);
    Keys.close();
  }
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(38400);
  start_up();
  // HWSERIAL.addMemoryForWrite(Startframe[], 4);
}
String Combo = "";
void StartTX()
{
  // char sendData[] = {"b'\nR2,0,6\r'"};
  String User_Input = Serial.readString();

  //  int ConfigDataLength = sizeof(ConfigData);
  byte sendData[] = {0x0A, 0x52, 0x32, 0x2C, 0x30, 0x2C, 0x34, 0x0D}; // byte sendData[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x61, 0x00, 0x  04, 0x03, 0x02, 0x01};
  int sendDataLength = sizeof(sendData);
  // Serial1.write(sendData, sendDataLength);
  memset(buffer_hsp24, 0, sizeof(buffer_hsp24));
  int bufferIndex_hsp24 = 0;

  if (User_Input == "READ")
  {
    Serial1.write(sendData, sendDataLength);
    Serial1.readBytes(buffer_hsp24, 24);
    int Read_size = sizeof(buffer_hsp24);
    if (Read_size > 6)
    {
      Serial.println(Read_size);
      while (bufferIndex_hsp24 < Read_size)
      {
        Serial.print(buffer_hsp24[bufferIndex_hsp24], HEX);
        Serial.print("  ");
        bufferIndex_hsp24++;
      }
    }
  }
  if (buffer_hsp24[0] == 0x0A)
  {
    int cycles = 1;
    bool Checking = true;
    while (Checking == true)
    {
      if (buffer_hsp24[cycles] == 0x0D)
      {
        Checking = false;
        Serial.println(" Faslse");
      }
      else
      {
        TID[cycles - 1] = buffer_hsp24[cycles];
        Serial.print(TID[cycles - 1], HEX);
        Serial.print("  ");
      }
      cycles++;
    }

    memcpy(&TID, &buffer_hsp24[1], cycles);
    int i = 0;
    Combo = "";
    Keys = SD.open(SDFILENAME, FILE_WRITE);
    while (i <= sizeof(TID))
    {
      Combo.append(TID[i]);
      Serial.print(TID[i]);
      Serial.print("  ");
      i++;
    }
    Serial.print(Combo);
    Keys.print(Combo);
    Keys.print(",");
    Keys.close();
  }
  Serial.println(" ");
  Serial.println("/n");
  Serial.println("/n");
}
int counter = 0;
int i = 0;

void Data_save()
{
  
  Keys = SD.open(SDFILENAME, FILE_WRITE);
  Keys.print(Combo);
  Keys.print(",");
  Keys.close();
  Serial.println("Stored data: ");
  Serial.print(Combo);
}
void card_read(bool Store_data)
{
  bool moveon = true;
  byte sendData[] = {0x0A, 0x52, 0x32, 0x2C, 0x30, 0x2C, 0x34, 0x0D}; // byte sendData[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0x61, 0x00, 0x  04, 0x03, 0x02, 0x01};
  int sendDataLength = sizeof(sendData);
  // Serial1.write(sendData, sendDataLength);
  memset(buffer_hsp24, 0, sizeof(buffer_hsp24));
  int bufferIndex_hsp24 = 0;

  Serial1.write(sendData, sendDataLength);
  Serial1.readBytes(buffer_hsp24, 24);
  int Read_size = sizeof(buffer_hsp24);
  if (Read_size > 6)
  {
    Serial.println(Read_size);
    while (bufferIndex_hsp24 < Read_size)
    {
      Serial.print(buffer_hsp24[bufferIndex_hsp24], HEX);
      Serial.print("  ");
      bufferIndex_hsp24++;
    }
  }
  if (buffer_hsp24[2] == 0x0D)
  {
    Serial.println("READ FAILED");
    moveon = false;
  }
  else if (moveon == true)
  {
    int cycles = 1;
    bool Checking = true;
    while (Checking == true)
    {
      if (buffer_hsp24[cycles] == 0x0D)
      {
        Checking = false;
        Serial.println(" Faslse");
      }
      else
      {
        TID[cycles - 1] = buffer_hsp24[cycles];
        Serial.print(TID[cycles - 1], HEX);
        Serial.print("  ");
      }
      cycles++;
    }
    Serial.print("memory copy");
    memcpy(&TID, &buffer_hsp24[1], cycles);
    Combo = "";
    int i = 0;
    while (i <= sizeof(TID))
    {
      Combo.append(TID[i]);
      Serial.print(TID[i]);
      Serial.print("  ");
      i++;
    }
    if (Store_data == true)
    {
      int i = 0;
      Combo = "";
      Keys = SD.open(SDFILENAME, FILE_WRITE);
      while (i <= sizeof(TID))
      {
        Combo.append(TID[i]);
        Serial.print(TID[i]);
        Serial.print("  ");
        i++;
      }
      Serial.print(Combo);
      Keys.print(Combo);
      Keys.print(",");
      Keys.close();
    }
  }
}

bool Chekcing()
{
  card_read(false);
  Serial.print(Combo);

  Keys = SD.open(SDFILENAME);

  if (Keys.find(Combo) == true)
  {
    Serial.println("Card on file");
    Keys.close();
    return true;
  }
  else
  {
    Serial.println("Card not on file");
    Keys.close();
    return false;
  }
}

void User_select()
{
  String User_Input = Serial.readString();
  if (User_Input == "CHECK")
  {
    Chekcing();
  }
  else if (User_Input == "READ")
  {
    card_read(false);
  }
  else if (User_Input == "Store")
  {
    bool To_store = Chekcing();
    if (To_store == true)
    {
      Serial.println("CARD already Registarted");
    }
    else if (To_store == false)
    {
      card_read(true);
    }
  }
  else if (User_Input == "WIPE")
  {
    Serial.println("WARNING: This will remove all keys saved on SD Card");
    Serial.println("Y/N");
    while (true)
    {

      String Agree = Serial.readString();
      if (Agree == "Y")
      {
        SD.remove(SDFILENAME);
        Keys = SD.open(SDFILENAME, FILE_WRITE);
        Keys.close();
        Serial.print("FILE DATA ERASED");
        break;
      }
      else if (Agree == "N")
      {
        Serial.print("OPED OUT");
        break;
      }
    }
  }
}
void loop()
{
  while (true)
  {
    User_select();
  }
  counter++;

  Serial.println(" ");
  delay(1000);
}
