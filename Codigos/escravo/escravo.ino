void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    if (receivedChar == '<') {
      // Início de quadro detectado
      int senderID = Serial.parseInt();
      if (Serial.read() == ' ') {
        // Espaço como separador
        String message = Serial.readStringUntil('>');
        // Verificar se a mensagem é para este escravo
        if (senderID == 2) {
          Serial.print("Recebido do Master: ");
          Serial.println(message);
          
        }
      }
    }
  }
}
