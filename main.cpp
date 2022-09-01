#include <LiquidCrystal.h>
#include <Servo.h>
LiquidCrystal lcd(8, 11, 9, 7, 3, 2); //define lcd gpio numbers
Servo myservo;  // create servo object to control a servo

//define gpio numbers:
const int PIR = 5;
const int servo = 10;
const int red_light = 13;
const int blue_light = 12;
const int green_light = 4;
const int temp_sensor = A0;
const int start_switch = A1;
const double min_temp = -40 ;
const int buzzerPin = 6;
int dayStart = 0; //The day is not start yet (day start = 1)
int dayEnd = 0; //The day is not over yet (day end = 1)
char numBatch = 0;
int vaccine= 2;
int blink= 0;
int INT_numBatch;
int empty_batch=0;
int batch_half_empty[3]={0,0,0};//Half empty batches
int batch_full[3]={0,0,0};//Full batches
int start_switch_val;
int patient= 0;
int inputNum=0;
int inputNum6=0;
int patientsInClinic; //tne number of patient in the clinic
int updatepatient; //when the nurs update the number patient that got vaccine
int counterUntilUpdate=0; //count the patient until nurs update (5 input)
int flagtemp=0;
int temp_cycle=0;
int firstServoAction=0;

void setup()
{
	//pin input and output defenition
  pinMode(red_light, OUTPUT);
  pinMode(blue_light,OUTPUT);
  pinMode(green_light,OUTPUT);
  pinMode(temp_sensor,INPUT);
  pinMode(PIR,INPUT);
  pinMode(start_switch,INPUT);
  //serial port and lcd port defenition
  Serial.begin(9600);
  lcd.begin(16, 2);
}//end void setup()

