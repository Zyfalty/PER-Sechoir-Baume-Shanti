// ENIB 2026-2027
// PER Module
// Project Sechoir Baume Shanti
// intended for Arduino Nano every

#define ARDUINO_GIGA_ID 1
#define BROADCAST_ID    2
#define DEFAULT_ID      3

#define FCT_ID_DEBUG    0

unsigned int my_id = DEFAULT_ID;
// set fct_id to the default value
// possible values are :
// 0×0 : test/debug
// 0×1 : ask for temperature and humidity (Giga only)
// 0×2 : temperature and humidity are XX / XX (Nano only)
// 0×3 : ask for new ID (Nano only)
// 0×4 : give new ID (Giga only)
// 0×5 : notify of new ID (Nano only)
unsigned int fct_id = FCT_ID_DEBUG;

void setup_id(){
  // set id to default
  my_id = DEFAULT_ID;
  // ask the Giga card for an id
  fct_id = 0x3; // used to ask for a new available id

  unsigned char can_msg[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  can_msg[0] = ARDUINO_GIGA_ID>>24;        // id3
  can_msg[1] = ARDUINO_GIGA_ID>>16&0xff;   // id2
  can_msg[2] = ARDUINO_GIGA_ID>>8&0xff;    // id1
  can_msg[3] = ARDUINO_GIGA_ID&0xff;       // id0
    
  can_msg[6] = fct_id;

  Serial.println(*can_msg);
}

void notify_changed_id(){
  fct_id = 5; // used to notify others that the id is used

  unsigned char can_msg[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  can_msg[0] = BROADCAST_ID>>24;        // id3
  can_msg[1] = BROADCAST_ID>>16&0xff;   // id2
  can_msg[2] = BROADCAST_ID>>8&0xff;    // id1
  can_msg[3] = BROADCAST_ID&0xff;       // id0
    
  can_msg[6] = fct_id;
    
  for(int i=0; i<8; i++){
    can_msg[7+i] = my_id>>(48-8*i)&0xff;
  }
  Serial.println(*can_msg);
}

void change_id(char *buff){
  // set the id to the one received from the Arduino Giga
  my_id = buff;
  // notify all of my new id
  notify_changed_id();
}

void check_id(char *buff){
  // check if id is the same as the one someone notified, if so, reset id
  unsigned int notified_id = buff;
  if (my_id = notified_id){
    setup_id();
  }
}

void send_temp_humi(){
  // send measured temperature and humidity to the Arduino Giga
}

void setup() {
  // init UART baudrate to 9600
  Serial.begin(9600);
  // setup longcan parameters
  Serial.print("+++");                        // init config mode
  Serial.println("AT+C=08");                  // set CAN baud rate to '08' => 50k/s
  Serial.println("AT+F=[0][0][00000001]");    // read messages from ID 1 (the aduino giga)
  Serial.println("AT+F=[0][0][00000003]");    // read broadcasted messages
  Serial.println("AT+Q");                     // switch back to normal mode

  setup_id();
}

void loop() {
  // 
  int len = 0;
  unsigned char dta[20];
  unsigned char buff[7];

  while (Serial.available() >= 0) {
    dta[len++] = Serial.read();
    if (len==12){
      break;
    }
  }
  // operations on received data to determine received cmde
  if (len==12){  // just to safeguard
    // extract the fct_id
    unsigned int fct_to_call = dta[5];
    // store the content of the message in a buffer
    for(int i=0; i<7; i++){
      buff[i] = dta[i+5];
    }
    // use appropriate function
    switch(fct_to_call){
      // 0×0 : test/debug => nothing to do
      case 0x1 :  // 0×1 : ask for temperature and humidity (Giga only)
        send_temp_humi();
      // 0×2 : temperature and humidity are XX / XX (Nano only) => should not be received
      // 0×3 : ask for new ID (Nano only) => should not be received
      case 0x4 :  // 0×4 : give new ID (Giga only)
        change_id(buff);
      case 0x5 :  // 0×5 : notify of new ID (Nano only)
        check_id(buff);
    }
  }
}
