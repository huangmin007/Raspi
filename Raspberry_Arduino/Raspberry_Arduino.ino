
void setup()
{
  Serial.begin(9600);
  while(!Serial)
    ; 
  Serial.println("ready.");
}

void loop()
{
  if(Serial.available() > 0)
  {
    Serial.print(Serial.read());
  }
  
  if(millis() % 1000 == 0)
  {
    Serial.println("A");
    delay(1);
  }
  
}
