void setup() {
  Serial.begin(9600);
}

void loop() {
  sendHDLC(1, "Dados para escravo 1");
  delay(1000);
  sendHDLC(2, "Dados para escravo 2");
  delay(1000);
}

void sendHDLC(uint8_t address, const char* data) {
  Serial.write("<");  // Delimitador de início
  Serial.print(address);  // Endereço do escravo
  Serial.write(" ");
  Serial.write(data);
  Serial.write(">"); //Delimitador de fim.
}
