#define toggleBuzzerDelay 5000

#include <LiquidCrystal.h>
#include <Servo.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int servoPin = 8, ServoPos = 0;
Servo ServoMotor;

#define BuzzerPin A5

#include <SoftwareSerial.h>
SoftwareSerial Wifi(11, 10);
String ssid = "raspberry";
String password = "P@ssw0rd";
String server_id = "api.thingspeak.com";
String api_key = "L5DS5C09UHAEJN81";    //sanjana2019 //Rangarajan05
//String api_key = "WCM90PZ63EEUC4DF";
String GET_data;
String receiveBuffer;
int wifiSendCount = 0;

void setup() {
  pinMode(12, INPUT_PULLUP);
  Serial.begin(9600);
  Wifi.begin(9600);
  lcd.begin(16, 2);
  ServoMotor.attach(servoPin);
  ServoPos = ServoMotor.read();

  pinMode(12, INPUT_PULLUP);
  initializeBuzzer();
  lcd.print("Installing. ");
  int count = 0;
  while (count < 100) {
    lcd.setCursor(13, 0);
    lcd.print(count);
    count += random(10);
    lcd.print("%");
    delay(random(100) * 10);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test Servo");
  SetServo(0);
  SetServo(90);
  SetServo(0);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test Buzzer");
  for (int i = 0; i < 3; i++) {
    testBuzzer();
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Searching Wifi...");

  if (sendWifi("AT")) {
    Serial.println("ESP8266 Started");
    if (sendWifi("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"")) {
      lcd.clear();
      lcd.print("WIFI CONNECTED");
      delay(3000);
      Serial.println("Connected To Wifi Hotspot");
    } else {
      lcd.clear();
      lcd.print("AP NOT DETECTED");
      while (1);
      Serial.println("Not Connected With Wifi");
    }
  } else {
    lcd.clear();
    lcd.print("CHECK WIRING!");
    while (1);
    Serial.println("Error to Start ESP8266");
  }

  lcd.clear();
  lcd.setCursor(0, 0);

}

void loop() {
  for (int i = 0; i < 24; i++) {
    if (i == 8) {
      toggleBuzzer(toggleBuzzerDelay);
    } else if (i == 14) {
      toggleBuzzer(toggleBuzzerDelay);
    } else if (i == 21) {
      toggleBuzzer(toggleBuzzerDelay);
    }
    lcd.setCursor(0, 0);
    lcd.print("TIME : ");
    lcd.setCursor(7, 0);
    if (i < 10) {
      lcd.print("0");
    }
    lcd.print(i);
    lcd.print(":");
    for (int j = 0; j < 60; j++) {
      lcd.setCursor(10, 0);
      if ( j < 10) {
        lcd.print("0");
      }
      lcd.print(j);
      if(!digitalRead(12)){ delay(50);
        if(!digitalRead(12)){
          SetServo(90);
          sendData();
          delay(5000);
          SetServo(0);
          while(!digitalRead(12));
        }
        
      } else {
        delay(100);
      }
    }
  }
}

void SetServo(int SetServo) {
  for (ServoPos = ServoPos; ServoPos < SetServo; ServoPos++) {
    ServoMotor.write(ServoPos);
    delay(10);
  }
  for (ServoPos = ServoPos; ServoPos > SetServo; ServoPos--) {
    ServoMotor.write(ServoPos);
    delay(10);
  }
}

void initializeBuzzer() {
  pinMode(BuzzerPin, OUTPUT);
}

void testBuzzer() {
  digitalWrite(BuzzerPin, HIGH);
  delay(1000);
  digitalWrite(BuzzerPin, LOW);
  delay(1000);
}

void onBuzzer() {
  digitalWrite(BuzzerPin, HIGH);
}

void offBuzzer() {
  digitalWrite(BuzzerPin, LOW);
}

void toggleBuzzer(int buzzerDelay) {
  digitalWrite(BuzzerPin, HIGH);
  delay(buzzerDelay);
  digitalWrite(BuzzerPin, LOW);
}

int sendWifi(String at_command) {
  if (wifiCommand(at_command).indexOf("OK\r\n") > 0) {
    return 1;
  } else {
    return 0;
  }
}

String wifiCommand(String at_command) {
  Serial.println(at_command);
  Wifi.println(at_command);
  do {
    receiveBuffer = Wifi.readString();
    Serial.println(receiveBuffer);
  } while (!(receiveBuffer.indexOf("OK\r\n") > 0) && !(receiveBuffer.indexOf("ERROR\r\n") > 0) && !(receiveBuffer.indexOf("FAIL\r\n") > 0));
  return receiveBuffer;
}

void sendData() {
  int pulse = 1;
  lcd.setCursor(0, 1);
  lcd.print("Uploading Wait...");
  delay(250);
  if (sendWifi("AT+CIPSTART=\"TCP\",\"" + server_id + "\",80")) {
    Serial.println("Server Connected");
    GET_data = "GET /update?api_key=" + api_key + "&field1=" + pulse + " HTTP/1.1\r\nHost: " + server_id + "\r\nConnection: close\r\n\r\n";
    if (wifiCommand("AT+CIPSEND=" + String(GET_data.length() + 2))) {
      sendWifi(GET_data);
    } else {
      Serial.println("Error to Send Data");
    }
  } else {
    Serial.println("Server Not Connected");
  }
  delay(200);
  lcd.setCursor(0,1);
  lcd.print("                 ");
}
