#include <iostream>
#include <fstream>
#include <string>
using namespace std;


class Employee{
    string employeeName;
    int employeeID;
    double hourlyRate;
    int numHoursWorked;
    double wagesDue;
    Employee *previous;
    Employee *next;
public:
    Employee(int num, double rate, string fullname): employeeID(num), hourlyRate(rate), employeeName(fullname),
    numHoursWorked(0), wagesDue(0.00), previous(nullptr), next(nullptr){};
    ~Employee() =default;
    void set_next(Employee* temp_ptr) {next = temp_ptr;}
    void set_previous(Employee* temp_ptr) {previous = temp_ptr;}
    void increment_hours_worked(int num){numHoursWorked += num;}
    int get_hours_worked(){return numHoursWorked;}
    void set_hours_worked(int hours){numHoursWorked = hours;}
    void calculate_wages(){if (hourlyRate == 0.00) cout << "ERROR: No hourly rate entered"; else wagesDue = numHoursWorked * hourlyRate;}
    double get_wages_due(){return wagesDue;}
    string get_name(){return employeeName;}
    int get_ID(){return employeeID;}
    double get_rate(){return hourlyRate;}
    Employee* get_next(){return next;}
    Employee* get_previous(){return previous;}

};

class EmployeeList{
    Employee *head;
    Employee *tail;
public:
    EmployeeList(): head(NULL), tail(NULL){};
    Employee* get_head() {return head;}
    void set_head(Employee* new_head) {head = new_head;}
    Employee* get_tail() {return tail;}
    void set_tail(Employee* new_tail) {tail = new_tail;}
};

void getFile (ifstream& inFile){
    string filename;
    cout << "Enter filename: ";
    cin >> filename;
    inFile.open(filename);
    while (inFile.fail()){
        cout << "ERROR" << endl;
        cout << "File failed to open" << endl;
        cin.ignore(1000, '\n');
        cin.clear();
        inFile.clear();
        cout << "Enter filename: ";
        cin >> filename;
        inFile.open(filename);
    }
}

void stripLeft(string& inString){
    int stringLength = inString.length();
    int index = 0;
    string newString;
    while (inString[index] == ' ')
        index++;
    while (index < stringLength) {
        newString += inString.at(index);
        index++;
    }
    inString = newString;
}

void stripRight(string& inString){
    int stringLength = inString.length();
    int index = stringLength-1;
    string newString;
    while (inString[index] == ' ' || inString[index] == '\n' || inString[index] == '\r')
        index--;
    for (int i = 0; i <= index; i++) {
        newString += inString.at(i);
    }
    inString = newString;
}


void readEmployeesFile(ifstream& inFile, EmployeeList* myList){
    int nextID;
    double pay;
    string fullname;
    Employee *newEmployee;

    while (inFile >> nextID >> pay){
        getline(inFile, fullname);
        stripLeft(fullname);
        stripRight(fullname);
        newEmployee = new Employee(nextID, pay, fullname);
        newEmployee->set_next(myList->get_head());  // sets the new employee node to point to the current head
        if (myList->get_head() == NULL)
            myList->set_tail(newEmployee);
        else
            myList->get_head()->set_previous(newEmployee);  // sets "previous" pointer on current head to new node
        myList->set_head(newEmployee); // points the head pointer to the new node
    }
    inFile.close();
}

void readHoursFile(ifstream& inFile, EmployeeList* myList){
    int nextID;
    int hours;
    Employee *iter = myList->get_head();

    while (inFile >> nextID >> hours){
        while (nextID != iter->get_ID() && iter->get_next() != nullptr){
            iter = iter->get_next();
        }
        if (nextID == iter->get_ID())
            iter->increment_hours_worked(hours);
        else
            cout << "Error: employee number " << nextID << " not found" << endl;
        iter = myList->get_head();
    }

}

void calculateWages(EmployeeList* inList){
    Employee *iter;
    for (iter = inList->get_head(); iter != nullptr; iter = iter->get_next()){
        iter->calculate_wages();
    }
}

void reverseSortByWages(EmployeeList* outList) {
    Employee *current;
    Employee *current_next;
    bool isFinished = false;

    // set marker for the sorted section after first element
    current = outList->get_head()->get_next();
    while (current != nullptr) {
        //select first unsorted element
        while (current->get_next() != nullptr && current->get_wages_due() < current->get_next()->get_wages_due()) {
            current = current->get_next();
        }
        //save cursor position
        current_next = current->get_next();
        if (current_next == nullptr)
            isFinished = true;

        //swap other elements to the right to create the correct position and shift unsorted element
        Employee *temp = current; // temp has the element that we want to move

        // current decrements to the insertion point
        // decrement current so that it does not equal current
        if (current->get_previous() != nullptr && temp->get_wages_due() > temp->get_previous()->get_wages_due())
            current = current->get_previous();
        // continue decrementing until we reach the appropriate insertion point
        while (current->get_previous() != nullptr && temp->get_wages_due() > current->get_wages_due())
            current = current->get_previous();

        // insert the employee node
        if (temp != current) {
            temp->get_previous()->set_next(temp->get_next()); // n - 1 >> n + 1, initial node position
            if (temp->get_next() != nullptr)
                temp->get_next()->set_previous(temp->get_previous()); // n - 1 << n + 1, initial node position

            if (temp->get_wages_due() >= current->get_wages_due()) { // left-most position, stopped at nullptr,
                temp->set_previous(current->get_previous()); // n >> n + 1
                temp->set_next(current);// n - 1 << n
                current->set_previous(temp); // n << n - 1
            }
            if (temp->get_wages_due() < current->get_wages_due()) {
                temp->set_previous(current); // n >> n + 1
                temp->set_next(current->get_next());// n - 1 << n
                current->get_next()->set_previous(temp); // n - 1 >> n
                current->set_next(temp); // n << n - 1
            }
        }

        if(isFinished) {
            current = outList->get_head();
            while (outList->get_head()->get_previous() != nullptr) {
                current = current->get_previous(); // find new head
                outList->set_head(current);
            }

            current = outList->get_tail();
            while (outList->get_tail()->get_next() != nullptr) {
                current = current->get_next(); //find new tail
                outList->set_tail(current);
            }
        }
            if (!isFinished)
                current = current_next; // increment marker
            else
                return;
    }
}


void writeToFile(EmployeeList* inList){
    ofstream outFile;
    string filename;

    cout << "Payroll information (sorted)" << endl;
    cout << "Enter destination file name: ";
    cin >> filename;

    outFile.open(filename);
    outFile << "*********Payroll Information********" << endl;
    Employee *iter;
    for (iter = inList->get_head(); iter != nullptr; iter = iter->get_next()){
        outFile << iter->get_name() << ", $" << iter->get_wages_due() << endl;
    }
    outFile << "*********End payroll**************" << endl;
    outFile.close();
}



int main() {
    ifstream userInput;
    EmployeeList *list = new EmployeeList;
    cout << "Employee file (open)" << endl;
    getFile(userInput);
    readEmployeesFile(userInput, list);

    cout << "Employee hours file (open)" << endl;
    getFile(userInput);
    readHoursFile(userInput, list);
    calculateWages(list);

    reverseSortByWages(list);
    writeToFile(list);

    Employee *iter;
    for (iter = list->get_head(); iter != nullptr; iter = iter->get_next())
        delete iter;

    delete list;

    return 0;
}
