#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define instance for LCD I2C display 
LiquidCrystal_I2C lcd(0x27,16,2);

//#define para definir os pins digitais que se conectam a um led
#define LED_VERDE A2  // Pin digital correspondente ao led verde
//Os restantes pins conectam-se a um led vermelho
#define LED1 2
#define LED2 3
#define LED3 4
#define LED_CENTRAL 5 //Led central = led vermelho do meio 
#define LED5 6
#define LED6 7
#define LED7 8
//pin corrrespondente ao buzzer
#define BUZZER 13

//#define para definir os pins digitais que se conectam ao stepper motor
#define IN1 11
#define IN2 10
#define IN3 9
#define IN4 12
int Direction = 0;
int Steps = 0;
int distanciaAntiga = 0; 
int apitar = 0;
boolean portaoEmMovimento = true;
int numero_passos_atual = 4096;


//Vamos usar o método mills() para determar o intervalo de tempo entre determinadas ações, uma vez que delay()
//pausa o programa, não nos deixando fazer outras coisas simultâneamente.
//inspirado no site: https://docs.arduino.cc/built-in-examples/digital/BlinkWithoutDelay
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 10000;  //the value is a number of milliseconds

unsigned long startMillis1;  //some global variables available anywhere in the program
unsigned long currentMillis1;
unsigned long period1 = 100;  //the value is a number of milliseconds

//SR01 digital pins
#define trigPin A1 // Trigger Pin
#define echoPin A0 // Echo Pin

int maximumRange = 200; // Maximum range needed. 200 centrimetros = 2 metros
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

void setup()
{ 
    Serial.begin(9600);
    lcd.begin();                      // initialize the lcd 
    lcd.backlight();
    pinMode(BUZZER,OUTPUT);

    //fazer loop a todos os números dos pins digitais dos leds vermelhos
    for(int i =2;i<=8;i++){ 
        pinMode(i, OUTPUT);
    }

    pinMode(LED_VERDE, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    startMillis = millis();  //initial start time
}
void loop()
{
    Serial.println(Steps);
    //Código que é preciso para o SR01 medir a distância entre este e um objeto que esteja à sua frente
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);

    distanciaAntiga = distance;
    distance = duration/58.2;

    
    //imprimir no lcd a distância ?
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Distancia :");
    lcd.setCursor(0,10);
    lcd.print(distance);
    

    //Se o valor da distância, determinado no loop anterior, for igual ao valor da distância determinado no loop atual
    //e também se já passaram 10 segunodos desde que o carro entrou em repouso, o portão da garagem fechar-se-á.
    // Mas como o sensor pode dar ás vezes valores instáveis e acrescentar mais um ou menos um centímetro apesar de
    // a distância, na realidade, não ter mudado, ignoramos se a diferença entre as duas distância é de menos 1 ou mais 1
    if(distance == distanciaAntiga + 1 || distance == distanciaAntiga || distance == distanciaAntiga - 1){
        currentMillis = millis();
        if(currentMillis - startMillis >= period){
            portaoEmMovimento = true;
            fecharPortao();
        }
    }else{ // se o carro estiver em movimento, o programa reinicia a contagem dos 10 segundos
        startMillis = millis();
        portaoEmMovimento = false;
        //Se o carro voltou a entrar em movimento, a porta da garagem voltará a abrir-se
        //Direction = 0;
        //stepper(Direction);
    }

    if (distance >= 100 && distance <= maximumRange){//O carro encontra-se entre os 100 e 200 cm
        acenderUmaLuzVermelha();
    }
    else if(distance >= 40 && distance < 100){//O carro encontra-se entre os 40 e 100 cm
        acenderTresLuzesVermelhas();
    }
    else if(distance >= 30 && distance < 40){//O carro encontra-se entre os 40 e 30 cm
        acenderCincoLuzesVermelhas();
    }
    else if(distance <30){//O carro encontra-se entre os 30 e 20 cm
        acenderTodasLuzesVermelhas();
    }
    else{//Se o carro estiver "fora do range"
        acenderLuzVerde();
    }

    //Delay 80ms antes da próxima leitura
    delay(150); 
}

void acenderUmaLuzVermelha(){
    if(!portaoEmMovimento){ //Se o portão não estiver a fechar
        for(int i = 2; i<=8;i++){ //o programa deixa todas as luzes desligadas menos o led_central = led vermelho do meio 
            if (i==LED_CENTRAL){
            digitalWrite(LED_CENTRAL,HIGH);
            continue;
            }
            digitalWrite(i, LOW);
        }
        digitalWrite(LED_VERDE,LOW); //Led verde mantêm-se apagado
        noTone(BUZZER); // O buzzer não toca
    }else{  //Se o portão estiver a fechar
        noTone(BUZZER); // O buzzer não toca também
    }
}

