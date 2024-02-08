#include <M5Stack.h>
#include "emulatetag.h"
#include "NdefMessage.h"

#include <PN532_HSU.h>
#include <PN532.h>
PN532_HSU pn532hsu(Serial2);
EmulateTag nfc(pn532hsu);

uint8_t ndefBuf[120];
NdefMessage message;
int messageSize;


uint8_t uid[3] = {0x55, 0x5D, 0xB4};

int counter = 0;
int step = 100;
String text;
String baseText = "021905300";

void setup()
{
  Serial.begin(115200);
  Serial.println("------- FABOLOUS START --------");

  M5.begin();        
  M5.Power.begin();  
  M5.Lcd.fillScreen(BLACK); 

  M5.Lcd.setCursor(10, 10); 
  M5.Lcd.setTextColor(GREEN); 
  M5.Lcd.setTextSize(3); 
  M5.Lcd.printf("FABALOUS");
  
  // uid must be 3 bytes!
  nfc.setUid(uid);

  setNfcText(baseText);

  nfc.init();
}

void setNfcText(String text){
  message = NdefMessage();
  message.addTextRecord(text);
  messageSize = message.getEncodedSize();
  if (messageSize > sizeof(ndefBuf))
  {
    Serial.println("ndefBuf is too small");
    while (1)
    {
    }
  }
  Serial.print("Ndef encoded message size: ");
  Serial.println(messageSize);
  message.encode(ndefBuf);
  nfc.setNdefFile(ndefBuf, messageSize);
}

void loop()
{
  M5.update();

  if (M5.BtnA.wasReleased()|| M5.BtnA.pressedFor(100)) {   
    counter += step; 
    char counterChars[5];
    sprintf(counterChars, "%05d", counter);
    String counterText = String(counterChars);
    text = baseText + counterText;
    Serial.println(text);

    M5.Lcd.fillScreen(BLACK); 

    M5.Lcd.setCursor(10, 10); 
    M5.Lcd.setTextColor(GREEN); 
    M5.Lcd.setTextSize(3); 
    M5.Lcd.printf("FABALOUS");

    M5.Lcd.setCursor(10, 70); 
    M5.Lcd.setTextColor(YELLOW); 
    M5.Lcd.setTextSize(4); 
    M5.Lcd.printf(counterText.c_str());

    M5.Lcd.setCursor(10, 120); 
    M5.Lcd.setTextColor(YELLOW); 
    M5.Lcd.setTextSize(3); 
    M5.Lcd.printf(text.c_str());

    setNfcText(text);
  }
    
  if(!nfc.emulate(500)){ // timeout 1 second
      Serial.println(".");
  }
  
  if (nfc.writeOccured())
  {
    Serial.println("\nWrite occured !");
    uint8_t *tag_buf;
    uint16_t length;

    nfc.getContent(&tag_buf, &length);
    NdefMessage msg = NdefMessage(tag_buf, length);
    msg.print();
  }

  delay(1);
}



