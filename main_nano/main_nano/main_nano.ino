void setup() {
  // init UART baudrate to 9600
  Serial.begin(9600);
  // setup longcan parameters
  Serial.print("+++");  // init config mode
  Serial.println("AT+C=08");   // set CAN baud rate to '08' => 50k/s
  Serial.println("AT+F=[0][0][00000001]");   // only read messages from ID 1 (the aduino giga)
}

void loop() {
  // place any received message in receivedData
  while (Serial.available() >= 0) {
    char receivedData = Serial.read();
  }
  // operations on receivedData to determine received cmde
}