void acenderTresLuzesVermelhas(){
    int frequencia_tone = 850;
    period1 = 200; // = 0.2 segundos de intervalo

    if(!portaoEmMovimento){ //Se o portão não estiver a fechar
        for(int i = 2; i<=8;i++){
            if (i==LED5 || i==LED3 || i==LED_CENTRAL){  // todos os leds desligam-se menos estes três
                digitalWrite(i,HIGH);
                continue;
            }
            digitalWrite(i, LOW);
        }
        digitalWrite(LED_VERDE,LOW);//Led verde mantêm-se apagado
        usarBuzzer(frequencia_tone, period1);
    }else{ //Se o portão estiver a fechar
        noTone(BUZZER); // O buzzer não toca 
    }

}

void acenderCincoLuzesVermelhas(){
    int frequencia_tone = 850;
    period1 = 100;

    if(!portaoEmMovimento){
        digitalWrite(LED2,HIGH);
        digitalWrite(LED3,HIGH);
        digitalWrite(LED_CENTRAL,HIGH);
        digitalWrite(LED5,HIGH);
        digitalWrite(LED6,HIGH);
        digitalWrite(LED1,LOW);
        digitalWrite(LED7,LOW);
        digitalWrite(LED_VERDE,LOW);
        usarBuzzer(frequencia_tone, period1);
    }else{
        noTone(BUZZER);
    }

}

void usarBuzzer(int frequencia_tone, long period1){
    if(!apitar){ // Se o buzzer ainda não começou a apitar
        startMillis1 = millis(); // Começa a contagem do tempo dos 0 até 0.2 segundos
        apitar = 1; //da proxima vez que o programa chamar o void loop(), vai considerar que o buzzer já está a emitir som
    }
    currentMillis1 = millis(); // relembrar: millis() devolve os milissegundos que passaram desde que inicializamos o arduino
    if(apitar == 1 && currentMillis1 - startMillis1 >= period1){ // se o buzzer está a apitar e já passaram 0.2 segundos
        noTone(BUZZER); //o buzzer para de tocar
        startMillis1 = millis(); // a contagem do tempo reinicia 
        apitar = 2; //da proxima vez que o programa fizer loop, vai considerar que o buzzer foi pausado
    }else if (apitar == 2 && currentMillis1 - startMillis1 >= period1){ // se o buzzer foi pausado e já passaram novamente 0.2 segundos
        apitar = 0; //recomeçar o loop: apitar = 0, apitar = 1, apitar = 2, apitar = 0, ...
    }else{
        tone(BUZZER, frequencia_tone); // Sempre que as condições, mencionadas acima, são falsas, o buzzer apita
    }
}

void acenderTodasLuzesVermelhas(){
    int frequencia_tone = 850;

    if(!portaoEmMovimento){
        digitalWrite(LED2,HIGH);
        digitalWrite(LED3,HIGH);
        digitalWrite(LED_CENTRAL,HIGH);
        digitalWrite(LED5,HIGH);
        digitalWrite(LED6,HIGH);
        digitalWrite(LED1,HIGH);
        digitalWrite(LED7,HIGH);
        digitalWrite(LED_VERDE,LOW);
        tone(BUZZER, frequencia_tone); 
    }else{
        noTone(BUZZER);
    }
}


void acenderLuzVerde(){
  if(!portaoEmMovimento){
    digitalWrite(LED_VERDE,HIGH);
    for(int i=2; i<=8;i++){
      digitalWrite(i,LOW);
    }
  }
}

void fecharPortao(){
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    digitalWrite(LED_CENTRAL,LOW);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,LOW);
    digitalWrite(LED1,HIGH);
    digitalWrite(LED7,HIGH);
    digitalWrite(LED_VERDE,LOW);
    Direction = 1;
    stepper(Direction);

}

void stepper(int Diretion){
  int x = 0;
  while (x <50){
    x++;
    if (Direction){ //se a direção = 1, então a porta vai se fechar
         //se a porta ainda não está totalmente fechada, vamos continuar a dencerementar os steps
         if(numero_passos_atual <= 0){
            return;
         }else{
            Steps--;
            numero_passos_atual--;
            if(Steps<0){Steps=7;}
         }
        
    }else{ //se a direção = 0, então a porta vai se abrir
         //se a porta ainda não está totalmente aberta, vamos continuar a incerementar os steps
         if(numero_passos_atual >= 4096){
          return;
         }else{
            Steps++;
            numero_passos_atual++;
            if(Steps>7){Steps=0;}
        }
    }
    
    // Não usamos o for loop porque queremos que dois leds vermelhos estejam a piscar ao mesmo tempo que a garagem se feche
    // ou que o led verde esteja a piscar ao mesmo tempo que a garagem se abre
    
    switch(Steps){
        case 0:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
        case 1:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        break;
        case 2:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
        case 3:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
        case 4:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
        case 5:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
        case 6:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
        case 7:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
        default:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }
    // O programa precisa de delay para funcionar corretamente
    delayMicroseconds(1000);
    }
}
