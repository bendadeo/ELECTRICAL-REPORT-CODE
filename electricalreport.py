
import tkinter as tk
from tkinter import messagebox
import sqlite3

# =========================
# DATABASE SETUP
# =========================
conn = sqlite3.connect(r"D:\pythonforelectricalreport\students.db")
cursor = conn.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS students (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT,
    name TEXT,
    mssv TEXT
)
""")

conn.commit()

# =========================
# MAIN WINDOW
# =========================
root = tk.Tk()
root.title("Student Management")
root.geometry("700x550")

# =========================
# TITLE
# =========================
title_label = tk.Label(
    root,
    text="Add Students",
    font=("Arial", 18, "bold")
)
title_label.pack(pady=10)

label_frame = tk.Frame(root)
label_frame.pack()
tk.Label(
    label_frame,
    text = "UID",
    font = ("Arial",12,"bold"),
    width = 25
).grid(row =0,column= 0 ,padx= 0)

tk.Label(
    label_frame,
    text ="NAME",
    font=("Arial",12,"bold"),
    width = 25
).grid(row = 0 , column = 1, padx = 0)
    
tk.Label(
    label_frame,
    text ="STUDENT ID",
    font=("Arial",12,"bold"),
    width = 25
).grid(row = 0 , column = 2, padx = 0)
# =========================
# FRAME FOR INPUT ROWS
# =========================
rows_frame = tk.Frame(root)
rows_frame.pack(pady=10)

# Store all entry widgets
student_rows = []

# =========================
# FUNCTION TO ADD NEW ROW
# =========================
def add_row():

    row_frame = tk.Frame(rows_frame)
    row_frame.pack(pady=5)

    uid_entry = tk.Entry(
        row_frame,
        width= 25,
        font =("Arial",12)
    )
    uid_entry.pack(side=tk.LEFT, padx=5)
    
    # Name Entry
    name_entry = tk.Entry(
        row_frame,
        width=25,
        font=("Arial", 12)
    )
    name_entry.pack(side=tk.LEFT, padx=5)

    # MSSV Entry
    mssv_entry = tk.Entry(
        row_frame,
        width=25,
        font=("Arial", 12)
    )
    mssv_entry.pack(side=tk.LEFT, padx=5)
    
    student_rows.append((uid_entry,name_entry, mssv_entry))


def add_uid_to_last_row(uid):

    uid_entry, name_entry, mssv_entry = student_rows[-1]

    uid_entry.delete(0, tk.END)

    uid_entry.insert(0, uid)



# =========================
# SAVE FUNCTION
# =========================
def save_students():

    for uid_entry,name_entry, mssv_entry in student_rows:

        uid  = uid_entry.get().strip()
        name = name_entry.get().strip()
        mssv = mssv_entry.get().strip()


        if name and mssv:

            cursor.execute("""
            INSERT INTO students (uid,name, mssv)
            VALUES (?, ?,?)
            """, (uid,name, mssv))

    conn.commit()

    messagebox.showinfo(
        "Success",
        "Students saved successfully!"
    )
    root.destroy()

# =========================
# BUTTON FRAME
# =========================
button_frame = tk.Frame(root)
button_frame.pack(pady=20)

# Add Row Button
add_button = tk.Button(
    button_frame,
    text="+",
    font=("Arial", 16, "bold"),
    width=4,
    command=add_row
)
add_button.pack(side=tk.LEFT, padx=10)

# Save Button
save_button = tk.Button(
    button_frame,
    text="Save",
    font=("Arial", 12),
    width=10,
    command=save_students
)
save_button.pack(side=tk.LEFT, padx=10)

# Add first row automatically
add_row()

# =========================
# RUN GUI
# =========================
root.mainloop()

# Close database when app closes
conn.close()

