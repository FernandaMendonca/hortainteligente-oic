# Horta Inteligente
Projeto desenvolvido para a disciplina de Objetos Inteligentes Conectados do curso de Sistemas de Informação, na turma 05J.

Fernanda da Silva Mendonça - 41890280

O objetivo desse projeto é desenvolver uma aplicação que a partir do valor identificado pelo sensor de umidade do solo, acionar um bot no Telegram avisando que já é hora de regar as plantas.

# Componentes

|   |
|---|
| Placa NodeMCU ESP8266 
| Sensor de umidade de solo Higrômetro
| Leds

# Métodos

* [Protocolo MQTT](https://mosquitto.org/)
* [Node Red](https://nodered.org/)
* [Bot Telegram](https://telegram.me/BotFather)

A comunicação da aplicação é feita com o protocolo MQTT com o broker Mosquittto, a partir do valor retornado pela placa do NodeMCU, é usado uma instância do NodeRED para enviar uma mensagem por um bot do Telegram.
