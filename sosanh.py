import serial
import sqlite3
import time

COM_PORT = "COM8"
BAUDRATE = 9600
DATABASE = r"D:\pythonforelectricalreport\students.db"

ser = serial.Serial(COM_PORT, BAUDRATE, timeout=1)
time.sleep(2)

conn = sqlite3.connect(DATABASE)
cursor = conn.cursor()


print("PLEASE PLACE YOUR CARD ON THE RFID")
try:
    while True:

        line = ser.readline().decode(errors="ignore").strip() 

        if not line:
            continue

        print("RECEIVED:", line)

        if line.startswith("UID:"):

            uid = line[4:].strip()

            cursor.execute(
                """
                SELECT NAME, MSSV
                FROM students
                WHERE UID = ?
                """,
                (uid,)
            )

            result = cursor.fetchone()

            if result:

                name, mssv = result

                print("===================")
                print("ACCESS GRANTED")
                print(f"NAME : {name}")
                print(f"MSSV : {mssv}")
                print("===================")

                ser.write(b"OPEN\n")

            else:
                print("CAN NOT DETECT")
                ser.write(b"DENY\n")
                


except KeyboardInterrupt:
    print("\nProgram stopped.")

finally:
    conn.close()
    ser.close()
