#include <Arduino.h>
#include <TM1637Display.h>

// Hall Sensor Settigs
#define P_HALL 2
#define P_COOLING_IN 5
#define P_COOLING_OUT 6

//#define MUESTREO 100  //ms

// Display Settings
#define D_CLK_PIN 3
#define D_DIO_PIN 4
//#define D_REFRESH 1000 //ms
#define D_BRIGHTNESS 3
#define D_COLON 0b01000000
//#define FACTOR 1000 // Distancia / Radio sale de t=(d/r)*Periodo

const uint8_t SEG_INF[] = {
	SEG_G,          // -
	SEG_G,          // -
	SEG_G,          // -
	SEG_G           // -
	};

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};
const uint8_t SEG_OFF[] = {
	0,           // 
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_A | SEG_E | SEG_F | SEG_G,                   // F
	SEG_A | SEG_E | SEG_F | SEG_G                    // F
	};

const uint8_t SEG_BYE[] = {
	0,           // 
	SEG_C | SEG_D | SEG_E | SEG_F | SEG_G ,                 // b
	SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,                  // Y
	SEG_A | SEG_E | SEG_F | SEG_D | SEG_G                   // E
	};


TM1637Display display(D_CLK_PIN, D_DIO_PIN);


unsigned long oldtime=0;        
unsigned long time=0;
unsigned long CookTime=0;

unsigned long previous = 2^16;



unsigned long timeProm = 0;

#define FACTOR_ADJ 3.5 //5.4          //FACTOR DE MULTIPLICACION, MAS ALTO SUBE LA ESCALA DE TIEMPO

void setup()
{
    //Serial.begin(9600);
    display.setBrightness(D_BRIGHTNESS,true);
    display.setSegments(SEG_INF);

    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(P_COOLING_IN, INPUT_PULLUP);
    pinMode(P_COOLING_OUT, OUTPUT);
    digitalWrite(P_COOLING_OUT, HIGH);

    digitalWrite(P_HALL,HIGH);
    attachInterrupt(digitalPinToInterrupt(P_HALL),isr,RISING);
}

unsigned numRev = 0;   
unsigned long NextRefresh = 0;
unsigned long prevTime = 0; 
unsigned long avgTime = 0;
unsigned long prevDATA[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long idxDATA = 0;
unsigned long numData = 0;
#define PERSISTANCE 16
#define TIMEOUT_REVS 30

unsigned loopCount = 0;
void loop()
{
    unsigned long timeSum;
    unsigned long adjTime;
    unsigned long newTime;
    unsigned i;
    if (loopCount > 1000 && digitalRead(P_COOLING_IN)==HIGH){
        
        if (digitalRead(P_COOLING_IN)==HIGH) CoolingProcedure();
    }
    //if (numRev >= revNextStop) {
    if (millis() >= NextRefresh) {
        detachInterrupt(digitalPinToInterrupt(P_HALL));
        NextRefresh=millis()+1200;
        newTime = millis();
        adjTime = ((newTime - prevTime)) * FACTOR_ADJ / numRev;
        
        prevDATA[idxDATA] = adjTime;
        idxDATA = (idxDATA + 1) % PERSISTANCE;
        if (numData < PERSISTANCE) numData++;
                
        // ↓ HARDCODEADO YA QUE EL FOR NO FUNCIONA :(  ↓
        
        timeSum = prevDATA[0]  + prevDATA[1]  + prevDATA[2]  + prevDATA[3] +
                  prevDATA[4]  + prevDATA[5]  + prevDATA[6]  + prevDATA[7] +
                  prevDATA[8]  + prevDATA[9]  + prevDATA[10] + prevDATA[11] +
                  prevDATA[12] + prevDATA[13] + prevDATA[14] + prevDATA[15];/* +
                  prevDATA[16] + prevDATA[17] + prevDATA[18] + prevDATA[19] +
                  prevDATA[20] + prevDATA[21] + prevDATA[22] + prevDATA[23] +
                  prevDATA[24] + prevDATA[25] + prevDATA[26] + prevDATA[27] +
                  prevDATA[28] + prevDATA[29] + prevDATA[30] + prevDATA[31];*/

        
        timeDisplay(timeSum/numData);

        //RE ATTACH
        numRev=0;
        prevTime=millis();
        attachInterrupt(digitalPinToInterrupt(P_HALL),isr,RISING);
    }
    loopCount++;
}

#define SECONDS_TO_COOL 180UL

void CoolingProcedure(){//Rutina de descuento de tiempo de enfriamiento
    
    unsigned long endMS=0, initMS=0, currentMS=0;
    uint8_t segment=1;
    detachInterrupt(digitalPinToInterrupt(P_HALL));
    initMS = millis();
    endMS = initMS + SECONDS_TO_COOL * 1000;
    currentMS = initMS;
    while (currentMS <= endMS && digitalRead(P_COOLING_IN)==HIGH ){
        segment = (segment << 1);//
        if (segment > 0b00100000) segment = 1;
        display.showNumberDec(((endMS-currentMS)/1000) ,false,4,0);
        display.setSegments(&segment,1,0);
        delay(150);
        currentMS = millis();
    }
    bool TimeOut=true;
    while (TimeOut){
        if (digitalRead(P_COOLING_IN)==HIGH){
            display.setSegments(SEG_BYE);
            digitalWrite(P_COOLING_OUT,LOW);

        } else{
                digitalWrite(P_COOLING_OUT,HIGH);
                numRev=0;
                prevTime=millis();
                attachInterrupt(digitalPinToInterrupt(P_HALL),isr,RISING);
                TimeOut = false;
        }
    }
}

void isr(){//Rutina de interrupcion para conteo de eventos
    numRev++;
}

void timeDisplay(unsigned long time){//Muestra el tiempo en formato MM:SS
    if (time < 3600){
        if (time > 600)
            time -= time % 10;
        else
            time -= time % 5;
        int min = time / 60;
        int sec = time % 60;
        display.showNumberDecEx(min * 100 + sec, D_COLON ,false, 4, 0);
    }
    else {
        display.setSegments(SEG_INF);
    }
}