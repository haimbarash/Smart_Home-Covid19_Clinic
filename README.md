# Smart_Home-Covid19_Clinic
Smart Home system- Custom made for auto-manage Covid-19 vaccination clinic

The purpose of the project:
Implement a prototype system of a Smart House Management System for a Covid19 vaccination clinic on arduino uno micro-controller.  
  
As part of the war against the spread of the corona virus, the State of Israel embarked on a large-scale vaccination campaign. Vaccine doses are known to have a short shelf life and must remain under controlled temperature conditions.  
  
**Customer requirements and guidelines:**  
  
•	The clinic opens when the switch is lifted (ON) and with a minimum temperature (Hardware 4) and closes when the switch is turned (OFF) (Hardware 1).  
  
•	Batches of vaccines arrive at the clinic at the beginning of each day. The amount of batches ranges from 1-9 and is determined by implementing input into Serial Monitor. Each batch contains 2 vaccine doses which are valid for two days. You must print to the Serial Monitor a message that a day has started and in addition a message will be printed asking you to enter a number to determine the amount of batches and thus the delivery of the vaccines will arrive (it will officially start today).  
  
•	When receiving the batches, the green light (hardware 2) will flash according to the amount of batches that arrived (in seconds). In addition, the amount of batches received will appear on the LCD screen (hardware 3).  
  
•	During the day, up to 8 patients can arrive to receive a vaccine. Each time a patient arrives, his arrival will be simulated by entering the number "4" from the user (up to 8 patients can arrive - meaning that there can be a day when no patients arrives or situations where several patients arrive together). When a patient arrives, the nurse will vaccinate him with the shortest-acting vaccine dose. The vaccination process will last 5 seconds during which the green light will flash.  
  
•	The amount of vaccines given will be proactively updated by the nurse. Each time a nurse wishes to update the number of patients who received a vaccine, she must enter the number "5", and the number of new daily vaccinations so far will be automatically added. In addition, a message will be displayed on the LCD summarizing the amount of vaccinators before and after the update.  
  
•	You must manage the vaccine inventory so that you know how many vaccine doses there are and what their potency is. That is, when fed the digit “0” for Serial Monitor will result in the following summary: X vaccines that expire in two days, X vaccines that will expire in a day, X vaccines that have expired. Note, this function should display current output and act at every stage of the program.  
  
•	In order to maintain the effeteness of the vaccine doses, it was decided to install a temperature control system; (hardware 4) in the refrigerator where the vaccine doses are stored. If the temperature rises above the minimum temperature, the validity of the vaccinations will decrease by a day (meaning vaccinations that were valid for another two days will now be valid for one day and so on...). In addition, a blue light will flash (hardware 2) and an alarm will be activated (hardware 5) for 6 seconds which will be neutralized by taping the number “6” arc to the Serial Monitor. Please note that the working day of the clinic will not continue if the alarm is not neutralized.  
  
•	As long as there is an available stock of vaccines, a green light will light up (Hardware 2). When the vaccine stock runs out, a static message will appear at the entrance to the clinic on the LCD screen and a red light will light up (hardware 2) until more supplies arrive.  
  
•	In addition, it was decided to install an anti-burglary alarm in the clinic which is active all the time (both when the clinic is open and closed). If the PIR sensor (hardware 6) detects a person moving in the warehouse (the PIR sensor has no sense of distance, it detects movement) the alarm will go off until the correct code (digit 7) is entered into the Serial Monitor. If there is a mistake in entering the code, the thief will manage to steal 2 batches with random validity from the warehouse (validity of two days, one day, expired) - the stolen batches do not have to have the same validity. The type of batches that were stolen must be printed to the LCD (meaning what was their validity). Only when the correct code is entered will the alarm stop working and a message will appear on the screen that the alarm has been successfully neutralized. Please note that the thief is greedy and is only interested in complete batches (batches from which portions have not been taken).  
  
•	When the day is over (turning the switch to the OFF position) a message should appear on the LCD screen that the clinic is closed and in addition the light goes out (hardware 2). Also, a summary must be printed to Serial Monitor which includes the following details: the number of vaccinators per that day and the number of vaccinators so far (in this simulation), the stock of rations and their validity.  
  
•	If the alarm was activated due to a fault in the cooling system (temperature sensor) and at the same time there was a break-in, the light will flash in three colors (red - blue - green) until the alarm is neutralized with entering the code "8" to Serial Monitor.  
  
•	When the number of batches that have been completely used is greater than the number of batches in stock, a message will appear on the LCD screen that the empty vaccines must be packed in order to be sent for recycling. In order to pack the vaccines, the servo motor (hardware 7) will rotate for 5 seconds, and the nurse will not be available to receive audience at that time.  
  
**List of hardware components:**
  
|num                | hardware Name            |  hardware picture |
:-------------------:|:-------------------------:|:-------------------------:
|1|DIP Switch                 |![DIP_Switch](https://user-images.githubusercontent.com/112472485/188282075-55fbbd14-17f4-4329-9c69-325d0ba7272c.PNG)
|2|LED RGB                    |![LED RGB](https://user-images.githubusercontent.com/112472485/188282206-ea211149-9ec4-4ff1-82d2-c645ff992f92.PNG)
|3|LCD Screen                 |![LCD Screen](https://user-images.githubusercontent.com/112472485/188282292-8fcbeba1-c8b6-41fb-a1fd-9a538054c258.PNG)
|4|Temperature sensor         |![Temp sensor](https://user-images.githubusercontent.com/112472485/188282319-d7936742-7428-49ca-b231-0ca221f850fe.PNG)
|5|Piezo (Buzzer)             |![buzzer](https://user-images.githubusercontent.com/112472485/188282344-d9d4f4a2-c9ec-43de-bbe3-30f178022d16.PNG)
|6|PIR Sensor                 |![PIR sensor](https://user-images.githubusercontent.com/112472485/188282441-d99c36a4-1752-4291-8aae-37923b4de5c4.PNG)
|7|Servo                      |![servo](https://user-images.githubusercontent.com/112472485/188282410-9909d90e-88f7-4aa5-9eeb-9457044a11a6.PNG)
  
  
**Electronic Schematic View:**  

![Electronic Schematic View](https://user-images.githubusercontent.com/112472485/188282613-42b6de08-b4e8-4a85-ab65-3757cf25c36f.PNG)


