void setup() {
  Serial.begin(9600);
}

void loop() {
  while (Serial.available()) {
    char byte = Serial.read();
    if (byte == '<') {
      // Início de quadro detectado
      int senderID = Serial.parseInt();
      if (senderID == 1){
        if (Serial.read() == ' ') {
          // Espaço como separador
          String message = Serial.readStringUntil('>');
          // Verificar se a mensagem é para este escravo
          Serial.print("Recebido do Master: ");
          Serial.println(message);
        } 
      }
    }
  }
}