void loop()//main loop algorithem
{
	if(digitalRead(PIR)==HIGH)
    PIR_alert();
start_switch_val = readStartSwitch();
double temp = readTemp();
boolean sec6 = true;
  
if(start_switch_val == 1 && temp < min_temp && dayStart==0)
 {
  	lcd.clear();      
  	dayStart=1;
    dayEnd=0;
   	Serial.println("START DAY");
    patientsInClinic= 0;//init the clinic to be empty
    readBatchSerial();
  
  while( dayStart == 1 && readStartSwitch() != 0)
  {
    temp = readTemp();

    if(digitalRead(PIR)==HIGH)
    	PIR_alert();
    
    if(temp > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
      	HighTemp();
        
    if (Serial.available() > 0) 
    	inputNum=Serial.read();//the input of the different cases
      
    if (inputNum=='0'){ //print Inventory
         printInventory();
         inputNum=NULL;
      }//end if            			

    if (inputNum=='4'){ //patient has arrived to th clinic
         patientArrived();
         inputNum=NULL;
      }//end if
    if (inputNum=='5')
      { //the nurs whants to updated the number of patient who got vaccine
       	updateNumpatient();
       	inputNum=NULL;
      }//end if
    
    if (empty_batch > (batch_full[0]+batch_full[1]+batch_full[2]+batch_half_empty[0]+batch_half_empty[1]+batch_half_empty[2]))
    {
      if (firstServoAction==0)
      {
        myservo.attach(servo);
        firstServoAction=1;
      }
      recycleBatches();
    }
    if(batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0)
    { //if stock isnt empty
    digitalWrite(green_light, HIGH); // green light is working while The vaccine stock isnt empty
    }
    else
    {
    digitalWrite(green_light, LOW); // green light off while The vaccine stock is empt
    }
   
    if(batch_half_empty[1]==0 && batch_half_empty[2]==0 && batch_full[1]==0 && batch_full[2]==0)
    {//if stock is empty
      digitalWrite(green_light, LOW);
      digitalWrite(red_light, HIGH); // red light is working while The vaccine stock isnt empty
      lcd.print("No vaccines in                          stock           ");     
    }
    else
    {
    lcd.clear();
    }
   }//end while
}//end if      
  
  if(start_switch_val == 0 && dayStart==1)
  {
    digitalWrite(red_light, LOW);
    digitalWrite(green_light, LOW);
    digitalWrite(blue_light, LOW);
    lcd.clear();
    lcd.print("Clinic is close                                         ");
    if (Serial.available() > 0) 
      inputNum=Serial.read();//the input of the different cases
      
      if (inputNum=='0')
      { //print Inventory
           printInventory();
            inputNum=NULL;
      }//end if    
    
    if(digitalRead(PIR)==HIGH)
   		PIR_alert();   
        dayStart=0;
        dayEnd=1;
    	numBatch=0;
    	downLevel();
     	Serial.println("DAY END.");
    	Serial.print("Number of patient who got vaccine today is:");
    	Serial.println(patientsInClinic);
    	Serial.print("Total number of patient who got vaccine is:");
    	Serial.println(updatepatient+counterUntilUpdate);
    	printInventory();
  }//end if 
  
    if(digitalRead(PIR)==HIGH)
    	PIR_alert();
  
    if (Serial.available() > 0) 
      	inputNum=Serial.read();//the input of the different cases
      
      if (inputNum=='0')
      { //print Inventory
           printInventory();
           inputNum=NULL;
      }//end if  
}//end void loop()

//-----------------------------------------functions-------------------------------------------------------------------


int readStartSwitch()
{
    if(analogRead(start_switch)==1023)
    return 1;
    if(analogRead(start_switch)==1013)
    return 0;
  return 0;  
}


double readTemp()
{
    double temp = analogRead(temp_sensor);
    return 100*(((temp*5)/1024)-0.5);
}


void printInventory()
{
     Serial.print("The number of expired doses: ");
     Serial.println(batch_full[0]*2+batch_half_empty[0]);
     Serial.print("The number of doses expires in 1: ");
     Serial.println(batch_full[1]*2+batch_half_empty[1]);
     Serial.print("The number of doses expires in 2: ");
     Serial.println(batch_full[2]*2+batch_half_empty[2]);
  	 Serial.println();	
}


void blinkGreenLed(int INT_numBatch)
{ 	
  lcd.print("Number of                               batches: ");
  lcd.print(INT_numBatch);
  for(blink = 0; blink < INT_numBatch; blink++) 
	{
		digitalWrite(green_light, HIGH); // set the led on
		delay(500); // wait for a second
		digitalWrite(green_light, LOW); // turn the LED off
		delay(500); // wait for a second
    
        if(digitalRead(PIR)==HIGH)
    	PIR_alert();
    
		if(readTemp() > min_temp)
		HighTemp();   
    
	}//end for
	digitalWrite(green_light, LOW);
}

void readBatchSerial()
{
   		Serial.println("Please insert number of batch: ");
        while(numBatch == 0)
		{
            if(digitalRead(PIR)==HIGH)
    		PIR_alert();
			
          	if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
			HighTemp();
          
        	if (Serial.available() > 0) 
        	{    
   				numBatch = Serial.read();
    			if ((numBatch >= '1') && (numBatch <= '9'))
                {
      				Serial.println("Number received: ");
        			Serial.println(numBatch);
      				INT_numBatch = numBatch-'0';
        			batch_full[2]= batch_full[2]+INT_numBatch;
                  	blinkGreenLed(INT_numBatch);
    			}//end if
              	
              	if (numBatch == '0')
                {
                  printInventory();
                  numBatch=0;
                }
        	}//end if (Serial.available() > 0)           
		}//end while
}

void patientArrived()
{
  if(patientsInClinic < 8)//If there is a place for a new patient
  {
    if(Givevaccine()==true)//If could vaccinate
    {
   		patientsInClinic = patientsInClinic +1 ;
   		counterUntilUpdate=counterUntilUpdate+1 ;
   		Serial.println("number of patient:");
   		Serial.println(patientsInClinic);
   		vaccineAction();
    }//end if
    else//There were no doses of vaccine availability
    {
        Serial.println("There is no available vaccine in the clinic");
    }//end else
  }//end if
  
  else//There is no place for the patient
  {
      Serial.println("There is no place in the clinic");
  }//end else
}

boolean Givevaccine() //True if the vaccine was performed successfully, else False
{
  if (batch_half_empty[1]>0)//vaccine dose is valid for one day
  {
    batch_half_empty[1]=batch_half_empty[1]-1;
    empty_batch++;
  }
  else
  {
    if (batch_full[1]>0)//Batch valid for one day
    {
      batch_full[1]=batch_full[1]-1; 
      batch_half_empty[1]=batch_half_empty[1]+1;
    }
    else
    {
      if (batch_half_empty[2]>0)//vaccine dose is valid for two days
      {
        batch_half_empty[2]=batch_half_empty[2]-1;
        empty_batch++;
      }
      else
      {
        if (batch_full[2]>0)//Batch valid for two days
        {
          batch_full[2]=batch_full[2]-1;
          batch_half_empty[2]=batch_half_empty[2]+1;
        }
        else
        {
          return false;//The nurse did not vaccinate the patient
        }
      }
    }
  }
  return true;//The nurse vaccinated the patient
}
                     
void vaccineAction() //function for the green light who works while patient gets dose of vaccine
{
      for(int BlinkvaccineAction = 0; BlinkvaccineAction < 5; BlinkvaccineAction++) 
	  {
        if(digitalRead(PIR)==HIGH)
    	PIR_alert();
        
        if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
		HighTemp();
        
		digitalWrite(green_light, HIGH); // set the led on
		delay(500); // wait for 0.5 second
        
        if(digitalRead(PIR)==HIGH)
    	PIR_alert();
        
        if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
		HighTemp();
        
        
		digitalWrite(green_light, LOW); // turn the LED off
		delay(500); // wait for 0.5 second
	}//end for
	digitalWrite(green_light, LOW);
}

void downLevel()
{
  //The validity of the vaccine decreases
  batch_half_empty[0]=batch_half_empty[0]+batch_half_empty[1];
  batch_half_empty[1]=batch_half_empty[2];
  batch_half_empty[2]=0;
  
  batch_full[0]=batch_full[0]+batch_full[1];
  batch_full[1]=batch_full[2];
  batch_full[2]=0;
}

void updateNumpatient () // when the nurs want to updates the number of vaccine that have been made today
{
  int previousUpdate=updatepatient; //the previous update of patient 
  updatepatient=updatepatient+counterUntilUpdate;//sums the counter that not count yet in the updatepatient
  counterUntilUpdate=0; //init the counter
     
  Serial.println("The previous number of patient who got vaccine is:");
  Serial.println(previousUpdate);
  Serial.println("The updated number of patient who got vaccine is:");
  Serial.println(updatepatient);
  
  lcd.clear();
  lcd.print("Previous number                         of Vaccinated:");
  delay(1000);
  lcd.clear();
  
        if(digitalRead(PIR)==HIGH)
    	PIR_alert();
        
        if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
		HighTemp();
  
   lcd.println(previousUpdate);
   delay(1000);
   lcd.clear();
  
        if(digitalRead(PIR)==HIGH)
    	PIR_alert();
        
        if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
		HighTemp();
  
   lcd.print("Updated number                          of Vaccinated:");
   delay(1000);
   lcd.clear();
  
        if(digitalRead(PIR)==HIGH)
    	PIR_alert();
        
        if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0)) // Temp alert Only if there are batches in stock
		HighTemp();
  
   lcd.println(updatepatient);
   delay(1000);
   lcd.clear();
}

