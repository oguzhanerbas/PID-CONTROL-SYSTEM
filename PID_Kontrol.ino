//KÜTÜPHANELER
#include <PID_v1.h> 
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

double yVal;
double xVal;

double xValback;
double yValback;
int notouch;

double startX = 82;     // BAŞLANGIÇ AÇISAL OLARAK BAŞLANGIÇ NOKTALARI TANIMLANDI
double startY = 75;
const byte Panel_Pin[] = {2, 3, 4, 5};
const int sense = A0;

int xVal_Nrf, yVal_Nrf;
char cstr1[10];
char cstr2[10];

const byte Adres[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN); 

// PID DEGERLERİ
double SetpointX, InputX, OutputX; //for X
double SetpointY, InputY, OutputY; //for Y

// SERVO DEGERLERİ
Servo servoX; //X EKSEN
Servo servoY; //Y EKSEN

/////TIME SAMPLE
int Ts = 15;
//PID const
float KpX = 0.25; //PID x ekseni değerleri
float KiX = 0.001;
float KdX = 0.04;

float KpY = 0.25; //PID y ekseni değerleri
float KiY = 0.001;
float KdY = 0.04;

double Xoutput, Youtput;

PID myPIDX(&InputX, &OutputX, &SetpointX, KpX, KiX, KdX, DIRECT);
PID myPIDY(&InputY, &OutputY, &SetpointY, KpY, KiY, KdY, DIRECT);

void setup() {

  Serial.begin(9600);
  
  for (int i = 0; i < 4; i++) {   // PANEL PİNLERİNİN TANIMLANMASI
    pinMode(Panel_Pin[i], OUTPUT);
  }

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5);
  radio.openWritingPipe(Adres);

  SetpointX = 0;                    // TOPUN DOKUNMATİK EKRANDA DENGELEMEYE ÇALIŞTIĞI YER
  SetpointY = 0;

  servoX.attach(6);                  // SERVO MOTORLARIN BAGLANDIGI PİNLER
  servoY.attach(7);

  servoX.write(startX);                    //SERVOLARIN BASLANGIC ACILARI
  servoY.write(startY);

  myPIDX.SetMode(AUTOMATIC);             //PIDX KONTROLÜMÜZÜ AÇIYORUZ.
  myPIDX.SetOutputLimits(-45, 45);       //PIDX HATA ARALIĞI
  myPIDY.SetMode(AUTOMATIC);             //PIDY KONTROLÜMÜZÜ AÇIYORUZ.
  myPIDY.SetOutputLimits(-55, 60);       //PIDY HATA ARALIĞI

  // TIME SAMPLE
  myPIDX.SetSampleTime(Ts);
  myPIDY.SetSampleTime(Ts);
  
  delay(10);
}

void loop() {

  xValback=xVal;
  yValback=yVal;
  delay(10);

  // REZİSTİF PANELDEN GELEN DEĞERLERİ OKUMAK İÇİN PİNLERE VERİLEN DEGERLER
  
  digitalWrite(Panel_Pin[0], HIGH);  
  digitalWrite(Panel_Pin[1], LOW);
  digitalWrite(Panel_Pin[2], LOW);
  digitalWrite(Panel_Pin[3], HIGH);

  delay(10);

  digitalWrite(Panel_Pin[0], HIGH);
  digitalWrite(Panel_Pin[1], LOW);
  digitalWrite(Panel_Pin[2], LOW);
  digitalWrite(Panel_Pin[3], HIGH);
  xVal = analogRead(sense); //  X KOORDİNATLARINI OKU
  xVal_Nrf = analogRead(sense);
  xVal = map(xVal, 290, 730, -160, 160); // X KOORDİNATLARININ PANELE GÖRE MİLİMETRE CİNSİNDEN BOYUTLANDIRILMASI
  xVal_Nrf = map(xVal_Nrf, 290, 730, -198, 198);
  delay(10);
  
  digitalWrite(Panel_Pin[0], LOW);
  digitalWrite(Panel_Pin[1], LOW);
  digitalWrite(Panel_Pin[2], HIGH);
  digitalWrite(Panel_Pin[3], HIGH);

  delay(10);
  InputX = xVal; // PIDX'E GİDEN X KOORDİNATLARI

  digitalWrite(Panel_Pin[0], LOW);
  digitalWrite(Panel_Pin[1], LOW);
  digitalWrite(Panel_Pin[2], HIGH);
  digitalWrite(Panel_Pin[3], HIGH);

  yVal_Nrf = analogRead(sense);
  yVal = analogRead(sense); // Y KOORDİNATLARINI OKU
  yVal_Nrf = map(yVal_Nrf, 290, 730, -161, 161);
  yVal = map(yVal, 290, 730, -160, 160); // Y KOORDİNATLARININ PANELE GÖRE MİLİMETRE CİNSİNDEN BOYUTLANDIRILMASI

  InputY = yVal;  // PIDY'E GİDEN Y KOORDİNATLARI

  myPIDX.Compute();  // PIDX HESAPLAMALARI
  myPIDY.Compute();  // PIDY HESAPLAMALARI

  Xoutput =  startX + OutputX;   //TOPUN DENGEYE GELMESİ İÇİN GEREKEN X AÇI DEGERLERİ 
  Youtput =  startY + OutputY;   //TOPUN DENGEYE GELMESİ İÇİN GEREKEN Y AÇI DEGERLERİ 

 itoa(xVal_Nrf, cstr1, 10);   //HABERLEŞME İLE GÖNDERİLEN X,Y KOORDİNATLARININ CHAR DİZİSİ OLARAK BİRLEŞTİRİLMESİ
 itoa(yVal_Nrf, cstr2, 10);
 strcat(cstr1 , ",");
 strcat(cstr1 , cstr2);

radio.write( &cstr1, sizeof(cstr1) );  

// TOP PLATFOMUN ÜSTÜNDE DEGİLSE SERVO MOTORLARI BAŞLANGICA GETİREN KISIM

if( (xValback<=xVal+10 && xValback>=xVal-10) && (yValback<=yVal+10 && yValback>=yVal-10) )
{
notouch=notouch+1;
}
else
{
notouch=0;
  servoX.attach(6);
  servoY.attach(7);

}

if(notouch>=20)
{
if( (xVal<=SetpointX+50 && xVal>=SetpointX-50) && (yVal<=SetpointY+50 && yVal>=SetpointY-50) )
{
notouch=0;
}

else{

    servoX.write(startX);
    servoY.write(startY);

    delay(100);
    servoX.detach();
    servoY.detach();
    notouch=0;
    
}
 
}

   servoX.write(Xoutput);                        
   servoY.write(Youtput);  

}
