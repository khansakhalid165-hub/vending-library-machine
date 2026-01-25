 #include<iostream>
#include<vector>
#include<iomanip>
#include<ctime>
#include<map>
#include<string>
#include<algorithm>
#include<limits>
#include <fstream>
#include <sstream>

using namespace std;

const double MEMBERSHIP_DEPOSIT = 1000.0;

class book{
    string bookId;
    string bookName;
    string author;
    int totalCopies;
    int availableCopies;
    public:
    book(string bookId,string bookName,string author,int totalCopies):bookId(bookId),bookName(bookName),author(author),totalCopies(totalCopies),availableCopies(totalCopies){}
    
    string getBookId(){ return bookId; }
    string getBookName(){ return bookName; }
    string getAuthor(){ return author; }
    int getTotalCopies(){ return totalCopies; }
    int getAvailableCopies(){ return availableCopies; }
    void setAvailableCopies(int copies) {
    availableCopies = copies;
}

    
    bool isAvailable(){
        return availableCopies > 0;
    }
    
    void borrowBook(){
        if(availableCopies > 0){
            availableCopies--;
        }
    }
    
    void returnbook(){
        if(availableCopies < totalCopies){
            availableCopies++;
        }
    }
    
    void setTotalCopies(int copies){
        totalCopies = copies;
        if(availableCopies > totalCopies){
            availableCopies = totalCopies;
        }
    }
    
    void displayInfo() const {
        cout << "  [BOOK] " << bookName << endl;
        cout << "         Author: " << author << endl;
        cout << "         Available: " << availableCopies << "/" << totalCopies << " copies" << endl;
    }
};

class guest{
    string guestId;
    string guestName;
    vector<string> borrowedBooks;
    double depositBalance;
    public:
    guest(string guestId,string guestName):guestId(guestId),guestName(guestName),depositBalance(0.0){}
    
    string getGuestId(){ return guestId; }
    string getGuestName(){ return guestName; }
    double getDepositBalance(){ return depositBalance; }
    vector<string> getBorrowedBooks(){ return borrowedBooks; }
    
    void setDepositBalance(double amount){
        depositBalance = amount;
    }
    
    void addBorrowedBook(string bookId){
        borrowedBooks.push_back(bookId);
    }
    
    void returnBorrowedBook(string bookId){
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookId);
        if(it != borrowedBooks.end()){
            borrowedBooks.erase(it);
        }
    }
    
    void deductFine(double amount){
        depositBalance -= amount;
        if(depositBalance < 0) depositBalance = 0;
    }
    
    void displayInfo() const {
        cout << "  [USER] " << guestName << " (ID: " << guestId << ")" << endl;
        cout << "         Deposit Balance: $" << depositBalance << endl;
        cout << "         Borrowed Books: " << borrowedBooks.size() << endl;
    }
};

class borrowRecord{
    string recordId;
    string guestId;
    string bookId;
    time_t borrowDate;
    time_t returnDate;
    bool isReturned;
    public:
    borrowRecord(string recordId,string guestId,string bookId):recordId(recordId),guestId(guestId),bookId(bookId),isReturned(false){
        borrowDate = time(0);
        returnDate = borrowDate + (7 * 24 * 60 * 60);
    }
    
    borrowRecord(string recordId,string guestId,string bookId,time_t borrowDate,time_t returnDate,bool isReturned):recordId(recordId),guestId(guestId),bookId(bookId),borrowDate(borrowDate),returnDate(returnDate),isReturned(isReturned){}
    
    string getRecordId(){ return recordId; }
    string getGuestId(){ return guestId; }
    string getBookId(){ return bookId; }
    time_t getBorrowDate(){ return borrowDate; }
    time_t getReturnDate(){ return returnDate; }
    bool isBookReturned(){ return isReturned; }
    
