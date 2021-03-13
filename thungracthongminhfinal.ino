#include <Arduino.h>
#include <Servo.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
// defines pins numbers
const int trigPin1 = 26;
const int echoPin1 = 27;
const int trigPin2 = 23;
const int echoPin2 = 25;
// defines variables
long duration1;
int distance1;
long duration2;
int distance2;
int count;
float PoT;
Servo myservo;  
int pos = 0;  
char buffer [100];
//String payLoad = "{\"trashId\": 1,\"model\":\"T1\",\"percent\":" + """ + PoT + "}";
void setup()
{ 
   Serial.begin(115200); 
   delay(4000);
 
  Serial.println("Connected to the WiFi network");
   myservo.attach(14);  
   pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output+
   pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
   pinMode(trigPin2, OUTPUT); 
   pinMode(echoPin2, INPUT);
}
  
 
void loop()
{
  // Clears the trigPin
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  // duration for sound wave to the user
  duration1 = pulseIn(echoPin1, HIGH);

  // Calculating the distance with the user
  distance1 = duration1*0.0345/2;
  delay(500);
  // Prints the distance on the Serial Monitor
  Serial.print("Distance to the user: ");  
  Serial.println(distance1);
  // duration for sound wave from the trash to the second ultrasonic inside the trash bin
  duration2 = pulseIn(echoPin2,HIGH);
  // Calculating the height of the trash inside the trashbin
  distance2 = 24 - duration2*0.0345/2; 
  PoT = distance2*100/24; 
  // Prints the height of the trash
  Serial.print("Height of the trash: ");  
  Serial.println(distance2);
  //Make the jsonPayload
  
  if ( distance2 >= 18)
  {
    Serial.print("The trash bin is full");
  }
  // servo + ultrasonic
 if (20 < distance1 && distance1 < 40)
  { 
    for (pos = 0; pos <= 180; pos += 1) 
    {
      // in steps of 1 degree
      myservo.write(pos);             
      delay(10);    
    }
    delay(10000);
    for (pos = 180; pos >= 0; pos -= 1) 
    { 
      myservo.write(pos);              
      delay(10);                       
    }
    count++;
    Serial.print(" number of time(s) the trash was used: ");
    Serial.println(count);
  }








    int cx;

  cx = snprintf ( buffer, 100, "{\"trashId\": 1,\"model\":\"T1\",\"percent\": %d", distance2);

  if (cx>=0 && cx<100)      // check returned value

    snprintf ( buffer+cx, 100-cx, "}" );

  puts (buffer);
  //Send the PoT to the server
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
  
   HTTPClient http;   
  
   http.begin("https://williamiotcloud.azurewebsites.net/api/Trash/UploadStatus");  //Specify destination for HTTP request
   http.addHeader("Content-Type", "application/json");             //Specify content-type header
  
   int httpResponseCode = http.POST(buffer);   //Send the actual POST request
  
   if(httpResponseCode>0){
  
    String response = http.getString();                       //Get the response to the request
  
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
  
   }else{
  
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  
   }
  
   http.end();  //Free resources
  
 }else{
  
    Serial.println("Error in WiFi connection");   
  
 }
  
}
