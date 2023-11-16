#define MESTRE 0x01
#define ESCRAVO1 0x02
#define ESCRAVO2 0x03
#define FLAG 0x7E

uint8_t stringToUint(const String& str) {
    // Converte a string para um número inteiro
    int intValue = str.toInt();
    return intValue;
}

uint16_t calculoCRC16(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;

        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
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
                // Remove os últimos 5 caracteres para obter o CRC recebido
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
                    Serial.write(FLAG);
                    Serial.print(MESTRE);
                    Serial.print("Mensagem válida");
                    Serial.write(FLAG);
                    Serial.print(" - Mensagem: ");
                    Serial.println(mensagemRecebida);
                    delay(2000);
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
