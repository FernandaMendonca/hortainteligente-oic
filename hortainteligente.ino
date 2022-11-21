#include <ESP8266WiFi.h> 
#include <PubSubClient.h> 
 
//configs
#define SSID_REDE     "*********"
#define SENHA_REDE    "*********" 
#define INTERVALO_ENVIO_MQTT        10000 
  
// //defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_SUBSCRIBE "iotplantafm/sub"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "iotplantafm/pub"    //tópico MQTT de envio de informações para Broker
#define ID_MQTT  "mqtt-explorer-0130ff8d"     //id mqtt (para identificação de sessão)
  
//constantes e variáveis globais
const char* BROKER_MQTT = "test.mosquitto.org"; //url do broker MQTT 
int BROKER_PORT = 1883;                      // porta do Broker MQTT
long lastConnectionTime; 
long lastMQTTSendTime;
WiFiClient client;
WiFiClient clientMQTT;
PubSubClient MQTT(clientMQTT);
  

float FazLeituraUmidade(void);
void initWiFi(void);
void initMQTT(void);
void reconectWiFi(void); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void); 
 
/*
 * implementações
 */
  

void initWiFi() 
{
    delay(10000);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID_REDE);
    Serial.println("Aguarde");
      
    reconectWiFi();
}
   

void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
    MQTT.setCallback(mqtt_callback);          
}
   

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
         
}
   

void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}
   

void reconectWiFi() 
{

    if (WiFi.status() == WL_CONNECTED)
        return;
          
    WiFi.begin(SSID_REDE, SENHA_REDE); // Conecta na rede WI-FI
      
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(10000);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID_REDE);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
  

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT();
      
     reconectWiFi();
}
  

float FazLeituraUmidade(void)
{
    int ValorADC;
    float UmidadePercentual;
  
     ValorADC = analogRead(0);   //978 -> 3,3V
     Serial.print("[Leitura ADC] ");
     Serial.println(ValorADC);
  
     //Quanto maior o numero lido do ADC, menor a umidade.
     //Sendo assim, calcula-se a porcentagem de umidade por:
     //      
     //   Valor lido                 Umidade percentual
     //      _    0                           _ 100
     //      |                                |   
     //      |                                |   
     //      -   ValorADC                     - UmidadePercentual 
     //      |                                |   
     //      |                                |   
     //     _|_  978                         _|_ 0
     //
     //(UmidadePercentual-0) / (100-0)  =  (ValorADC - 978) / (-978)
     //Logo:
     //UmidadePercentual = 100 * ((418-ValorADC) / 978)  
       
     UmidadePercentual = 100 * ((978-(float)ValorADC) / 978);
     Serial.print("[Umidade Percentual] ");
     Serial.print(UmidadePercentual);
     Serial.println("%");

     if (ValorADC < 489) {
       digitalWrite(D1, HIGH);
       Serial.println("Seco");
       digitalWrite(D1, LOW);
       

     } else {
       digitalWrite(D2, HIGH);
       Serial.println("Umido");
       digitalWrite(D2, LOW);
     }
  
     return UmidadePercentual;
}


void setup()
{  
    Serial.begin(9600);
    lastConnectionTime = 0; 
    lastMQTTSendTime = 0;
    initWiFi();
    initMQTT();
    Serial.println("Planta IoT");
    pinMode(D1,OUTPUT);
    pinMode(D2,OUTPUT);
}
  

void loop()
{
    float UmidadePercentualLida;
    int UmidadePercentualTruncada;
    char FieldUmidade[11];
    char MsgUmidadeMQTT[50];
      
    VerificaConexoesWiFIEMQTT(); 
        
    UmidadePercentualLida = FazLeituraUmidade();
    UmidadePercentualTruncada = (int)UmidadePercentualLida;

    if ((millis() - lastMQTTSendTime) > INTERVALO_ENVIO_MQTT)
    {
        sprintf(MsgUmidadeMQTT,"- Umidade baixa, hora de regar!!.",UmidadePercentualTruncada);
        MQTT.publish(TOPICO_PUBLISH, MsgUmidadeMQTT);
        lastMQTTSendTime = millis();
    }
     
    delay(5000);
}
