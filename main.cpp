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
const double max_temp = -40 ;
const int buzzerPin = 6;
int dayStart = 0; //0 = The day did not started yet (day start = 1)
int dayEnd = 0; //0 = The day is not over yet (day end = 1)
char num_batch = 0;
int vaccine= 2;
int blink= 0;
int num_batch_int;
int empty_batch=0;
int batch_half_empty[3]={0,0,0};//Half empty batches
int batch_full[3]={0,0,0};//Full batches
int start_switch_val;
int patient= 0;
int input_serial_num=0;
int input_serial_num6=0;
int patients_in_clinic; // number of patient in the clinic
int update_patients; //when the מurse update the number of patients that got vaccine
int counter_until_update=0; //count the patients until the nurse update (input 5)
int flagtemp=0;
int temp_cycles=0;
int first_servo_action=0;

//Function Declarations
int ReadStartSwitch();
double ReadTemp();
void PrintInventory();
void PrintBlinkBatchsNum(int num_batch_int);
void ReciveBatchsReadSerial();
void PatientArrived();
boolean GiveVaccine();
void VaccineAction();
void ValidityDecreas();
void PatientsNumUpdate();
void BlinkBlueLed();
void PIRAlert();
void WrongCode();
void HighTemp();
void HighTempDuringRobbery();
void PIRAlertDuringHighTemp();
void RecycleBatches();

void setup()
{//pin input and output defenition
  pinMode(red_light, OUTPUT);
  pinMode(blue_light,OUTPUT);
  pinMode(green_light,OUTPUT);
  pinMode(temp_sensor,INPUT);
  pinMode(PIR,INPUT);
  pinMode(start_switch,INPUT);
  //serial port and lcd port defenition
  Serial.begin(9600);
  lcd.begin(16, 2);
}//end void setup

