# 📚 Vending Library System

**A console-based Library Management System in C++** that simulates a real-world library experience. Users can register, borrow and return books, pay deposits, and withdraw membership.  

---

## 🔹 Features

- User registration and login  
- Borrow books with a $1000 deposit  
- Return books with overdue fine calculation ($5/day)  
- Withdraw membership and get deposit back or buy a book  
- Search books by name or author  
- View account information and borrowed books  
- Persistent data storage in text files (`books.txt`, `guests.txt`, `borrow_records.txt`)  

---

## 🔹 How to Run

1. Ensure you have a **C++ compiler** installed (e.g., g++, Code::Blocks, Visual Studio).  
2. Clone the repository:
   ```bash
   git clone https://github.com/your-username/your-repo-name.git
Navigate to the project folder:

3.cd your-repo-name

5.Compile the program:

g++ -o library main.cpp

5.Run the executable:

./library   # Linux/Mac
library.exe # Windows

🔹 Project Structure
/project-folder
│
├─ main.cpp             # Main program with menu and logic
├─ books.txt            # Book database (auto-generated)
├─ guests.txt           # Guest database (auto-generated)
├─ borrow_records.txt   # Borrow records (auto-generated)
└─ README.md            # This file
