
//SENSOR DE TEMPERATURA

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>//Biblioteca do UDP.
#include <ArduinoJson.h>
#include <NTPClient.h>//Biblioteca do NTP.

WiFiUDP udp;//Cria um objeto "UDP".
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.
const int conect = D1;
const int enviou = D2;
void setup() {
 
  Serial.begin(115200);//CONECÇÃO da Serial
  pinMode(conect, OUTPUT);
  pinMode(enviou, OUTPUT);
  WiFi.begin("UCHOA_QUEIROS", "22041975");    //Conecção do WiFi
  Serial.print("Esperando por conecçao");
  while (WiFi.status() != WL_CONNECTED) {  //espera o wifi conectar 
    delay(500);
    Serial.print(".");
  }
     Serial.print("Conectado");
     digitalWrite(conect, HIGH);
     
   ntp.begin();//Inicia o NTP.
   ntp.forceUpdate();//Força o Update.
}

void loop() {
 
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    StaticJsonBuffer<300> JSONbuffer;   //Declara um buffer Json estatico
    StaticJsonBuffer<300> JSONbuffer2;   //Declara um buffer Json estatico
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
    JsonObject& JSONencoder2 = JSONbuffer2.createObject();
 
    float adc = analogRead(A0);// Lê a tensão do pino 17 que é o ADC do ESP8266, versão ESP-07.
    int i = 0; 
    while (i <= 30) {// Lê 30 vezes a temperatura, para obter uma média da temperatura lida.
      adc += analogRead(A0);// Vai somando as temperaturas lidas!
      Serial.print(".");
      delay(100);// delay entre cada leitura.
      i++; 
     }
     adc = adc/30;
    float c = (adc * 330.0f) / 1023.0f;// Transforma os valores lidos do pino em Celsius.
    c = round(c);
    JSONencoder2["value"] = random(10, 29);
    JSONencoder2["type"] = "Float";
    JSONencoder["temperature"] = JSONencoder2;
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    ///////////////////ENVIA OS DADOS PARA O SERVIDOR VIA POST/////////////////////////////
 
    HTTPClient http;    //Declare object of class HTTPClient
 
    http.begin("http://192.168.2.144:1026/v2/entities/Casa1/attrs");      //Especifica o destino da requisição
    http.addHeader("Content-Type", "application/json");  //Especifica o content-type header
 
    int httpCode = http.POST(JSONmessageBuffer);//Envia a requisição
    String payload = http.getString();//Recupera a resposta payload
 
    Serial.println("codigo de retorno HTTP" + httpCode);//Imprime o codigo de retorno HTTP
    Serial.println("resposta da requisição payload" + payload);//Imprime a resposta da requisição payload
    digitalWrite(enviou, HIGH);
    http.end();//Fecha conecção
 
  } else {
 
    Serial.println("Erro na conecção WIFI");
 
  }
  delay(5000);  //Send a request every 5 seconds
  digitalWrite(enviou, LOW);
  delay(5000);  //Send a request every 5 seconds
 
}