void loop()//main loop algorithem
{
	if(digitalRead(PIR)==HIGH)
		PIRAlert();//robbery alarm starts
	start_switch_val = ReadStartSwitch();
	double temp = ReadTemp();
	boolean sec6 = true;
  
	if(start_switch_val == 1 && temp < max_temp && dayStart==0)
	 {//The day began
		lcd.clear();      
		dayStart=1;// 1 = the day started
		dayEnd=0;// 0 = the day is not over yet
		Serial.println("START DAY");
		patients_in_clinic= 0;//initializing the clinic to be empty
		ReciveBatchsReadSerial();
	  
	  while( dayStart == 1 && ReadStartSwitch() != 0)
	  {// checks during the working time
		temp = ReadTemp();// temp read
		if(digitalRead(PIR)==HIGH)// alarm sensor check
			PIRAlert();
		if(temp > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))
			HighTemp();// the temp is above the max allowed temp	
		if (Serial.available() > 0) //input serial read
			input_serial_num=Serial.read();
		//actions acording to the input value:  
		if (input_serial_num=='0'){ //print inventory
			 PrintInventory();
			 input_serial_num=NULL;
		  }//end if            			
		if (input_serial_num=='4'){ //a patient arrived to th clinic
			 PatientArrived();
			 input_serial_num=NULL;
		  }//end if
		if (input_serial_num=='5')
		  { //the nurs whants to updat the number of patients who were vaccinated
			PatientsNumUpdate();
			input_serial_num=NULL;
		  }//end if
		if (empty_batch > (batch_full[0]+batch_full[1]+batch_full[2]+batch_half_empty[0]+batch_half_empty[1]+batch_half_empty[2]))
		{//recycle empty batches action
		  if (first_servo_action==0)
		  {
			myservo.attach(servo);
			first_servo_action=1;
		  }
		  RecycleBatches();
		}
		if(batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0)
		{ //if stock isnt empty
			digitalWrite(green_light, HIGH); // green light is working while The vaccine stock isnt empty
		}
		else
		{
			digitalWrite(green_light, LOW); // green light off while The vaccine stock is empty
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
  {// closing the clinic routine
    digitalWrite(red_light, LOW);
    digitalWrite(green_light, LOW);
    digitalWrite(blue_light, LOW);
    lcd.clear();
    lcd.print("Clinic is close                                         ");
    if (Serial.available() > 0) 
      input_serial_num=Serial.read();//the input of the different cases
      
		if (input_serial_num=='0')
		{ //print inventory
           PrintInventory();
           input_serial_num=NULL;
		}//end if    
    
    if(digitalRead(PIR)==HIGH)//robbery alarm starts
   		PIRAlert();
	//initialzing varibels after the day is over
	dayStart=0;
	dayEnd=1;
	num_batch=0;
	ValidityDecreas();
	Serial.println("DAY END.");
	Serial.print("Number of patient who got vaccine today is:");
	Serial.println(patients_in_clinic);
	Serial.print("Total number of patient who got vaccine is:");
	Serial.println(update_patients+counter_until_update);
	PrintInventory();
  }//end if 
  
	if(digitalRead(PIR)==HIGH)//robbery alarm starts
		PIRAlert();

	if (Serial.available() > 0) 
		input_serial_num=Serial.read();//the input of the different cases
	  
		if (input_serial_num=='0')
		{ //print Inventory
		   PrintInventory();
		   input_serial_num=NULL;
		}//end if  
}//end void loop
//---------------------------------------------------------------------------------------------------
//--------------------------------------------***********--------------------------------------------
//--------------------------------------------*functions*--------------------------------------------
//--------------------------------------------***********--------------------------------------------
//---------------------------------------------------------------------------------------------------
//read the switch possition
int ReadStartSwitch()
{
    if(analogRead(start_switch)==1023)
    return 1;
    if(analogRead(start_switch)==1013)
    return 0;
  return 0;  
}
//read temp value
double ReadTemp()
{
    double temp = analogRead(temp_sensor);
    return 100*(((temp*5)/1024)-0.5);
}
//print inventory te serial port
void PrintInventory()
{
     Serial.print("The number of expired doses: ");
     Serial.println(batch_full[0]*2+batch_half_empty[0]);
     Serial.print("The number of doses expires in 1: ");
     Serial.println(batch_full[1]*2+batch_half_empty[1]);
     Serial.print("The number of doses expires in 2: ");
     Serial.println(batch_full[2]*2+batch_half_empty[2]);
  	 Serial.println();	
}
//print number of batchs to the lcd screen & blink green led light
void PrintBlinkBatchsNum(int num_batch_int)
{ 	
  lcd.print("Number of                               batches: ");
  lcd.print(num_batch_int);
  for(blink = 0; blink < num_batch_int; blink++) 
	{
		digitalWrite(green_light, HIGH); // set led on
		delay(500); // wait for a second
		digitalWrite(green_light, LOW); // turn LED off
		delay(500); // wait for a second
    
        if(digitalRead(PIR)==HIGH)//robbery alarm starts
    	PIRAlert();
    
		if(ReadTemp() > max_temp)//temp check
		HighTemp();   
    
	}//end for
	digitalWrite(green_light, LOW);//green LED off
}
//receiving new batchs routine
void ReciveBatchsReadSerial()
{
   		Serial.println("Please insert number of batch: ");
        while(num_batch == 0)
		{
            if(digitalRead(PIR)==HIGH)//robbery alarm starts
    		PIRAlert();
			
          	if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))//temp check
			HighTemp();
          
        	if (Serial.available() > 0) 
        	{    
   				num_batch = Serial.read();
    			if ((num_batch >= '1') && (num_batch <= '9'))
                {
      				Serial.println("Number received: ");
        			Serial.println(num_batch);
      				num_batch_int = num_batch-'0';
        			batch_full[2]= batch_full[2]+num_batch_int;
                  	PrintBlinkBatchsNum(num_batch_int);
    			}//end if
              	
              	if (num_batch == '0')
                {
                  PrintInventory();
                  num_batch=0;
                }
        	}//end if (Serial.available > 0)           
		}//end while
}
//when patient arrived
void PatientArrived()
{
  if(patients_in_clinic < 8)//If there is a place for a new patient
  {
    if(GiveVaccine()==true)//If could vaccinate
    {
   		patients_in_clinic = patients_in_clinic +1 ;
   		counter_until_update=counter_until_update+1 ;
   		Serial.println("number of patient:");
   		Serial.println(patients_in_clinic);
   		VaccineAction();
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
//True value if the vaccine was performed successfully, else False
boolean GiveVaccine()
{
  if (batch_half_empty[1]>0)//vaccine dose valid for one day
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
//flash the green light while patient get is getting vaccinated
void VaccineAction()
{
      for(int blink_VaccineAction = 0; blink_VaccineAction < 5; blink_VaccineAction++) 
	  {
        if(digitalRead(PIR)==HIGH)//robbery alarm starts
    	PIRAlert();
        
        if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))//temp check
		HighTemp();
        
		digitalWrite(green_light, HIGH); //set the led on
		delay(500); //wait for 500 milisec = 0.5 second
        
        if(digitalRead(PIR)==HIGH)//robbery alarm starts
    	PIRAlert();
        
        if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))//temp check
		HighTemp();
		digitalWrite(green_light, LOW); // turn the LED off
		delay(500); // wait for 0.5 second
	}//end for
	digitalWrite(green_light, LOW);
}
//decreases the validity of the vaccines. 1 day less. 
void ValidityDecreas()
{
  batch_half_empty[0]=batch_half_empty[0]+batch_half_empty[1];
  batch_half_empty[1]=batch_half_empty[2];
  batch_half_empty[2]=0;
  
  batch_full[0]=batch_full[0]+batch_full[1];
  batch_full[1]=batch_full[2];
  batch_full[2]=0;
}
//updates the number of vaccinations that have been made today. for nurse use.
void PatientsNumUpdate()
{
  int patients_previous_update=update_patients; //the previous update of patient 
  update_patients=update_patients+counter_until_update;//sums the counter that not count yet in the update_patients
  counter_until_update=0; //init the counter
     
  Serial.println("The previous number of patient who got vaccine is:");
  Serial.println(patients_previous_update);
  Serial.println("The updated number of patient who got vaccine is:");
  Serial.println(update_patients);
  
  lcd.clear();
  lcd.print("Previous number                         of Vaccinated:");
  delay(1000);
  lcd.clear();
  
        if(digitalRead(PIR)==HIGH)//robbery alarm starts
    	PIRAlert();
        
        if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))//temp check
		HighTemp();
  
   lcd.println(patients_previous_update);
   delay(1000);
   lcd.clear();
  
        if(digitalRead(PIR)==HIGH)//robbery alarm starts
    	PIRAlert();
        
        if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0))//temp check
		HighTemp();
  
   lcd.print("Updated number                          of Vaccinated:");
   delay(1000);
   lcd.clear();
  
        if(digitalRead(PIR)==HIGH)//robbery alarm starts
    	PIRAlert();
        
        if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0)) // Temp alert Only if there are batches in stock
		HighTemp();
  
   lcd.println(update_patients);
   delay(1000);
   lcd.clear();
}
//Blink blue led function - while patient is vaccinated
void BlinkBlueLed()
{

		digitalWrite(blue_light, HIGH); // set the led on
		delay(500); // wait for a 500 mili-second

		digitalWrite(blue_light, LOW); // turn the LED off
		delay(500); // wait for a 500 mili-second
		digitalWrite(blue_light, LOW);
  
}
//robbery alarm function
void PIRAlert()
{
  int alarm_led_color=1;//for flashing the led light in 7 different colors
  digitalWrite(buzzerPin,HIGH);//alarm buzzer on
  lcd.clear();
  lcd.print ("Stealing alert");
  Serial.println("Enter passcode:");
  input_serial_num =NULL;
  while(input_serial_num !='7')
	{      
        	if (Serial.available() > 0)   
   				input_serial_num = Serial.read();
    
        	if(ReadTemp() > max_temp)//temperature alarm and robbery together
        	{
              digitalWrite(green_light, LOW);
              digitalWrite(blue_light, LOW);
              digitalWrite(red_light, LOW);
              
                while(input_serial_num !='8')
				{                 
                if (Serial.available() > 0)   
   				input_serial_num = Serial.read();
                
                if (input_serial_num == '0')// Inventory update
                {
				PrintInventory();
                input_serial_num =NULL;
                }
                  
              	if (alarm_led_color==1) //red light blink   
              	digitalWrite(red_light, HIGH);
                  
              	if (alarm_led_color==2)    
              	digitalWrite(red_light, LOW);
                 
                if (alarm_led_color==3)  //blue light blink
              	digitalWrite(blue_light, HIGH);

                if (alarm_led_color==4)  
              	digitalWrite(blue_light, LOW);
                  
                if (alarm_led_color==5)//green light blink
              	digitalWrite(green_light, HIGH);
                
                if (alarm_led_color==6)
              	digitalWrite(green_light, LOW);
                  
                  delay(500);
                  
                  alarm_led_color++;
                  
                  if (alarm_led_color==7)
                    alarm_led_color=1;
                }
              digitalWrite(green_light, LOW);
              digitalWrite(blue_light, LOW);
              digitalWrite(red_light, LOW);
              input_serial_num =NULL;
              
              HighTempDuringRobbery();//calls a special high temp function- during a robbery
              input_serial_num =3;
        	}
    
    		if (input_serial_num == '0')//Inventory update
			PrintInventory();
    
			if(input_serial_num != NULL && input_serial_num !='7')//Wrong code
            {
              	lcd.clear();
              	Serial.println("Re-Enter passcode:");
				WrongCode();
    			input_serial_num =NULL;
            }
    }//end while
  
digitalWrite(buzzerPin,LOW); //alarm buzzer off
lcd.clear();
lcd.print ("Alert canceled");
delay(2000);
lcd.clear();
}
//robbery- wrong password inserted
void WrongCode()
{
	int random_batch;
	int batch_expired=0;
	int batch_ex_in_1_day=0;
	int batch_ex_in_2_day=0;
	//Random selection of stolen batches
	for (int i = 0; i < 2; i++)
	{
		if ((batch_full[0]+batch_full[1]+batch_full[2]) !=0)// There are batches in stock
		{
			random_batch= random(0, (batch_full[0]+batch_full[1]+batch_full[2]));
			if(batch_full[0] !=0 && random_batch < batch_full[0])//batch_full[0] stole
			{
				batch_full[0]=batch_full[0]-1;
				batch_expired=batch_expired+1;
			}    
			if(batch_full[1] !=0 && random_batch >= batch_full[0] && random_batch < (batch_full[0] + batch_full[1]))//batch_full[1] stole
			{
				batch_full[1]=batch_full[1]-1;
				batch_ex_in_1_day=batch_ex_in_1_day+1;
			}    
			if(batch_full[2] !=0 && random_batch >= (batch_full[0] + batch_full[1]) && random_batch < (batch_full[0] + batch_full[1]+batch_full[2]))//batch_full[2] stole
			{
				batch_full[2]=batch_full[2]-1;
				batch_ex_in_2_day=batch_ex_in_2_day+1;
			}
		}
	}
	if(batch_expired==2)
	{
		lcd.print ("2 expired batch-                        es were stolen");
		delay(2000);
		lcd.clear();
	}
	if(batch_ex_in_1_day==2)
	{
		lcd.print ("2 batches expire                        in 1 day stolen");
		delay(2000);
		lcd.clear();
	}
	if(batch_ex_in_2_day==2)
	{
		lcd.print ("2 batches expire                        in 2 day stolen");
		delay(2000);
		lcd.clear();
	}
	if(batch_expired==0 && batch_ex_in_1_day==0 && batch_ex_in_2_day==0)
	{
		lcd.print ("No batches were                         stolen");
		delay(2000);
		lcd.clear();
	}
	else if(batch_expired!=2 && batch_ex_in_1_day!=2 && batch_ex_in_2_day!=2)
	{
		if (batch_expired==1)
		{
			lcd.print ("1 expired batch                         were stolen");
			delay(2000);
			lcd.clear();
			lcd.print ("&");  
			delay(1000);
			lcd.clear();
		}
		if (batch_ex_in_1_day==1)
		{
			lcd.print ("1 batch expire                          in 1 day stolen");
			delay(2000);
			lcd.clear();
			if (batch_ex_in_2_day==1)
			{
				lcd.print ("&");  
				delay(1000);
				lcd.clear();
			}
		}
		if (batch_ex_in_2_day==1)
		{
			lcd.print ("1 batch expire                          in 2 day stolen");
			delay(2000);
			lcd.clear();
		} 
	}
}//end void WrongCode
//high temp alarm
void HighTemp()
{
  {
    int alarm_led_color=1;
		Serial.println("temp is high");
		ValidityDecreas();//the validity of the batches decreases by 1 day
		temp_cycles=0;
		digitalWrite(green_light,LOW);
		input_serial_num=NULL;
		while(input_serial_num !='6')
		{        
        	if(digitalRead(PIR)==HIGH)//temperature and robbery alarm together
        	{
				digitalWrite(green_light, LOW);
				digitalWrite(blue_light, LOW);
				digitalWrite(red_light, LOW);
				input_serial_num=NULL;
                while(input_serial_num !='8')
				{                  
					if (Serial.available() > 0)   
						input_serial_num = Serial.read();
					if (input_serial_num == '0')
					{
						PrintInventory();
						input_serial_num =NULL;
					}
					if (alarm_led_color==1)// red light blink   
						digitalWrite(red_light, HIGH);
					if (alarm_led_color==2)    
						digitalWrite(red_light, LOW); 
					if (alarm_led_color==3)// blue light blink     
						digitalWrite(blue_light, HIGH);
					if (alarm_led_color==4)  
						digitalWrite(blue_light, LOW);
					if (alarm_led_color==5)// green light blink   
						digitalWrite(green_light, HIGH);
					if (alarm_led_color==6)
						digitalWrite(green_light, LOW);
					delay(500);
					alarm_led_color++;
					if (alarm_led_color==7)
						alarm_led_color=1;
                }
				digitalWrite(green_light, LOW);
				digitalWrite(blue_light, LOW);
				digitalWrite(red_light, LOW);
				input_serial_num =NULL;
				PIRAlertDuringHighTemp();//calls a special robbery alarm function- during an high temp occasion
        	}
        	if (Serial.available() > 0) //Inventory update
        	{    
   				input_serial_num = Serial.read();
    			if (input_serial_num == '0')
					PrintInventory();
        	}//end if (Serial.available > 0)
			if(temp_cycles<7)
			{
				digitalWrite(buzzerPin,HIGH);
			}
			else
			{
				digitalWrite(buzzerPin,LOW);
			}
			BlinkBlueLed();// Duration 1 second
			temp_cycles = temp_cycles + 1;
		}//end while
      digitalWrite(buzzerPin,LOW);//turn off buzzer
    }//end if
    input_serial_num=NULL;
}
//high temp alarm that occurs during a robbery 
void HighTempDuringRobbery()
{
    int alarm_led_color=1;
    Serial.println("The temp is high");
    ValidityDecreas();//the validity get down by 1 day
    temp_cycles=0;
    digitalWrite(green_light,LOW);
    input_serial_num=NULL;
    while(input_serial_num !='6')
	{
  
        if (Serial.available() > 0) // Inventory update
        {    
   			input_serial_num = Serial.read();
    		if (input_serial_num == '0')
				PrintInventory();
        }//end if (Serial.available > 0)
        if(temp_cycles<7)
        {
			digitalWrite(buzzerPin,HIGH);
        }
        else
        {
			digitalWrite(buzzerPin,LOW);
        }
        	BlinkBlueLed();// duration 1 second
        temp_cycles = temp_cycles + 1;
	}//end while
    digitalWrite(buzzerPin,LOW);
    input_serial_num=NULL;
}
//robbery alarm that occurs during an high temp occasion 
void PIRAlertDuringHighTemp()
{
	int alarm_led_color=1;
	digitalWrite(buzzerPin,HIGH);
	lcd.clear();
	lcd.print ("Stealing alert");
	Serial.println("Enter passcode:");
	input_serial_num =NULL;
	while(input_serial_num !='7')
	{  
        if (Serial.available() > 0) //Inventory update  
   			input_serial_num = Serial.read();
		if (input_serial_num == '0')
			PrintInventory();
		if(input_serial_num != NULL && input_serial_num !='7')
        {
            lcd.clear();
            Serial.println("Re-Enter passcode:");
			WrongCode();
    		input_serial_num =NULL;
        }
    }//end while
	digitalWrite(buzzerPin,LOW); 
	lcd.clear();
	lcd.print ("Alert canceled");
	delay(2000);
	lcd.clear();
}
//recycle batches action
void RecycleBatches()
{
	lcd.clear();
	lcd.print ("Recycle procces                         packing batches");
	for (int i=0;i<5;i++)
	{
		if(digitalRead(PIR)==HIGH)
		PIRAlert();//robbery alarm starts
		if(ReadTemp() > max_temp && (batch_half_empty[1]>0 || batch_half_empty[2]>0 || batch_full[1]>0 || batch_full[2]>0)) //Temp alert Only if there are batches in stock
		HighTemp();
		if (Serial.available() > 0) //Inventory update 
		{    
			input_serial_num = Serial.read();
			if (input_serial_num == '0')
				PrintInventory();
		}//end if (Serial.available > 0)
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