


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//>>>  arquivos complementares do projeto : https://www.dropbox.com/sh/7kwu0zb2edtqu37/AADPDuygyTI_VljF7Dvz36FQa?dl=0
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>



// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal.h>                                          //Biblioteca para o display disp
#include <Wire.h>
#include <SoftwareSerial.h>                                         //bibioteca para bluetooth   
#include "Dimmer.h"                                                 //biblioteca para dimmer
SoftwareSerial bluetooth(0,1);     //0 = rx 1=tx                    //define os pino do bluetooth
Dimmer lamp(3);                                                     //define os pino do dimmer
LiquidCrystal disp(9, 8, 7, 6, 5, 4);                               // define os pinos do display(12, 11,5,4,3,2)
int LABEL = 1;
int linha = 0; 

int led1 = 11;                                                       //pino do led local
int led2 = 12;                                                       //pino do led remoto
int led3 = 13;                                                       //pino do led automatico

const int ldr = A1;                                                 // pino do ldr

const int sensorTensao = A3 ;                                       //pino do sensor de tensao
float valordaporta;                                                 //--_
float valorMedia ;                                                  //   --  variaveis para calculo de tensao   
float valordetensao;                                                //__-


const int pinoSensor = A2 ;                                          // pino do sensor de corrente    
int sensorValue_aux = 0;                                             //--_
float valorSensor = 0;                                               //   --variaveis para calculo de corrente 
float valorCorrente = 0;                                             //__-
float voltsporUnidade = 0.004887586;                                 // 5/1023
float sensibilidade = 0.066;                                         //  Para ACS712 de 30 Amperes use 0.066
 
const int pot = A0;                                                  //pino do potenciometro
char enc = 10;                                                       //variavel com valor de encremento da lampada 
char valor ;                                                         // variavel auxiliar
int brilho;                                                          // variavel que imprime valor                                                                
float potencia;                                                      // valor que de potencia v.i
int flag = 0;   

 /////////////////////////////////// ///////////////////
unsigned long tempo = 0 ;                                         //   --__
long intervalo = 3000 ;                                           //        --   variaveis para calculo de tempo 
boolean flagclear ;                                               //   __--


const int botao =   10;                                          //pino dp Botão para selecionar tela 
                                   

// >>>>>>>>  Funções Auxiliares <<<<<<<<<<<
void selMenu();                                                     //Função para selecionar o tipo de controle 
void dispMenu();                                                    //Função para mostrar o tipo de controle
void telainicial();                                                 // tela inicial
void local();                                                    //   --__ 
void remoto();                                                   //       -- tipos de controle
void automatico();                                               //   __--

char menu = 1;                                                     //Variável para selecionar o menu                          
boolean t_butUp;                                                   //Flags para armazenar o estado do botão


void setup()
{
      pinMode(pinoSensor, INPUT);       //define sensor de corrente como entrada
      pinMode(sensorTensao, INPUT);     //define sensor de tensao como entrada
      pinMode(pot, INPUT);              //define  potenciometro como entrada
      pinMode(ldr, INPUT);              //define  ldr como entrada
      pinMode(botao, INPUT);            //define botao como entrada
      pinMode(led1, OUTPUT);            //   --__
      pinMode(led2, OUTPUT);            //        -- define os led's como saida
      pinMode(led3, OUTPUT);            //   __--
      bluetooth.begin(9600);            //inicia bluetooth
      lamp.begin();                     // inicia controle  da carga.
      disp.begin(16,2);                 //Inicializa LCD 16 x 2  
       t_butUp   = 0x00;                //limpa flag do botão Up

                               
}

void loop()
{
   selMenu();
   dispMenu();
} 

void selMenu()                                       
{
   if(!digitalRead(botao))   t_butUp   = 0x01;           //Botão Up pressionado? Seta flag  
   
    if(digitalRead(botao) && t_butUp)                    //Botão Up solto e flag setada?
    {                                                    //Sim...
      t_butUp = 0x00;                                    //Limpa flag
      disp.clear();                                      //Limpa display
      menu++;                                            //Incrementa menu
      if(menu > 0x04) menu = 0x01;                       //Se menu maior que 4, volta a ser 1
    } 
  
}
void dispMenu() 
{    
  switch(menu)                                        //Controle da variável menu
    {
       case 1:                                       //Caso 1
             telainicial();                               //Chama a função de relógio
             
             break;                                     //break
       case 2:                                       //Caso 2
             local();                             //Chama a função do termômetro
         
             break;                                     //break
       case 3:                                       //Caso 3
             remoto();                                  //Chama função para acionamento de lâmpadas

             break;                                     //break
       case 4:                                       //Caso 4
             automatico();                                   //Chama função para o menu4
            
             break;   
     
             
        
    
    } 

} 

void telainicial()                                       
{
   disp.setCursor(0,0);                                 //Posiciona cursor na coluna 1, linha 1
   disp.print("Opcao de ");                             //Imprime mensagem
   disp.setCursor(0,1);                                 //Posiciona cursor na coluna 1, linha 2
   disp.print("Controle");   
   
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);

   
   
} 

