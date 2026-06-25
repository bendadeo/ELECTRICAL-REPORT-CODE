import serial
import sqlite3
ser =  serial.Serial("COM8",115200)
conn = sqlite3.connect("students.db")
cusor = conn.cursor()

while True:
    line = ser.readline().decode().strip()
    if line.startswith("UID"):
        uid = line[4:]
        print(uid)
        cusor.execute(
            "SELECT name FROM students WHERE uid=?",
            (uid,)
        )
        results = cusor.fetchone()
        if results:
            print("NAME:", results[0])
            ser.write(
                f"FOUND:{results[0]}\n".encode()
            )
        else:
            print("unknow card.")
            ser.write(
                b"UNKNOWN,PLSARREST.\n"
            )

