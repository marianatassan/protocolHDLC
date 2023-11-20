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
  PERGUNTA1,
  PERGUNTA2,
  SLAVE1,
  SLAVE2,
  COMUNICA,
  COMUNICA2,
  CHECK1,
  CHECK2,
  CHECK3,
  CHECK4,
  CHECK5,
  CHECK6,
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

// FRAME 5 - PERGUNTA ESCRAVO 1
void EnviarFrame5(){
  uint8_t frame5[] = {0x45,0x73,0x63,0x72,0x61,0x76,0x6F,0x20,0x32};
  size_t dataLength5 = sizeof(frame5);
  uint16_t crc5 = calculoCRC16(frame5, dataLength5);
  EnviarMensagem(ESCRAVO1, frame5, dataLength5, crc5);
}

// FRAME 6 - SOLICITA MENSAGEM S1
void EnviarFrame6(){
      uint8_t frame6[] = {0x4D,0x61,0x6E,0x64,0x61,0x20,0x61,0x20,0x6D,0x65,0x6E,0x73,0x61,0x67,0x65,0x6D};
      size_t dataLength6 = sizeof(frame6);
      uint16_t crc6 = calculoCRC16(frame6, dataLength6);
      EnviarMensagem(ESCRAVO1, frame6, dataLength6, crc6);
}

// FRAME 7 - PERGUNTA ESCRAVO 2
void EnviarFrame7(){
  uint8_t frame7[] = {0x45,0x73,0x63,0x72,0x61,0x76,0x6F,0x20,0x31};
  size_t dataLength7 = sizeof(frame7);
  uint16_t crc7 = calculoCRC16(frame7, dataLength7);
  EnviarMensagem(ESCRAVO2, frame7, dataLength7, crc7);
}

// FRAME 8 - SOLICITA MENSAGEM S2
void EnviarFrame8(){
      uint8_t frame8[] = {0x4D,0x61,0x6E,0x64,0x61,0x20,0x61,0x20,0x6D,0x65,0x6E,0x73,0x61,0x67,0x65,0x6D};
      size_t dataLength8 = sizeof(frame8);
      uint16_t crc8 = calculoCRC16(frame8, dataLength8);
      EnviarMensagem(ESCRAVO2, frame8, dataLength8, crc8);
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
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                  //Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = FRAME1;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    //Serial.println("Debug-elseif");
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
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                 // Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = FRAME2;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    //Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = FRAME3;
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
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                  //Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = FRAME3;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    //Serial.println("Debug-elseif");
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
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                 //Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = FRAME4;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "Mensagem válida"){
                    //Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = PERGUNTA1;
                    break;
                  }             
              }        
        }
      }
      delay(50);
      break;

    case PERGUNTA1:
      EnviarFrame5();
      Serial.println();
      delay(5000);
      state = CHECK5;
      break;
      
    case CHECK5:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                 //Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = PERGUNTA1;
                      delay(4000);
                      break;
                  }
                  else if (mensagem == "sim"){
                    //Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = COMUNICA;
                    break;
                  }
                  else if (mensagem == "nao"){
                    //Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = PERGUNTA2;
                    break;
                  }              
              }        
        }
      }
      delay(50);
      break;
      
    case COMUNICA:
      EnviarFrame6();
      Serial.println();
      delay(5000);
      state = SLAVE1;
      break;
      
    case SLAVE1:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                 //Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = COMUNICA;
                      delay(4000);
                      break;
                  }
                  else {
                     const char* bytes = mensagem.c_str();
                     size_t dataLength9 = strlen(bytes);
                     uint16_t crc9 = calculoCRC16(reinterpret_cast<const uint8_t*>(bytes), dataLength9);
                     EnviarMensagem(ESCRAVO2, bytes, dataLength9, crc9);
                     delay(2000);
                     Serial.println();
                     state = PERGUNTA2;
                     break;
                  }           
              }        
        }
      }
      delay(50);
      break;
      
    case PERGUNTA2:
      EnviarFrame7();
      Serial.println();
      delay(5000);
      state = CHECK6;
      break;;
      
    case CHECK6:
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
                 String mensagem1 = Serial.readStringUntil(FLAG);
                 Serial.println(mensagem1);
                 if (mensagem1 == "Erro CRC") {
                      Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = PERGUNTA2;
                      delay(4000);
                      break;
                  }
                  else if (mensagem1 == "nao"){
                    Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = FRAME1;
                    break;
                  }
                  else{
                    Serial.println("Debug-elseif");
                    Serial.println("Resposta recebida com sucesso.");
                    delay(2500);
                    state = COMUNICA2;
                    break;
                  }              
              }        
        }
      }
      delay(50);
      break;
      
    case COMUNICA2:
      EnviarFrame8();
      Serial.println();
      delay(5000);
      state = SLAVE2;
      break;
      
    case SLAVE2:
      // Aguarda resposta
      if (Serial.available() > 0) {
          char flag = Serial.read();
          //Serial.println("Debug-serial");
          
          // Detectar início do frame
          if (flag == FLAG) {
            //Serial.println("Debug-FLAG");
              int endereco = Serial.parseInt();
              if (endereco == MESTRE) {
                 //Serial.println("Debug-endereco");
                  String mensagem = Serial.readStringUntil(FLAG);
                  if (mensagem == "Erro CRC") {
                      //Serial.println("Debug-if");
                      Serial.println("Erro CRC detectado. Tentando novamente...");
                      Serial.println("Reenviando Frame");
                      state = COMUNICA2;
                      delay(4000);
                      break;
                  }
                  else {
                     const char* bytes = mensagem.c_str();
                     size_t dataLength10 = strlen(bytes);
                     uint16_t crc10 = calculoCRC16(reinterpret_cast<const uint8_t*>(bytes), dataLength10);
                     EnviarMensagem(ESCRAVO1, bytes, dataLength10, crc10);
                     delay(2000);
                     Serial.println();
                     state = FRAME1 ;
                     break;
                  }           
              }        
        }
      }
      delay(50);
      break;
  }
}