void local(){                                    

  //>>>>>>>>>>>>>>>>>>>>>>  comando do Potenciometro  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   
 int valorPot = map(analogRead(pot),0,1023,0,100);
 lamp.set(valorPot);
 brilho = valorPot;
  //>>>>>>>>>>>>>>>>>>>>>>>>>>sensor de corrente <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 
  for(int i=10000; i>0; i--)
  {
    sensorValue_aux = (analogRead(pinoSensor) -510);
     valorSensor += pow(sensorValue_aux,2);
    
  }
                                                                
      valorSensor = (sqrt(valorSensor/ 10000)) * voltsporUnidade;     // finaliza o calculo da média e ajusta o valor lido para volts
       valorCorrente = (valorSensor/sensibilidade);                      // calcula a corrente considerando a sensibilidade do sernsor (185 mV por amper)
 
        if(valorCorrente <= 0.095)
        {
           valorCorrente = 0;
        }
 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> valor de tensao >>>>>>>>>>>>>>>>>>>

 valordaporta = 0;
 valorMedia = 0;
 for(int i=0;i<1000;i++)
  {
    valordaporta = valordaporta + analogRead(sensorTensao); 
  }  
  valorMedia = valordaporta / 1000;
  
  valordetensao= valorMedia * 2.685267449;      //43.01   2.603765721
     
        potencia = (valordetensao*valorCorrente);

 unsigned long tempoAtual = millis() ;

 if (tempoAtual-tempo >= intervalo ){
  tempo = tempoAtual;
    flagclear = !flagclear;  
}
 
    if (flagclear == 0){
     disp.clear();                   /// limpar o lcd
      disp.setCursor(0, 0);           // posicionar o cusor na coluna 0, linha 0
       disp.print("tensao: ");
        disp.setCursor(7, 0);  
         disp.print(valordetensao);
          disp.print(" V" );
           disp.setCursor(0, 1); 
            disp.print("Corrente: ");
             disp.setCursor(10, 1);  
              disp.print(valorCorrente);
               disp.print(" A" );
}
       if (flagclear == 1){
       disp.clear();                   /// limpar o lcd
        disp.setCursor(0, 0);      // posicionar o cusor na coluna 0, linha 0
         disp.print("BRILHO: ");  // imprime o valor de brilho em %     
          disp.setCursor(8, 0);  
           disp.print(brilho);
            disp.print(" %" );
        disp.setCursor(0, 1);           // posicionar o cusor na coluna 0, linha 0
         disp.print("Pot: ");
          disp.setCursor(5, 1);  
           disp.print(potencia);
            disp.print(" W" );
}
  digitalWrite(led1,HIGH);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
} // fim do local



 
void remoto()   // remoto                                      
{
    
   //>>>>>>>>>>>>>>>>>>>>>>  comando do bluetooth  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  linha++;
   char x = bluetooth.read();    // variavel que rece o valor do bluetooth
     brilho = valor ;
  if (x == '-'){                   // se a variavel for = A
   valor = valor - (enc) ;        // valor será = o valor instantaneo + 10 
       if(valor <= 0) valor = 0 ;
         lamp.set(valor);             // ligar lampada na potencia calculada 
            bluetooth.print ("Brilho: ");
            bluetooth.print (brilho - (enc) );
            bluetooth.println ("%");
  }
  if (x == '+'){                               
     valor = valor + (enc) ;                // valor será = o valor instantaneo - 10
      if (valor >= 100) valor = 100 ;
      lamp.set(valor);                        //// ligar lampada na potencia calculada 
       bluetooth.print ("Brilho: ");
       bluetooth.print (brilho + (enc) );
       bluetooth.println ("%");
  }
  
  if (x == '0'){ 
    valor = 0;                              
      lamp.set(valor);                        //// desligar lampada
      bluetooth.println ("desligado");
  }

 if (x == '1'){   
      valor = 100;                        
      lamp.set(valor);                        //// ligar lampada
       bluetooth.println ("Ligado");
     
  }
  if (x == '2'){

 
    
      bluetooth.println (">>>>>>>>>>>>>>>>>>>>");                               
      bluetooth.print ("Tensao: ");
      bluetooth.print (valordetensao );
      bluetooth.println ("V");
      bluetooth.print ("Corrente: ");
      bluetooth.print (valorCorrente );
      bluetooth.println ("A");
      bluetooth.print ("Potencia: ");
      bluetooth.print (potencia );
      bluetooth.println ("W");
 
  }

   

  //>>>>>>>>>>>>>>>>>>>>>>>>>>sensor de corrente <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 
  for(int i=10000; i>0; i--){
    // le o sensor na pino analogico A1 e ajusta o valor lido ja que a saída do sensor é (1023)vcc/2 para corrente =0
    sensorValue_aux = (analogRead(pinoSensor) -510);
    // somam os quadrados das leituras.
     valorSensor += pow(sensorValue_aux,2);
    //delay(1);
  }
 
                                                                   // finaliza o calculo da média quadratica e ajusta o valor lido para volts
  valorSensor = (sqrt(valorSensor/ 10000)) * voltsporUnidade;
   valorCorrente = (valorSensor/sensibilidade);// calcula a corrente considerando a sensibilidade do sernsor (185 mV por amper)
 
    if(valorCorrente <= 0.095){
      valorCorrente = 0;
  }
 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> valor de tensao >>>>>>>>>>>>>>>>>>>

 
 valordaporta = 0;
  valorMedia = 0;
 for(int i=0;i<1000;i++)
  {
    valordaporta = valordaporta + analogRead(sensorTensao); 
  }  
  valorMedia = valordaporta / 1000;
  
  valordetensao= valorMedia * 2.683481987;     //43.01   2.644410489
      
      
      
      
      
        potencia = (valordetensao*valorCorrente);

 unsigned long tempoAtual = millis() ;

 if (tempoAtual-tempo >= intervalo ){
  tempo = tempoAtual;
  flagclear = !flagclear;  
}
 
    if (flagclear == 0){
     disp.clear();                   /// limpar o lcd
      disp.setCursor(0, 0);           // posicionar o cusor na coluna 0, linha 0
       disp.print("tensao: ");
        disp.setCursor(7, 0);  
         disp.print(valordetensao);
          disp.print(" V" );
          disp.setCursor(0, 1); 
           disp.print("Corrente: ");
           disp.setCursor(10, 1);  
             disp.print(valorCorrente);
               disp.print(" A" );
}
       if (flagclear == 1){
         disp.clear();                   /// limpar o lcd
        disp.setCursor(0, 0);      // posicionar o cusor na coluna 0, linha 0
         disp.print("BRILHO: ");  // imprime o valor de brilho em %     
          disp.setCursor(8, 0);  
           disp.print(brilho);
            disp.print(" %" );
        disp.setCursor(0, 1);           // posicionar o cusor na coluna 0, linha 0
         disp.print("Pot: ");
         disp.setCursor(5, 1);  
           disp.print(potencia);
            disp.print(" W" );


         

}


  digitalWrite(led1,LOW);
  digitalWrite(led2,HIGH);
  digitalWrite(led3,LOW);
    
    


} //fim do remoto