void BlinkblueLed() //function for the green light who works while patient gets dose of vaccine
{

		digitalWrite(blue_light, HIGH); // set the led on
		delay(500); // wait for a 500 mili-second

		digitalWrite(blue_light, LOW); // turn the LED off
		delay(500); // wait for a 500 mili-second
		digitalWrite(blue_light, LOW);
  
}

void PIR_alert()
{
  int dualCycle=1;
  digitalWrite(buzzerPin,HIGH);
  lcd.clear();
  lcd.print ("Stealing alert");
  Serial.println("Enter passcode:");
  inputNum =NULL;
  while(inputNum !='7')
	{      
        	if (Serial.available() > 0)   
   				inputNum = Serial.read();
    
        	if(readTemp() > min_temp)//Temperature alarm and stealing together
        	{
              digitalWrite(green_light, LOW);
              digitalWrite(blue_light, LOW);
              digitalWrite(red_light, LOW);
              
                while(inputNum !='8')
				{                 
                if (Serial.available() > 0)   
   				inputNum = Serial.read();
                
                if (inputNum == '0')// Inventory update
                {
				printInventory();
                inputNum =NULL;
                }
                  
              	if (dualCycle==1) //red light blink   
              	digitalWrite(red_light, HIGH);
                  
              	if (dualCycle==2)    
              	digitalWrite(red_light, LOW);
                 
                if (dualCycle==3)  //blue light blink
              	digitalWrite(blue_light, HIGH);

                if (dualCycle==4)  
              	digitalWrite(blue_light, LOW);
                  
                if (dualCycle==5)//green light blink
              	digitalWrite(green_light, HIGH);
                
                if (dualCycle==6)
              	digitalWrite(green_light, LOW);
                  
                  delay(500);
                  
                  dualCycle++;
                  
                  if (dualCycle==7)
                    dualCycle=1;
                }
              digitalWrite(green_light, LOW);
              digitalWrite(blue_light, LOW);
              digitalWrite(red_light, LOW);
              inputNum =NULL;
              
              HighTempWO();
              inputNum =3;
        	}
    
    		if (inputNum == '0')//Inventory update
			printInventory();
    
			if(inputNum != NULL && inputNum !='7')//Wrong code
            {
              	lcd.clear();
              	Serial.println("Re-Enter passcode:");
				wrong_code();
    			inputNum =NULL;
            }
    }//while
  
digitalWrite(buzzerPin,LOW); 
lcd.clear();
lcd.print ("Alert canceled");
delay(2000);
lcd.clear();
}

