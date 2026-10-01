#include <SPI.h> 
#include <RFID.h>
#include <Servo.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

RFID rfid(13, 12);       //D13:pin of tag reader SDA. D12:pin of tag reader RST 
unsigned char status; 
unsigned char str[MAX_LEN]; //MAX_LEN is 16: size of the array 

String accessGranted [2] = {"YOUR_RFID_TAG_1", "YOUR_RFID_TAG_2"};  //RFID serial numbers to grant access to
int accessGrantedSize = 2;                                //The number of serial numbers

Servo lockServo;                //Servo for locking mechanism
int lockPos = 15;               //Locked position limit
int unlockPos = 75;             //Unlocked position limit
boolean locked = true;

int redLEDPin = 24;             // pin for RED LED
int yellowLEDPin = 23;          // pin for YELLOW LED
int greenLEDPin = 22;           // pin for GREEN LED

char array1[] = "PLEASE SCAN YOUR TAG";    // CHANGE THIS AS PER YOUR NEED 
LiquidCrystal_I2C lcd(0x27, 20, 4);        // CHANGE THE 0X27 ADDRESS TO YOUR SCREEN ADDRESS IF NEEDED

char* password = "753"; // change the password here, just pick any 3 numbers
int position = 0;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



void setup() 
{ 
  Serial.begin(9600);           //Serial monitor is only required to get tag ID numbers and for troubleshooting
  SPI.begin();                  //Start SPI communication with reader
  rfid.init();                  //initialization 
  pinMode(redLEDPin, OUTPUT);              //LED startup sequence
  pinMode(greenLEDPin, OUTPUT);
  digitalWrite(redLEDPin, HIGH);
  delay(200);
  digitalWrite(greenLEDPin, HIGH);
  delay(200);
  digitalWrite(redLEDPin, LOW);
  delay(200);
  digitalWrite(greenLEDPin, LOW);
  
  lockServo.attach(11);
  lockServo.attach(lockPos);
  LockedPosition(true);                 //Move servo into locked position

  Serial.println("Place card/tag near reader...");

  lcd.init();
  lcd.backlight();
  lcd.print(array1);
  lcd.setCursor(0,1);
  lcd.setCursor(0,2); 
  lcd.setCursor(0,3);
} 

void loop() 
{ 
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)   //Wait for a tag to be placed near the reader
  { 
    Serial.println("Card found"); 
    String temp = "";                             //Temporary variable to store the read RFID number
    if (rfid.anticoll(str) == MI_OK)              //Anti-collision detection, read tag serial number 
    { 
      Serial.print("The card's ID number is : "); 
      for (int i = 0; i < 4; i++)                 //Record and display the tag serial number 
      { 
        temp = temp + (0x0F & (str[i] >> 4)); 
        temp = temp + (0x0F & str[i]); 
      } 
      Serial.println (temp);
      checkAccess (temp);     //Check if the identified tag is an allowed to open tag
    } 
    rfid.selectTag(str); //Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
  }
  rfid.halt();
}

void LockedPosition(int locked)
{
if (locked)
{
digitalWrite(redLEDPin, HIGH);
digitalWrite(greenLEDPin, LOW);
lockServo.write(1);
}
else
{
digitalWrite(redLEDPin, LOW);
digitalWrite(greenLEDPin, HIGH);
lockServo.write(180);
}
}

void checkAccess (String temp)    //Function to check if an identified tag is registered to allow access
{
  boolean granted = false;
  char array1[] = "  RFID CARD ACCEPTED  ";
  char array2[] = "   ACCESS DENIED  ";

  for (int i=0; i <= (accessGrantedSize-1); i++)    //Runs through all tag ID numbers registered in the array
  {
    if(accessGranted[i] == temp)            //If a tag is found then open/close the lock
    {
      Serial.println ("RFID CARD ACCEPTED");
      granted = true;
      if (locked == true)         //If the lock is closed then open it
      {
          lcd.init();
          lcd.backlight();
          lcd.print(array1);
          lcd.setCursor(0,1);
          lcd.setCursor(0,2);
          lcd.setCursor(0,3);
          askForPin();            // method for entering the 3 digit pin code
          
          lockServo.write(unlockPos);
          locked = false;
      }
      else if (locked == false)   //If the lock is open then close it
      {
          lockServo.write(lockPos);
          locked = true;
      }
      digitalWrite(greenLEDPin, HIGH);    //Green LED sequence
      delay(200);
      digitalWrite(greenLEDPin, LOW);
      delay(200);
      digitalWrite(greenLEDPin, HIGH);
      delay(200);
      digitalWrite(greenLEDPin, LOW);
      delay(200);
    }
  }
  
  if (granted == false)     //If the tag is not found
  {
    Serial.println ("Access Denied");
     lcd.init();
     lcd.backlight();
     lcd.print(array2);
     lcd.setCursor(0,1);
     lcd.setCursor(0,2);
     lcd.setCursor(0,3);
    
    digitalWrite(redLEDPin, HIGH);      //Red LED sequence
    delay(200);
    digitalWrite(redLEDPin, LOW);
    delay(200);
    digitalWrite(redLEDPin, HIGH);
    delay(200);
    digitalWrite(redLEDPin, LOW);
    delay(200);

}
}
void askForPin() {
    LockedPosition(true);
    char array3[] = "  PLEASE ENTER PIN ";
     lcd.init();
     lcd.backlight();
     lcd.print(array3);
     lcd.setCursor(0,1);
     lcd.setCursor(0,2);
     lcd.setCursor(0,3);

     char key = keypad.getKey();
if (key == '*' || key == '#')
{
position = 0;
LockedPosition(true);
}
if (key == password[position])
{
position ++;
}
if (position == 3)
{
LockedPosition(false);
}
delay(100);
digitalWrite(redLEDPin, LOW);
digitalWrite(greenLEDPin, HIGH);
lockServo.write(180);
}


     
