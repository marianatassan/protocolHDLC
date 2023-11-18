#define MESTRE 0x01
#define ESCRAVO1 0x02
#define ESCRAVO2 0x03
#define FLAG 0x7E

//Função para calcular o CRC
uint16_t calculoCRC16(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF; //valor de inicio do CRC

    //Percorre cada byte
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8; //XOR entre o CRC e o dado deslocado 8 bits

        //Percorre cada bit do byte
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) { //Verifica se o bit mais significativo (15) é igual a 1
                crc = (crc << 1) ^ 0x1021; //XOR do crc deslocado 1 bit com o Polinômio gerador
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    static String mensagemRecebida;
    
    while (Serial.available()) {
        char flag = Serial.read();

        if (flag == FLAG) {
            int endereco = mensagemRecebida.toInt();

            if (endereco == ESCRAVO1) {
                // Pega os últimos 5 caracteres para obter o CRC recebido
                String crcRecebidoStr = mensagemRecebida.substring(mensagemRecebida.length() - 5);
                uint16_t crcRecebido = crcRecebidoStr.toInt();

                // Remove o CRC da mensagem
                mensagemRecebida.remove(mensagemRecebida.length() - 5, 5);
                
                // Remove o endereço da mensagem
                mensagemRecebida.remove(0, 1);
                
                // Converte a string da mensagem para bytes
                const char* bytes = mensagemRecebida.c_str();
                size_t tamanho = strlen(bytes);

                // Calcula o CRC dos bytes recebidos
                uint16_t crcCalculado = calculoCRC16(reinterpret_cast<const uint8_t*>(bytes), tamanho);

                // Verifica se os CRCs coincidem
                if (crcCalculado == crcRecebido) {
                  if(mensagemRecebida == "Escravo 2"){
                      Serial.write(FLAG);
                      Serial.print(MESTRE);
                      Serial.print("sim");
                      Serial.write(FLAG);
                      Serial.print(" Mensagem: ");
                      Serial.println(mensagemRecebida);
                      delay(2000);
                  }
                  else if(mensagemRecebida == "Manda a mensagem"){
                      Serial.write(FLAG);
                      Serial.print(MESTRE);
                      Serial.print("Olá escravo 2 - S1");
                      Serial.write(FLAG);
                      Serial.print(" Mensagem: ");
                      Serial.println(mensagemRecebida);
                      delay(2000);
                  }
                  else{
                      Serial.write(FLAG);
                      Serial.print(MESTRE);
                      Serial.print("Mensagem válida");
                      Serial.write(FLAG);
                      Serial.print(" Mensagem: ");
                      Serial.println(mensagemRecebida);
                      delay(2000);
                  }
                } else {
                    Serial.write(FLAG);
                    Serial.print(MESTRE);
                    Serial.write("Erro CRC");
                    Serial.write(FLAG);
                    Serial.println();
                    delay(2000);
                }
            } 

            // Limpa a mensagem recebida para a próxima leitura
            mensagemRecebida = "";
        } else {
            mensagemRecebida += flag;
        }
    }
}