void wrong_code()
{
  int randBatch;
  int batchExpired=0;
  int batchExIn1=0;
  int batchExIn2=0;
  
  for (int i = 0; i < 2; i++)
  {
  if ((batch_full[0]+batch_full[1]+batch_full[2]) !=0)// There are batches in stock
  {
  
  randBatch= random(0, (batch_full[0]+batch_full[1]+batch_full[2]));
  
  if(batch_full[0] !=0 && randBatch < batch_full[0])//batch_full[0] stole
     {
	 batch_full[0]=batch_full[0]-1;
     batchExpired=batchExpired+1;
     }    
     if(batch_full[1] !=0 && randBatch >= batch_full[0] && randBatch < (batch_full[0] + batch_full[1]))//batch_full[1] stole
     {
	 batch_full[1]=batch_full[1]-1;
     batchExIn1=batchExIn1+1;
     }    
     if(batch_full[2] !=0 && randBatch >= (batch_full[0] + batch_full[1]) && randBatch < (batch_full[0] + batch_full[1]+batch_full[2]))//batch_full[2] stole
     {
	 batch_full[2]=batch_full[2]-1;
     batchExIn2=batchExIn2+1;
     }
  }
  }
  
  if(batchExpired==2)
  {
    lcd.print ("2 expired batch-                        es were stolen");
    delay(2000);
	lcd.clear();
  }
  
   if(batchExIn1==2)
   {
    lcd.print ("2 batches expire                        in 1 day stolen");
    delay(2000);
	lcd.clear();
   }
  
   if(batchExIn2==2)
   {
    lcd.print ("2 batches expire                        in 2 day stolen");
    delay(2000);
	lcd.clear();
   }
  
   if(batchExpired==0 && batchExIn1==0 && batchExIn2==0)
   {
     lcd.print ("No batches were                         stolen");
     delay(2000);
	lcd.clear();
   }
  else if(batchExpired!=2 && batchExIn1!=2 && batchExIn2!=2)
  {
    if (batchExpired==1)
    {
    lcd.print ("1 expired batch                         were stolen");
    delay(2000);
	lcd.clear();
    lcd.print ("&");  
    delay(1000);
	lcd.clear();
    }
    
    if (batchExIn1==1)
    {
    lcd.print ("1 batch expire                          in 1 day stolen");
    delay(2000);
	lcd.clear();
      if (batchExIn2==1)
      {
    	lcd.print ("&");  
    	delay(1000);
		lcd.clear();
      }
    }
    
    if (batchExIn2==1)
    {
    lcd.print ("1 batch expire                          in 2 day stolen");
    delay(2000);
	lcd.clear();
    } 
  }//else if closer
}//void function closer

