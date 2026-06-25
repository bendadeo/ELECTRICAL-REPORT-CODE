
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9

// Registers    
#define CommandReg      0x01  
//Start and stop command execution
#define ComIrqReg       0x04 
//Interrupt request bits
#define ErrorReg        0x06 
//Error bits showing the error status
#define FIFODataReg     0x09    
//input and output of 64 byte FIFO buffer
#define FIFOLevelReg    0x0A    
//number of bytes stored in the FIFO buffer
#define BitFramingReg   0x0D 
//adjustments bit-oriented framse
#define ModeReg         0x11 
//defines transmission data rate and framing
#define TxControlReg    0x14  
//control the logical behavior of the antenna drivers
#define TxASKReg        0x15    
//controls the setting of the transmission modulation
#define TModeReg        0x2A    
//defines general modes for transmitting and receiving
#define TPrescalerReg   0x2B    
//defines setting in interal timer
#define TReloadRegH     0x2C     
#define TReloadRegL     0x2D
#define VersionReg      0x37
// show the version of the system

// Commands
#define PCD_IDLE        0x00
//place the Reader to the IDLE state by stopping any executions.
#define PCD_TRANSCEIVE  0x0C
//initiates a transmission to the RFID tag and automatically recives the respond.
#define PCD_SOFTRESET   0x0F
//Reset the state of the system by the register Pin.
// PICC Commands
#define PICC_REQA       0x26
//Request the RFID card in the magnetic field to verify the existence of the card.
#define PICC_ANTICOLL   0x93
//Stop the confliction to take the UID.

const int SERVO_PIN = 3;

uint8_t uid[5];
void servoPulse(int pulseWidth)
{
    digitalWrite(SERVO_PIN, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(SERVO_PIN, LOW);
    delayMicroseconds(20000 - pulseWidth);
}


void servoMove(int pulseWidth)
{
    for(int i = 0; i < 50; i++)
    {
        servoPulse(pulseWidth);
    }
}

void writeRegister(uint8_t reg, uint8_t value)
{
    digitalWrite(SS_PIN, LOW);
    SPI.transfer(reg << 1);
    SPI.transfer(value);
    digitalWrite(SS_PIN, HIGH);
}



uint8_t readRegister(uint8_t reg)
{
    digitalWrite(SS_PIN, LOW);
    SPI.transfer((reg << 1) | 0x80);
    uint8_t value = SPI.transfer(0x00);
    digitalWrite(SS_PIN, HIGH);
    return value;
}




void setBitMask(uint8_t reg, uint8_t mask)
{
    writeRegister(reg, readRegister(reg) | mask);
}

void clearBitMask(uint8_t reg, uint8_t mask)
{
    writeRegister(reg, readRegister(reg) & (~mask));
}

void initRC522()
{
    writeRegister(CommandReg, PCD_SOFTRESET);
    delay(50);
    writeRegister(TModeReg, 0x8D);
    writeRegister(TPrescalerReg, 0x3E);
    writeRegister(TReloadRegL, 30);
    writeRegister(TReloadRegH, 0);
    writeRegister(TxASKReg, 0x40);
    writeRegister(ModeReg, 0x3D);
    uint8_t temp = readRegister(TxControlReg);
    if(!(temp & 0x03))
    {
        writeRegister(TxControlReg,temp | 0x03);
    }

}






    

bool requestREQA()
{
    writeRegister(CommandReg, PCD_IDLE);
    writeRegister(FIFOLevelReg, 0x80);
    writeRegister(FIFODataReg, PICC_REQA);
    writeRegister(BitFramingReg,0x07);
    writeRegister(CommandReg,PCD_TRANSCEIVE);
    setBitMask(BitFramingReg,0x80);
    delay(10);
    uint8_t irq = readRegister(ComIrqReg);
    uint8_t err =readRegister(ErrorReg);
    uint8_t fifo =readRegister(FIFOLevelReg);
    clearBitMask(BitFramingReg,0x80);
    return (fifo == 2);
}




    // Serial.print("IRQ = ");
    // Serial.println(irq, HEX);

    // Serial.print("ERR = ");
    // Serial.println(err, HEX);

    // Serial.print("FIFO = ");
    // Serial.println(fifo);


bool antiCollision()
{
    writeRegister(CommandReg, PCD_IDLE);
    writeRegister(FIFOLevelReg, 0x80);
    writeRegister(FIFODataReg,0x93);
    writeRegister(FIFODataReg,0x20);
    writeRegister(BitFramingReg,0x00);
    writeRegister(CommandReg,PCD_TRANSCEIVE);
    setBitMask(BitFramingReg,0x80);
    delay(10);
    clearBitMask(BitFramingReg,0x80);
    uint8_t err = readRegister(ErrorReg);
    uint8_t fifo = readRegister(FIFOLevelReg);
    if(err & 0x1B)
    {
        return false;
    }

    if(fifo != 5)
    {
        return false;
    }

    for(int i=0;i<5;i++)
    {
        uid[i] =
        readRegister(FIFODataReg);
    }
    return true;
}


void setup()
{
    Serial.begin(9600);

    pinMode(SS_PIN, OUTPUT);
    pinMode(RST_PIN, OUTPUT);

    digitalWrite(SS_PIN, HIGH);
    digitalWrite(RST_PIN, HIGH);

    SPI.begin();

    initRC522();

    // Serial.println("READY");
}

void loop()
{
    if(requestREQA())
    {
        Serial.println("CARD FOUND");

        if(antiCollision())
        {
            Serial.print("UID: ");

            for(int i=0;i<4;i++)
            {
                if(uid[i] < 0x10)
                {
                    Serial.print("0");
                }

                Serial.print(
                    uid[i],
                    HEX
                );
            }

            Serial.println();
        }
    }

    delay(3000);
    if(antiCollision()){
        char uidString[9];
        sprintf(
            uidString,
            "%02X%02X%02X%02X",
            uid[0],
            uid[1],
            uid[2],
            uid[3]
        );
        Serial.print("UID: ");
        Serial.println(uidString);
    }
    if(Serial.available()){
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if(cmd == "OPEN"){
            servoMove(2000);
            delay(3000);
            servoMove(1500);
        }
        else if (cmd == "DENY"){
            servoMove(1500);
        }

    }
}