    int getDaysOverdue() const {
        if(isReturned) return 0;
        
        time_t currentTime = time(0);
        if(currentTime > returnDate){
            double secondsOverdue = difftime(currentTime, returnDate);
            int daysOverdue = secondsOverdue / (60 * 60 * 24);
            return (daysOverdue > 0) ? daysOverdue : 0;
        }
        return 0;
    }
    
    void issueReturn(){
        isReturned = true;
    }
};

class library{
    map<string,book*> books;
    map<string,guest*> guests;
    vector<borrowRecord*> borrowRecords;
    int recordCounter;
    int guestCounter;
    int lastGuestNumber = 5;   

    
    public:
    library():recordCounter(1), guestCounter(1){}
    
    ~library(){
        for(auto& pair : books){
            delete pair.second;
        }
        for(auto& pair : guests){
            delete pair.second;
        }
        for(auto record : borrowRecords){
            delete record;
        }
    }
    void saveBorrowRecordsToFile() {
    ofstream file("borrow_records.txt");

    for (auto record : borrowRecords) {
        file << record->getRecordId() << "|"
             << record->getGuestId() << "|"
             << record->getBookId() << "|"
             << record->getBorrowDate() << "|"
             << record->getReturnDate() << "|"
             << record->isBookReturned() << endl;
    }
    file.close();
}
void loadBorrowRecordsFromFile() {
    ifstream file("borrow_records.txt");
    if (!file) return;

    borrowRecords.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string recordId, guestId, bookId;
        time_t borrowDate, returnDate;
        bool returned;

        getline(ss, recordId, '|');
        getline(ss, guestId, '|');
        getline(ss, bookId, '|');
        ss >> borrowDate;
        ss.ignore();
        ss >> returnDate;
        ss.ignore();
        ss >> returned;

        borrowRecord* r = new borrowRecord(recordId,guestId,bookId,borrowDate,returnDate,returned);
        borrowRecords.push_back(r);

        if (!returned && books.count(bookId)) {
            books[bookId]->borrowBook();}

        if (!returned && guests.count(guestId)) {
            guests[guestId]->addBorrowedBook(bookId);
        }
    }
    file.close();
}

    void saveBooksToFile() {
    ofstream file("books.txt");
    for (auto &pair : books) {
        book* b = pair.second;
        file << b->getBookId() << "|"
             << b->getBookName() << "|"
             << b->getAuthor() << "|"
             << b->getTotalCopies() << "|"
             << b->getAvailableCopies() << endl;
    }
    file.close();
}
void loadBooksFromFile() {
    ifstream file("books.txt");
    if (!file) return;   // first time run

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, author;
        int total, available;

        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, author, '|');
        ss >> total;
        ss.ignore();
        ss >> available;

        book* b = new book(id, name, author, total);
        b->setAvailableCopies(available);
        books[id] = b;
    }
    file.close();
}
void saveGuestsToFile() {
    ofstream file("guests.txt");
    for (auto &pair : guests) {
        guest* g = pair.second;
        file << g->getGuestId() << "|"
             << g->getGuestName() << "|"
             << g->getDepositBalance() << endl;
    }
    file.close();
}
void loadGuestsFromFile() {
    ifstream file("guests.txt");
    if (!file) return;

    guests.clear();

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string id, name;
        double balance;

        getline(ss, id, '|');
        getline(ss, name, '|');
        ss >> balance;

        if (id.size() < 2 || id[0] != 'G') continue;

        int num = stoi(id.substr(1));
        lastGuestNumber = max(lastGuestNumber, num);

        guest* g = new guest(id, name);
        g->setDepositBalance(balance);
        guests[id] = g;
    }
    file.close();
}


    
    void displayWelcome(){
        cout << "\n=====================================================" << endl;
        cout << "                                                     " << endl;
        cout << "          VENDING LIBRARY SYSTEM                     " << endl;
        cout << "                                                     " << endl;
        cout << "          Your 24/7 Automated Library                " << endl;
        cout << "                                                     " << endl;
        cout << "=====================================================" << endl;
    }
    
    void displayInstructions(){
        cout << "\n-----------------------------------------------------" << endl;
        cout << "              HOW TO USE THIS SYSTEM                 " << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "                                                     " << endl;
        cout << "  [1] NEW USER?                                      " << endl;
        cout << "      -> Select 'Register' to create account         " << endl;
        cout << "      -> You'll get a unique Guest ID                " << endl;
        cout << "      -> Remember your ID for future visits!         " << endl;
        cout << "                                                     " << endl;
        cout << "  [2] EXISTING USER?                                 " << endl;
        cout << "      -> Select 'Login' and enter your Guest ID      " << endl;
        cout << "      -> Access all features instantly               " << endl;
        cout << "                                                     " << endl;
        cout << "  [$] BORROWING BOOKS:                               " << endl;
        cout << "      -> Pay $1000 deposit (one-time)                " << endl;
        cout << "      -> Borrow 1 book for 7 days FREE               " << endl;
        cout << "      -> Return on time to get full deposit back     " << endl;
        cout << "      -> Late? $5/day fine deducted from deposit     " << endl;
        cout << "                                                     " << endl;
        cout << "  [*] LEAVING?                                       " << endl;
        cout << "      -> Withdraw membership anytime                 " << endl;
        cout << "      -> Get money back OR buy a book                " << endl;
        cout << "                                                     " << endl;
        cout << "  [+] FEATURES:                                      " << endl;
        cout << "      * Browse 20+ books in our catalog              " << endl;
        cout << "      * Search by book name or author                " << endl;
        cout << "      * Easy menu-based selection                    " << endl;
        cout << "      * View your account anytime                    " << endl;
        cout << "                                                     " << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
    void initializeLibrary(){
        // Add 20 Books
        books["B001"] = new book("B001", "C++ Programming", "Bjarne Stroustrup", 3);
        books["B002"] = new book("B002", "Data Structures and Algorithms", "Mark Allen Weiss", 4);
        books["B003"] = new book("B003", "Introduction to Algorithms", "Thomas Cormen", 3);
        books["B004"] = new book("B004", "Operating System Concepts", "Abraham Silberschatz", 2);
        books["B005"] = new book("B005", "Database System Concepts", "Henry Korth", 3);
        books["B006"] = new book("B006", "Computer Networks", "Andrew Tanenbaum", 4);
        books["B007"] = new book("B007", "Artificial Intelligence", "Stuart Russell", 2);
        books["B008"] = new book("B008", "Machine Learning", "Tom Mitchell", 3);
        books["B009"] = new book("B009", "Python Programming", "Mark Lutz", 5);
        books["B010"] = new book("B010", "Java Programming", "Herbert Schildt", 4);
        books["B011"] = new book("B011", "Web Development", "Jon Duckett", 3);
        books["B012"] = new book("B012", "Software Engineering", "Ian Sommerville", 2);
        books["B013"] = new book("B013", "Cloud Computing", "Thomas Erl", 3);
        books["B014"] = new book("B014", "Cybersecurity Essentials", "Charles Brooks", 2);
        books["B015"] = new book("B015", "Blockchain Technology", "Imran Bashir", 2);
        books["B016"] = new book("B016", "Deep Learning", "Ian Goodfellow", 3);
        books["B017"] = new book("B017", "Computer Graphics", "Donald Hearn", 2);
        books["B018"] = new book("B018", "Compiler Design", "Alfred Aho", 2);
        books["B019"] = new book("B019", "Mobile App Development", "Neil Smyth", 3);
        books["B020"] = new book("B020", "DevOps Handbook", "Gene Kim", 2);
        
        // Add 5 Guests
        guests["G001"] = new guest("G001", "Ali Ahmed");
        guests["G002"] = new guest("G002", "Sara Khan");
        guests["G003"] = new guest("G003", "Hassan Ali");
        guests["G004"] = new guest("G004", "Fatima Noor");
        guests["G005"] = new guest("G005", "Usman Tariq");
    }
    
    string registerGuest(){
        cout << "\n--------------------------------------" << endl;
        cout << "      NEW MEMBER REGISTRATION         " << endl;
        cout << "--------------------------------------" << endl;
        cout << "\nEnter your name: ";
        string name;
        cin.ignore();
        getline(cin, name);
        
        string newId = "G" + to_string(++lastGuestNumber);
        guest* g = new guest(newId, name);
        guests[newId] = g;

        
        cout << "\n[SUCCESS] Registration Successful!" << endl;
        cout << "================================" << endl;
        cout << "Your Guest ID: " << newId << endl;
        cout << "** IMPORTANT: Remember this ID!" << endl;
        cout << "================================" << endl;
        
        waitForBack();
        return newId;
    }
    
    string loginGuest(){
        cout << "\n--------------------------------------" << endl;
        cout << "           MEMBER LOGIN               " << endl;
        cout << "--------------------------------------" << endl;
        cout << "\nEnter your Guest ID: ";
        string guestId;
        cin >> guestId;
        
        if(guests.find(guestId) == guests.end()){
            cout << "\n[ERROR] Guest ID not found!" << endl;
            cout << "Please register first or check your ID." << endl;
            waitForBack();
            return "";
        }
        
        guest* g = guests[guestId];
        cout << "\n[SUCCESS] Welcome back, " << g->getGuestName() << "!" << endl;
        waitForBack();
        return guestId;
    }
    
    void waitForBack(){
        cout << "\n[Press 0 + Enter to go back to menu]" << endl;
        int back;
        cin >> back;
    }
    
    string selectBook(){
        cout << "\n--------------------------------------" << endl;
        cout << "         AVAILABLE BOOKS              " << endl;
        cout << "--------------------------------------" << endl;
        
        int num = 1;
        vector<string> bookIds;
        
        for(auto& pair : books){
            book* b = pair.second;
            if(b->isAvailable()){
                cout << "\n" << num << ". " << b->getBookName() << endl;
                cout << "   by " << b->getAuthor() << endl;
                cout << "   (" << b->getAvailableCopies() << " copies available)" << endl;
                bookIds.push_back(b->getBookId());
                num++;
            }
        }
        
        if(bookIds.empty()){
            cout << "\n[ERROR] No books currently available!" << endl;
            waitForBack();
            return "";
        }
        
        cout << "\n0. <- Back to Menu" << endl;
        cout << "\nSelect book number: ";
        
        int choice;
        cin >> choice;
        
        if(choice == 0) return "";
        
        if(choice >= 1 && choice < num){
            return bookIds[choice - 1];
        }
        
        cout << "[ERROR] Invalid selection!" << endl;
        waitForBack();
        return "";
    }
    
    void borrowBook(string guestId){
        guest* g = guests[guestId];
        
        // Check if already borrowed
        if(!g->getBorrowedBooks().empty()){
            cout << "\n[WARNING] You already have a borrowed book!" << endl;
            cout << "Please return it first to borrow another." << endl;
            waitForBack();
            return;
        }
        
        // Check deposit
        if(g->getDepositBalance() < MEMBERSHIP_DEPOSIT){
            double needed = MEMBERSHIP_DEPOSIT - g->getDepositBalance();
            cout << "\n--------------------------------------" << endl;
            cout << "         DEPOSIT REQUIRED             " << endl;
            cout << "--------------------------------------" << endl;
            cout << "\nTotal deposit needed: $" << MEMBERSHIP_DEPOSIT << endl;
            cout << "Amount to pay: $" << needed << endl;
            cout << "\n[INFO] This deposit is refundable when you:" << endl;
            cout << "   * Return book on time, OR" << endl;
            cout << "   * Withdraw your membership" << endl;
            cout << "\nPay now? (1. Yes  2. No): ";
            
            int choice;
            cin >> choice;
            
            if(choice == 1){
                g->setDepositBalance(MEMBERSHIP_DEPOSIT);
                cout << "\n[SUCCESS] Deposit paid successfully!" << endl;
            }
            else{
                cout << "\n[ERROR] Cannot borrow without deposit." << endl;
                waitForBack();
                return;
            }
        }
        
        // Select book
        string bookId = selectBook();
        if(bookId == "") return;
        
        book* b = books[bookId];
        
        // Borrow
        b->borrowBook();
        g->addBorrowedBook(bookId);
        
        string recordId = "R" + to_string(recordCounter++);
        borrowRecord* newRecord = new borrowRecord(recordId, guestId, bookId);
        borrowRecords.push_back(newRecord);
        
        cout << "\n[SUCCESS] Book Borrowed Successfully!" << endl;
        cout << "================================" << endl;
        cout << "Book: " << b->getBookName() << endl;
        time_t dueDate = newRecord->getReturnDate();
        cout << "Due Date: " << ctime(&dueDate);
        cout << "[REMINDER] Return within 7 days to avoid $5/day fine!" << endl;
        
        waitForBack();
    }
    
    void returnBook(string guestId){
        guest* g = guests[guestId];
        
        if(g->getBorrowedBooks().empty()){
            cout << "\n[WARNING] You have no borrowed books!" << endl;
            waitForBack();
            return;
        }
        
        string bookId = g->getBorrowedBooks()[0];
        book* b = books[bookId];
        
        cout << "\n--------------------------------------" << endl;
        cout << "           RETURN BOOK                " << endl;
        cout << "--------------------------------------" << endl;
        cout << "\nReturning: " << b->getBookName() << endl;
        
        // Find record
        for(auto& record : borrowRecords){
            if(record->getGuestId() == guestId && record->getBookId() == bookId && !record->isBookReturned()){
                
                int overdueDays = record->getDaysOverdue();
                
                if(overdueDays > 0){
                    double fine = overdueDays * 5.0;
                    cout << "\n[WARNING] LATE RETURN!" << endl;
                    cout << "================================" << endl;
                    cout << "Days overdue: " << overdueDays << endl;
                    cout << "Fine: $" << fine << endl;
                    g->deductFine(fine);
                    cout << "Fine deducted from deposit." << endl;
                    cout << "Remaining deposit: $" << g->getDepositBalance() << endl;
                }
                else{
                    cout << "\n[SUCCESS] Returned on time! No fines." << endl;
                }
                
                record->issueReturn();
                b->returnbook();
                g->returnBorrowedBook(bookId);
                
                cout << "\n[SUCCESS] Book returned successfully!" << endl;
                cout << "Thank you for using our library!" << endl;
                
                waitForBack();
                return;
            }
        }
    }
    
    void withdrawMembership(string guestId){
        guest* g = guests[guestId];
        
        if(!g->getBorrowedBooks().empty()){
            cout << "\n[ERROR] Cannot withdraw membership!" << endl;
            cout << "Please return all borrowed books first." << endl;
            waitForBack();
            return;
        }
        
        cout << "\n--------------------------------------" << endl;
        cout << "      WITHDRAW MEMBERSHIP             " << endl;
        cout << "--------------------------------------" << endl;
        cout << "\nAre you sure you want to leave?" << endl;
        cout << "1. Yes, withdraw" << endl;
        cout << "2. No, stay" << endl;
        cout << "\nChoice: ";
        
        int choice;
        cin >> choice;
        
        if(choice != 1){
            cout << "\n[SUCCESS] Glad you're staying!" << endl;
            waitForBack();
            return;
        }
        
        double balance = g->getDepositBalance();
        
        if(balance < MEMBERSHIP_DEPOSIT){
            cout << "\nYour remaining balance: $" << balance << endl;
            cout << "[SUCCESS] Money refunded!" << endl;
        }
        else{
            cout << "\nYour deposit: $" << balance << endl;
            cout << "\n1. Get money back" << endl;
            cout << "2. Buy a book with deposit" << endl;
            cout << "\nChoose: ";
            
            int opt;
            cin >> opt;
            
            if(opt == 1){
                cout << "\n[SUCCESS] $" << balance << " refunded!" << endl;
            }
            else if(opt == 2){
                cout << "\n--------------------------------------" << endl;
                cout << "      SELECT BOOK TO BUY              " << endl;
                cout << "--------------------------------------" << endl;
                
                int num = 1;
                vector<string> bookIds;
                
                for(auto& pair : books){
                    book* b = pair.second;
                    cout << "\n" << num << ". " << b->getBookName();
                    cout << " by " << b->getAuthor() << endl;
                    bookIds.push_back(b->getBookId());
                    num++;
                }
                
                cout << "\nSelect: ";
                int bookChoice;
                cin >> bookChoice;
                
                if(bookChoice >= 1 && bookChoice < num){
                    string selectedBookId = bookIds[bookChoice - 1];
                    book* b = books[selectedBookId];
                    
                    cout << "\n[SUCCESS] Purchased: " << b->getBookName() << endl;
                    
                    int newTotal = b->getTotalCopies() - 1;
                    
                    if(newTotal == 0){
                        delete b;
                        books.erase(selectedBookId);
                    }
                    else{
                        b->setTotalCopies(newTotal);
                        if(b->getAvailableCopies() > 0){
                            b->borrowBook();
                        }
                    }
                }
            }
        }
        
        delete g;
        guests.erase(guestId);
        cout << "\n[SUCCESS] Membership withdrawn." << endl;
        cout << "Thank you for being a member!" << endl;
    }
    
    void searchBookByName(){
        cout << "\n--------------------------------------" << endl;
        cout << "       SEARCH BY BOOK NAME            " << endl;
        cout << "--------------------------------------" << endl;
        cout << "\nEnter book name: ";
        string searchName;
        cin.ignore();
        getline(cin, searchName);
        
        cout << "\n======= SEARCH RESULTS =======" << endl;
        
        bool found = false;
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
        
        for(auto& pair : books){
            book* b = pair.second;
            string bookName = b->getBookName();
            transform(bookName.begin(), bookName.end(), bookName.begin(), ::tolower);
            
            if(bookName.find(searchName) != string::npos){
                cout << endl;
                b->displayInfo();
                found = true;
            }
        }
        
        if(!found){
            cout << "\n[ERROR] No books found matching: \"" << searchName << "\"" << endl;
        }
        
        waitForBack();
    }
    
    void searchBookByAuthor(){
        cout << "\n--------------------------------------" << endl;
        cout << "       SEARCH BY AUTHOR               " << endl;
        cout << "--------------------------------------" << endl;
        cout << "\nEnter author name: ";
        string authorName;
        cin.ignore();
        getline(cin, authorName);
        
        cout << "\n======= BOOKS BY THIS AUTHOR =======" << endl;
        
        bool found = false;
        transform(authorName.begin(), authorName.end(), authorName.begin(), ::tolower);
        
        for(auto& pair : books){
            book* b = pair.second;
            string author = b->getAuthor();
            transform(author.begin(), author.end(), author.begin(), ::tolower);
            
            if(author.find(authorName) != string::npos){
                cout << endl;
                b->displayInfo();
                found = true;
            }
        }
        
        if(!found){
            cout << "\n[ERROR] No books found by: \"" << authorName << "\"" << endl;
        }
        
        waitForBack();
    }
    
    void displayAllBooks(){
        cout << "\n--------------------------------------" << endl;
        cout << "           BOOK CATALOG               " << endl;
        cout << "--------------------------------------" << endl;
        
        for(const auto& pair : books){
            cout << endl;
            pair.second->displayInfo();
        }
        
        waitForBack();
    }
    
    bool isEmpty() {
    return books.empty();
}

    
    void viewMyAccount(string guestId){
        cout << "\n--------------------------------------" << endl;
        cout << "            MY ACCOUNT                " << endl;
        cout << "--------------------------------------" << endl;
        
        guest* g = guests[guestId];
        cout << endl;
        g->displayInfo();
        
        vector<string> borrowedBooks = g->getBorrowedBooks();
        if(!borrowedBooks.empty()){
            cout << "\n======= Currently Borrowed =======" << endl;
            for(const string& bookId : borrowedBooks){
                auto bookIt = books.find(bookId);
                if(bookIt != books.end()){
                    cout << "  [BOOK] " << bookIt->second->getBookName() << endl;
                }
            }
        }
        
        waitForBack();
    }
};

