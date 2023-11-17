//DEFINE OS ENDEREÇO E FLAG
#define MESTRE 0x01
#define ESCRAVO1 0x02
#define ESCRAVO2 0x03
#define FLAG 0x7E

//DEFINE OS ESTADOS
enum States {
  FRAME1,
  FRAME2,
  FRAME3,
  FRAME4,
  CHECK1,
  CHECK2,
  CHECK3,
  CHECK4,
};

//ESTADO INICIAL
States state = FRAME1;

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

//Função para enviar mensagem
void EnviarMensagem(uint8_t endereco, const char* data, uint8_t dataLength, uint16_t crc) {
  Serial.write(FLAG);  // Delimitador de início -> ~
  Serial.print(endereco);  // Endereço do escravo -> até 8 bits = 256 endereços
  for (size_t i = 0; i < dataLength; i++) {
      Serial.print(data[i]); // Data
  }
  Serial.print(crc); // CRC
  Serial.write(FLAG); // Delimitador de fim. -> ~
}

// FRAME 1 - ESCRAVO 1
void EnviarFrame1(){
    uint8_t frame1[] = {0x4f, 0x6c, 0x61, 0x20, 0x45, 0x73, 0x63, 0x72,  0x61, 0x76, 0x6f, 0x31};
    size_t dataLength1 = sizeof(frame1);
    uint16_t crc1 = calculoCRC16(frame1, dataLength1);
    EnviarMensagem(ESCRAVO1, frame1, dataLength1, crc1); 
}

// FRAME 2 - ESCRAVO 1
void EnviarFrame2(){
  uint8_t frame2[] = {0x54, 0x75, 0x64, 0x6f, 0x20, 0x62, 0x65, 0x6d,  0x3f};
  size_t dataLength2 = sizeof(frame2);
  uint16_t crc2 = calculoCRC16(frame2, dataLength2);
  EnviarMensagem(ESCRAVO1, frame2, dataLength2, crc2);
}

// FRAME 3 - ESCRAVO 2
void EnviarFrame3(){
  uint8_t frame3[] = {0x4f, 0x6c, 0xe1, 0x20, 0x45, 0x73, 0x63, 0x72, 0x61, 0x76, 0x6f, 0x32};
  size_t dataLength3 = sizeof(frame3);
  uint16_t crc3 = calculoCRC16(frame3, dataLength3);
  EnviarMensagem(ESCRAVO2, frame3, dataLength3, crc3);
}

// FRAME 4 - ESCRAVO 2
void EnviarFrame4(){
  uint8_t frame4[] = {0x43, 0x6f, 0x6d, 0x6f, 0x20, 0x76, 0x61, 0x69, 0x3f};
  size_t dataLength4 = sizeof(frame4);
  uint16_t crc4 = calculoCRC16(frame4, dataLength4);
  EnviarMensagem(ESCRAVO2, frame4, dataLength4, crc4);
}

void setup() {
  Serial.begin(9600);
  
}

void loop() {
  switch(state){
    case FRAME1:
      EnviarFrame1();
      Serial.println();
      delay(5000);
      state = CHECK1;
      break;
      
    case CHECK1:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                  Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame1");
                      state = FRAME1;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = FRAME2;
                    break;
                  }             
              }        
        }
      }
      delay(50);
      break;
      
    case FRAME2:
      EnviarFrame2();
      Serial.println();
      delay(5000);
      state = CHECK2;
      break;
      
    case CHECK2:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                  Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame1");
                      state = FRAME2;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = FRAME1;
                    break;
                  }             
              }        
        }
      }
      delay(50);
      break;
      
    case FRAME3:
      EnviarFrame3();
      Serial.println();
      delay(5000);
      state = CHECK3;
      break;
      
    case CHECK3:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                  Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame1");
                      state = FRAME3;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = FRAME4;
                    break;
                  }             
              }        
        }
      }
      delay(50);
      break;

    case FRAME4:
      EnviarFrame4();
      Serial.println();
      delay(5000);
      state = CHECK4;
      break;
      
    case CHECK4:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                 Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame1");
                      state = FRAME4;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = FRAME1;
                    break;
                  }             
              }        
        }
      }
      delay(50);
      break;
  }
}