void automatico()                                          
{
  
  //>>>>>>>>>>>>>>>>>>>>>>  comando do Potenciometro  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 int valorLdr = map(analogRead(ldr),0,1023,0,100);
 if (valorLdr <=20)
     valorLdr = 0;
 if (valorLdr >=90)
    valorLdr = 100;
   lamp.set(valorLdr);
   brilho =valorLdr; 



  //>>>>>>>>>>>>>>>>>>>>>>>>>>sensor de corrente <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 
  for(int i=10000; i>0; i--){
    // le o sensor na pino analogico A1 e ajusta o valor lido ja que a saída do sensor é (1023)vcc/2 para corrente =0
    sensorValue_aux = (analogRead(pinoSensor) -510);
    // somam os quadrados das leituras.
    valorSensor += pow(sensorValue_aux,2);
  
  }
 // finaliza o calculo da média quadratica e ajusta o valor lido para volts
  valorSensor = (sqrt(valorSensor/ 10000)) * voltsporUnidade;
   valorCorrente = (valorSensor/sensibilidade);// calcula a corrente considerando a sensibilidade do sernsor (185 mV por amper)
 
    if(valorCorrente <= 0.095){
      valorCorrente = 0;
  }
 //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> valor de tensao >>>>>>>>>>>>>>>>>>>

 
 valordaporta = 0;
  valorMedia = 0;
 for(int i=0;i<1000;i++)
  {
    valordaporta = valordaporta + analogRead(sensorTensao); 
  }  
  valorMedia = valordaporta / 1000;
  
  valordetensao= valorMedia * 2.574834991;//43.01
      
      
        potencia = (valordetensao*valorCorrente);

 unsigned long tempoAtual = millis() ;

 if (tempoAtual-tempo >= intervalo ){
  tempo = tempoAtual;
    flagclear = !flagclear;  
}
 
    if (flagclear == 0){
     disp.clear();                   /// limpar o lcd
      disp.setCursor(0, 0);           // posicionar o cusor na coluna 0, linha 0
       disp.print("tensao: ");
        disp.setCursor(7, 0);  
         disp.print(valordetensao);
          disp.print(" V" );
          disp.setCursor(0, 1); 
           disp.print("Corrente: ");
            disp.setCursor(10, 1);  
             disp.print(valorCorrente);
               disp.print(" A" );
}
       if (flagclear == 1){
         disp.clear();                   /// limpar o lcd
        disp.setCursor(0, 0);      // posicionar o cusor na coluna 0, linha 0
         disp.print("BRILHO: ");  // imprime o valor de brilho em %     
          disp.setCursor(8, 0);  
           disp.print(brilho);
            disp.print(" %" );
        disp.setCursor(0, 1);           // posicionar o cusor na coluna 0, linha 0
         disp.print("Pot: ");
         disp.setCursor(5, 1);  
           disp.print(potencia);
            disp.print(" W" );
}


  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,HIGH);


} // fim do menu 

