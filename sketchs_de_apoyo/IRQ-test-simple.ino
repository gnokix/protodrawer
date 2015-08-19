#define LED 9
#define SW1 2

int contador = 0;
int rebote = 0;
byte state = LOW;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(SW1, INPUT);
  Serial.begin(9600); 
  attachInterrupt(0, toggle, RISING);
}

void loop() { 
  
  if (rebote < 10000) { 
    rebote++ ;
    }

}

void toggle() { 

  if (rebote > 9998) {
   state = !state; 
   digitalWrite(LED, state);
   
   Serial.print("Contador = ");
   Serial.println(contador);

   rebote=0;
   contador++;
   
   } else {
    Serial.print("antirebote activado: ");
    Serial.println(rebote);
    Serial.println("");
   }
  
}
