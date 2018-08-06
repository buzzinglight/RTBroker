unsigned long lastTime = 0;

void setup() {
   Serial.begin(115200);
}

void loop() {
  if((millis() - lastTime) > 2000) {
    lastTime = millis();
    Serial.println("/websockets Arduino is waiting for messages!");
  }
}

//Serial port reception
String serialInput = "";
void serialEvent() {
  //Master serial port
  while (Serial.available()) {
    char receivedChar = (char)Serial.read();

    //Switch depending on received character
    if (receivedChar == '\n') {
      Serial.print("/websockets Arduino received: ");
      Serial.println(serialInput);
      serialInput = "";
    }
    else
      serialInput += receivedChar;
  }
}