void displayMainMenu(){
    cout << "\n=====================================================" << endl;
    cout << "                  MAIN MENU                          " << endl;
    cout << "=====================================================" << endl;
    cout << "  1. Register as New Member                          " << endl;
    cout << "  2. Login (Existing Member)                         " << endl;
    cout << "  3. View Instructions                               " << endl;
    cout << "  0. Exit                                            " << endl;
    cout << "=====================================================" << endl;
    cout << "\nEnter choice: ";
}

void displayGuestMenu(){
    cout << "\n=====================================================" << endl;
    cout << "                  MEMBER MENU                        " << endl;
    cout << "=====================================================" << endl;
    cout << "  1. Browse All Books                                " << endl;
    cout << "  2. Search Book by Name                             " << endl;
    cout << "  3. Search Book by Author                           " << endl;
    cout << "  4. Borrow a Book                                   " << endl;
    cout << "  5. Return Book                                     " << endl;
    cout << "  6. View My Account                                 " << endl;
    cout << "  7. Withdraw Membership                             " << endl;
    cout << "  0. Logout                                          " << endl;
    cout << "=====================================================" << endl;
    cout << "\nEnter choice: ";
}

int main(){
    library lib;
    lib.loadBooksFromFile();
    lib.loadGuestsFromFile();
    lib.loadBorrowRecordsFromFile();


if (lib.isEmpty()) {   
    lib.initializeLibrary();
}

    lib.displayWelcome();
    
    string loggedInGuestId = "";
    
    while(true){
        if(loggedInGuestId == ""){
            // Not logged in - show main menu
            displayMainMenu();
            int choice;
            cin >> choice;
            
            switch(choice){
                case 1:
                    loggedInGuestId = lib.registerGuest();
                    break;
                case 2:
                    loggedInGuestId = lib.loginGuest();
                    break;
                case 3:
                    lib.displayInstructions();
                    break;
                case 0:
                    cout << "\n Thank you for visiting!" << endl;
                    cout << "Come back soon! " << endl;
                    return 0;
                default:
                    cout << "\n❌ Invalid choice!" << endl;
            }
        }
        else{
            // Logged in - show member menu
            displayGuestMenu();
            int choice;
            cin >> choice;
            
            switch(choice){
                case 1:
                    lib.displayAllBooks();
                    break;
                case 2:
                    lib.searchBookByName();
                    break;
                case 3:
                    lib.searchBookByAuthor();
                    break;
                case 4:
                    lib.borrowBook(loggedInGuestId);
                    break;
                case 5:
                    lib.returnBook(loggedInGuestId);
                    break;
                    
                case 6:
                    lib.viewMyAccount(loggedInGuestId);
                    break;
                    
                case 7:
                    lib.withdrawMembership(loggedInGuestId);
                    loggedInGuestId = "";
                    break;
                    
                case 0:
                    lib.saveBooksToFile();
                    lib.saveGuestsToFile();
                    lib.saveBorrowRecordsToFile();
                    cout << "\n Thank you for visiting!" << endl;
                    cout << "Data saved successfully! " << endl;
                    return 0;

                    
                default:
                    cout << "\n❌ Invalid choice!" << endl;
            }
        }
    }
    
    return 0;
}