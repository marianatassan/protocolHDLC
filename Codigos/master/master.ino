#define SLAVE1_ID 2
#define SLAVE2_ID 3

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Enviar uma mensagem para o Escravo 1
  sendToSlave(SLAVE1_ID, "Hello from Master to Slave 1");
  delay(1000);

  // Enviar uma mensagem para o Escravo 2
  sendToSlave(SLAVE2_ID, "Hello from Master to Slave 2");
  delay(1000);
}

void sendToSlave(int slaveID, const char* message) {
  Serial.write('<'); // Caractere de início de quadro
  Serial.write(slaveID); // ID do escravo
  Serial.write(' '); // Espaço como separador
  Serial.print(message); // Mensagem
  Serial.write('>'); // Caractere de fim de quadro
}
