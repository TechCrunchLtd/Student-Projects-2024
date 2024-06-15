
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define MACHINE         13
#define BUZZER          3
#define BUTTON          2
#define ALCOHOL_SENSOR  A0
#define SENSITIVITY     A1
#define RED_LED         7
#define GREEN_LED       6
#define BLUE_LED        5


typedef enum LED_COLOR
{
    NONE,
    RED,
    GREEN,
    BLUE
}
LED_COLOR;

typedef enum SENSOR_STATE
{
    WAIT_SENSOR,
    NO_BREATHE,
    NORMAL_BREATHE,
    ALCOHOL_BREATHE
}
SENSOR_STATE;

typedef struct DEVICE_PARAM
{
    bool machine_state;
    SENSOR_STATE sensor_state;
}
DEVICE_PARAM;

DEVICE_PARAM device_params;
LiquidCrystal_PCF8574 lcd(0x27);

void switchLed(LED_COLOR new_color)
{
    static LED_COLOR current_color = NONE;
    if(current_color != new_color)
    {
        digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        digitalWrite(GREEN_LED, LOW);

        switch(new_color)
        {
            case RED:
                digitalWrite(RED_LED, HIGH);
            break;

            case BLUE:
                digitalWrite(BLUE_LED, HIGH);
            break;

            case GREEN:
                digitalWrite(GREEN_LED, HIGH);
            break;
        }

        current_color = new_color;
    }
}

void switchMachine(bool new_state)
{
    if(new_state != device_params.machine_state)
    {
        device_params.machine_state = new_state;
        digitalWrite(MACHINE, device_params.machine_state);
        Serial.println("Machine switched");
    }
}

void soundBuzzer(void)
{
    static unsigned long buzzer_time = 0;
    if(millis() - buzzer_time > 1000)
    {
        tone(BUZZER, 4000, 100);
        buzzer_time = millis();
    }
}

bool displayInit(void)
{  
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    if(error != 0)
    {
        return false;
    }

    lcd.begin(16, 2);
    lcd.setBacklight(255);

    return true;
}

void displayUpdate(void)
{
    static SENSOR_STATE old_sensor_state = ALCOHOL_BREATHE;
    static unsigned long display_time = 0;

    if((millis() - display_time > 500) /*&& (device_params.sensor_state != old_sensor_state)*/)
    {
        old_sensor_state = device_params.sensor_state;
        Serial.print("Dst: "); Serial.println(old_sensor_state);
        lcd.clear();
        lcd.setCursor(0,0);

        switch(old_sensor_state)
        {
            case NO_BREATHE:
                lcd.print("Waiting for");
                lcd.setCursor(0,1);
                lcd.print("operator breathe");
            break;

            case NORMAL_BREATHE:
                lcd.print("No alcohol sensed");
                lcd.setCursor(0,1);
                if(device_params.machine_state != false)
                {
                    lcd.print("machine running");
                }
                else
                {
                    lcd.print(" machine ready  ");
                }
            break;

            case WAIT_SENSOR:
                lcd.print("Machine Stopped");
            break;

            case ALCOHOL_BREATHE:
                lcd.print("Alcohol detected");
                lcd.setCursor(0,1);
                if(device_params.machine_state != false)
                {
                    lcd.print("unexpected run!!");
                }
                else
                {
                    lcd.print("Operator flagged");
                }
            break;
        }

        display_time = millis();
    }
}

bool buttonPressed(void)
{
    if(digitalRead(BUTTON) == true)
    {
        return true;
    }

    return false;
}

SENSOR_STATE alcholSensed(void)
{
    SENSOR_STATE read_state = NO_BREATHE;
    int threshold = 400;
    int advalue = 0;
    for(int i=0; i<10; i++)
    {
        advalue += analogRead(ALCOHOL_SENSOR);
    }
    advalue = advalue / 10;

    if(advalue < threshold)
    {
        read_state = NO_BREATHE;
    }
    else if(advalue < (threshold + 180))
    {
        if(device_params.sensor_state == NO_BREATHE)
        {
            read_state = NORMAL_BREATHE;
        }
    }
    else
    {
        read_state = ALCOHOL_BREATHE;
    }
    // Serial.print("Read: "); Serial.println(read_state);
    return read_state;
}

void setup()
{
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(MACHINE, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    Serial.begin(9600);

    switchMachine(LOW);
    switchLed(RED);
    soundBuzzer();
    displayInit();

    Serial.println("Init");

    //calibrate sensitivity
    if(buttonPressed())
    {
        lcd.clear();
        // lcd.setCursor(0,1);
        lcd.print("Calibrating...");
        delay(2000);
        lcd.print("state: ");
        while(1)
        {
            lcd.setCursor(9,1);
            lcd.print("          ");
            delay(100);
            lcd.setCursor(9,1);
            if(buttonPressed())
            {
                lcd.print("done!");
                delay(2000);
                break;
            }
            lcd.print(analogRead(ALCOHOL_SENSOR));
            delay(250); 
        }
    }
}

bool start_count = false;
uint8_t counter = 0;
void loop()
{

    switch( device_params.sensor_state)
    {
        case WAIT_SENSOR:
            if(alcholSensed() == NO_BREATHE)
            {
                if(start_count == false)
                {
                    counter = 0;
                    start_count = true;
                }
                else
                {
                    counter++;
                    if(counter == 5)
                    {
                        device_params.sensor_state = NO_BREATHE;
                        delay(1000);
                        start_count = false;
                    }
                }
            }
        break;

        case NO_BREATHE: //wait for breathe
            switchMachine(LOW);
            switchLed(BLUE);
            device_params.sensor_state = alcholSensed();
            if(buttonPressed())
            {
                lcd.clear();
                lcd.print(" PLEASE KINDLY  ");
                lcd.setCursor(0,1);
                lcd.print(" CHECKIN FIRST! ");
                delay(5000);
            }
        break;

        case NORMAL_BREATHE: //enable machine control
            switchLed(GREEN);
            
            if(alcholSensed() == ALCOHOL_BREATHE)
            {
                device_params.sensor_state = ALCOHOL_BREATHE;
                switchLed(NONE);
            }
            else if(buttonPressed())
            {
                if(device_params.machine_state == false)
                  switchMachine(true);
                else
                {
                  switchMachine(false);
                  device_params.sensor_state = WAIT_SENSOR;
                  lcd.clear();
                  lcd.print("Machine Stopped");
                  delay(3000);

                }
                
                delay(2000);
            }
        break;

        case ALCOHOL_BREATHE: //disable machine control
            switchLed(RED);
            switchMachine(LOW);
            if((alcholSensed() == NO_BREATHE) && buttonPressed() )
            {
                device_params.sensor_state = WAIT_SENSOR;
                delay(2000);
            }
        break;
    }

    displayUpdate();
    // Serial.print("Sst: "); Serial.println(device_params.sensor_state);
    // Serial.print("Mst: "); Serial.println(device_params.machine_state);
    delay(100);
}

/* ----------------------------------- EOF ---------------------------------- */