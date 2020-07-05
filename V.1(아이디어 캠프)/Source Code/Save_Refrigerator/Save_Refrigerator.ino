#include <ESP8266WiFi.h>
#include <DHT.h> 
#include <SPI.h>
#include <MFRC522.h> 

void Touch_Value(int Touch, float humi, float temp, int Time);//터치 신호 서버 업로드
void RFID_Value(int RFID, float humi, float temp, int Time);//RFID 신호 서버 업로드

//서버에 전송하기 위한 초기설정
const char* server = "api.thingspeak.com";
String apiKey = "PFOGQ3D9M68TXFXE";//Write api key
const char* MY_SSID = "Herthstone";//와이파이 이름
const char* MY_PWD = "talmojoa";//와이파이 비밀번호

const int TouchPin = D2;//터치센서핀
const int DHTPin = D4;//온습도센서핀
const int RST_PIN = D1;// reset핀
const int SS_PIN = D8;// SS핀

DHT dht(DHTPin, DHT11); //DHT11(온습도센서)
MFRC522 mfrc(SS_PIN, RST_PIN); //RIFD

// 초기 설정(시리얼 통신, 핀 설정)
void setup() {
	Serial.begin(115200);
	delay(10);
	dht.begin();//온습도센서 초기화
	SPI.begin();//RFID 초기화
	pinMode(TouchPin, INPUT);//터치센서 설정 
                    
	mfrc.PCD_Init();//RFID

	//와이파이 연결
	Serial.print("Connecting to " + *MY_SSID);

	WiFi.begin(MY_SSID, MY_PWD);

	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("Connected");
	Serial.println("");
}

void loop() {
	unsigned long long Start_time = millis();//시작 시간 측정
	float humi = dht.readHumidity();//습도 측정
	float temp = dht.readTemperature();//온도 측정

	//30초 경과 하면 루프 break
	while ((millis() - Start_time) < 15000) {
		int Touch = digitalRead(TouchPin);

		//터치 감지 시 출력 및 서버 전송
		if (Touch == 1) {
			Serial.print("터치 : ");
			Serial.println(Touch);
			int Time = 15000 - (millis() - Start_time);//실행시간

			Serial.println(Time);
			delay(50);

			Touch_Value(Touch, humi, temp, Time); //터치 데이터 입력
		}

		delay(50);

		//RFID 태그 감지 시 출력 및 서버 전송
		if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) {// 태그 접촉이 되지 않았을때 또는 ID가 읽혀지지 않았을때
			delay(50);
			continue;
		}

		int Teg = mfrc.uid.uidByte[0];
		Serial.print("Card UID:");// 태그의 ID출력
		Serial.println(Teg);// mfrc.uid.uidByte[0] 출력
		int Time = 15000 - (millis() - Start_time);//실행시간

		Serial.println(Time);
		delay(50);

		RFID_Value(Teg, humi, temp, Time);//RFID 데이터 입력
	}

	humi = dht.readHumidity();//습도 측정
	temp = dht.readTemperature();//온도 측정

	Serial.print("온도 : ");
	Serial.println(temp);
	Serial.print("습도 : ");
	Serial.println(humi);
  
	//온습도 데이터 서버 전송
	WiFiClient client;

	if (client.connect(server, 80)) { // 서버에 전송
		Serial.println("WiFi Client connected");

		String postStr = apiKey;
		postStr += "&field1=";
		postStr += String(NULL);
		postStr += "&field2=";
		postStr += String(NULL);
		postStr += "&field3=";
		postStr += String(temp);
		postStr += "&field4=";
		postStr += String(humi);
		postStr += "\r\n\r\n";

		client.print("POST /update HTTP/1.1\n");
		client.print("Host: api.thingspeak.com\n");
		client.print("Connection: close\n");
		client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
		client.print("Content-Type: application/x-www-form-urlencoded\n");
		client.print("Content-Length: ");
		client.print(postStr.length());
		client.print("\n\n");
		client.print(postStr);
		delay(1000);
		}
	client.stop();
}

// 터치센서 데이터 서버에 전송
void Touch_Value(int Touch, float humi, float temp, int Time) {
	delay(Time);//15초 전송 간격을 맞추기 위한 딜레이

	WiFiClient client;

	if (client.connect(server, 80)) 
	{ 
		Serial.println("Touch Value transmission");

		String postStr = apiKey;
		postStr += "&field1=";
		postStr += String(Touch);
		postStr += "&field2=";
		postStr += String(NULL);
		postStr += "&field3=";
		postStr += String(temp);
		postStr += "&field4=";
		postStr += String(humi);
		postStr += "\r\n\r\n";

		client.print("POST /update HTTP/1.1\n");
		client.print("Host: api.thingspeak.com\n");
		client.print("Connection: close\n");
		client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
		client.print("Content-Type: application/x-www-form-urlencoded\n");
		client.print("Content-Length: ");
		client.print(postStr.length());
		client.print("\n\n");
		client.print(postStr);
		delay(1000);
	}
	client.stop();
}

// RFID 센서 데이터 서버에 전송
void RFID_Value(int RFID, float humi, float temp, int Time) {
	delay(Time);//15초 전송 간격을 맞추기 위한 딜레이

	WiFiClient client;

	if (client.connect(server, 80)) { // 서버에 전송
		Serial.println("RFID Value transmission");

		String postStr = apiKey;
		postStr += "&field1=";
		postStr += String(NULL);
		postStr += "&field2=";
		postStr += String(RFID);
		postStr += "&field3=";
		postStr += String(temp);
		postStr += "&field4=";
		postStr += String(humi);
		postStr += "\r\n\r\n";

		client.print("POST /update HTTP/1.1\n");
		client.print("Host: api.thingspeak.com\n");
		client.print("Connection: close\n");
		client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
		client.print("Content-Type: application/x-www-form-urlencoded\n");
		client.print("Content-Length: ");
		client.print(postStr.length());
		client.print("\n\n");
		client.print(postStr);
		delay(1000);
	}
	client.stop();
}