void HighTemp()
{
  {
    int dualCycle=1;
      Serial.println("The temp is high");
      downLevel();//the validity get down in a day
      temp_cycle=0;
      digitalWrite(green_light,LOW);
      inputNum=NULL;
      while(inputNum !='6')
		{        
        	if(digitalRead(PIR)==HIGH)//Temperature alarm and stealing together
        	{
              digitalWrite(green_light, LOW);
              digitalWrite(blue_light, LOW);
              digitalWrite(red_light, LOW);
              inputNum=NULL;
              
                while(inputNum !='8')
				{                  
                if (Serial.available() > 0)   
   				inputNum = Serial.read();
                
                if (inputNum == '0')
                {
                  printInventory();
                  inputNum =NULL;
                }                  
              	if (dualCycle==1)// red light blink   
              	digitalWrite(red_light, HIGH);
                  
              	if (dualCycle==2)    
              	digitalWrite(red_light, LOW);
                 
                if (dualCycle==3)// blue light blink     
              	digitalWrite(blue_light, HIGH);

                if (dualCycle==4)  
              	digitalWrite(blue_light, LOW);
                  
                if (dualCycle==5)// green light blink   
              	digitalWrite(green_light, HIGH);
                
                if (dualCycle==6)
              	digitalWrite(green_light, LOW);
                  
                  delay(500);
                  
                  dualCycle++;
                  
                  if (dualCycle==7)
                    dualCycle=1;
                }
              digitalWrite(green_light, LOW);
              digitalWrite(blue_light, LOW);
              digitalWrite(red_light, LOW);
              inputNum =NULL;
              
              PIR_alertWO();
        	}
      
        	if (Serial.available() > 0) //Inventory update
        	{    
   				inputNum = Serial.read();
    			if (inputNum == '0')
                {
				printInventory();
    			}//end if
        	}//end if (Serial.available() > 0)
        if(temp_cycle<7)
        {
          digitalWrite(buzzerPin,HIGH);
        }
        else
        {
          digitalWrite(buzzerPin,LOW);
        }
        	BlinkblueLed();// Duration 1 secend
        temp_cycle = temp_cycle + 1;
		}//end while
      digitalWrite(buzzerPin,LOW);
    }//end if
    inputNum=NULL;
}

void HighTempWO()
{
  {
    int dualCycle=1;
      Serial.println("The temp is high");
      downLevel();//the validity get down in a day
      temp_cycle=0;
      digitalWrite(green_light,LOW);
      inputNum=NULL;
      while(inputNum !='6')
		{
  
        	if (Serial.available() > 0) // Inventory update
        	{    
   				inputNum = Serial.read();
    			if (inputNum == '0')
                {
				printInventory();
    			}//end if
        	}//end if (Serial.available() > 0)
        if(temp_cycle<7)
        {
          digitalWrite(buzzerPin,HIGH);
        }
        else
        {
          digitalWrite(buzzerPin,LOW);
        }
        	BlinkblueLed();// Duration 1 secend
        temp_cycle = temp_cycle + 1;
		}//end while
      digitalWrite(buzzerPin,LOW);
    }//end if
    inputNum=NULL;
}

void PIR_alertWO()
{
  int dualCycle=1;
  digitalWrite(buzzerPin,HIGH);
  lcd.clear();
  lcd.print ("Stealing alert");
  Serial.println("Enter passcode:");
  inputNum =NULL;
  while(inputNum !='7')
	{  
        	if (Serial.available() > 0) //Inventory update  
   				inputNum = Serial.read();
    
    		if (inputNum == '0')
			printInventory();
    
			if(inputNum != NULL && inputNum !='7')
            {
              	lcd.clear();
              	Serial.println("Re-Enter passcode:");
				wrong_code();
    			inputNum =NULL;
            }
    }//while
  
digitalWrite(buzzerPin,LOW); 
lcd.clear();
lcd.print ("Alert canceled");
delay(2000);
lcd.clear();
}

void recycleBatches()
{
lcd.clear();
lcd.print ("Recycle procces                         Packing batches");
for (int i=0;i<5;i++)
{
  	if(digitalRead(PIR)==HIGH)
    PIR_alert();
    
    if(readTemp() > min_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0)) //Temp alert Only if there are batches in stock
	HighTemp();
    
    if (Serial.available() > 0) //Inventory update 
    {    
   	inputNum = Serial.read();
    	if (inputNum == '0')
        {
			printInventory();
    	}//end if
    }//end if (Serial.available() > 0)
    
  
  int angle = 0;
  int servoClock = 0;
  while (angle <= 180 && servoClock < 50) //servo rotation for 6 sec 
  {
    myservo.write(angle);
    delay(10);
    angle++;
    servoClock++;   
  } 
  while (angle <= 180 && servoClock < 100) 
  {
    myservo.write(angle);      
    delay(10);
    angle--;
    servoClock++;
  }
}
empty_batch=0;  
lcd.clear();  
}