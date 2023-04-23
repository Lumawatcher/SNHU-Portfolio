#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

//VULNERABILITY - client list is hard coded. This can/should be abstracted, and client elections should not be obtainable through reverse engineering
//SOLUTION - Client list should be encrpyted and stored off site, and decrypted using a key provided by the user on this system (like their password)
string name1 = "Bob Jones", name2 = "Sarah Davis", name3 = "Amy Friendly", name4 = "Johnny Smith", name5 = "Carol Spears";
string num1 = "1", num2 = "2", num3 = "1", num4 = "1", num5 = "2";


bool CheckUserPermission();
void DisplayInfo();
void ChangeCustomerChoice();

int main() {
	char choice;
	bool answer = false;
	int logInAttempts = 0;
	//VULNERABILITY - no log in limit means you can just run the program a billion times to find the password and username
	//SOLUTION - a log in limit integer variable was added and is incremented and checked after each log in attempt. A limit of 5 attempts was set, after which the process exits with -1
	while (!answer) {
		answer = CheckUserPermission();
		logInAttempts++;
		if (!answer && (logInAttempts >= 4)) {
			cout << "Failed log in limit reached, please contact your system administrator" << endl;
			return -1;
		}
	}
	logInAttempts = 0;
	cout << "CS410 Project, code reverse engineered by Jared Smith " << endl
		<< "Hello, Welcome to Our Investment Company" << endl;


	while (true) {
		cout << "What would you like to do?" << endl
			<< "DISPLAY the client list (enter 1)" << endl
			<< "CHANGE a client's choice (enter 2)" << endl
			<< "EXIT the program (enter 3)" << endl;
		//VULNERABILITY - no input validation, accepts any input to var
		//OLD CODE - cin >> choice;
		//SOLUTION choice is changed to a char, and retrieved using buffer safe getchar() method
		choice = getchar();
		cout << "You chose " << choice << endl;

		if (choice == '1') DisplayInfo();

		else if (choice == '2') ChangeCustomerChoice();

		else if (choice == '3') break;

	}
	return 0;
}



bool CheckUserPermission() {
	//VULNERABILITY - no input validation for username and password
	//OLD CODE cin >> username
	//SOLUTION - set a character limit for the username and password of 20 (the 21st element is the terminator)
	string username, password;
	cout << "Enter Username: " << endl;
	cin >> setw(21) >> username;
	cout << "Enter Password: " << endl;
	cin >> setw(21) >>  password;
	//VULNERABILITY - hardcoded U and PW that can be (and were) discovered through software reverse engineering
	/*SOLUTION - assymetric encryption should be implemented, and values passed to the usernameand password variables should be
	* encrypted and sent off site to be verified against a secure database. The encrypted password variable will be stored
	* for use in decrpyting the client list	*/

	if ((username == "UUU") && (password == "123")) return true;

	else {
		cout << "Invalid Password. Please try again" << endl;
		return false;
	}

}

void DisplayInfo() {
	cout << "Client's Name    Service Selected (1 = Brokerage, 2 = Retirement)" << endl
		<< "1. " << name1 << " selected option " << num1 << endl
		<< "2. " << name2 << " selected option " << num2 << endl
		<< "3. " << name3 << " selected option " << num3 << endl
		<< "4. " << name4 << " selected option " << num4 << endl
		<< "5. " << name5 << " selected option " << num5 << endl;
	return;

}

void ChangeCustomerChoice() {
	char changechoice, newservice;
	cout << "Enter the number of the client that you wish to change" << endl;
	//VULNERABILITY no input validation means vars can be any combination of characters and numbers
	//OLD CODE cin >> changechoice; (SOLUTION below under the next comment block)
	changechoice = getchar();
	cout << "Please enter the client's new service choice (1 = Brokerage, 2 = Retirement)" << endl;
	//OLD CODE cin >> newservice
	//VULNERABILITY no input validation means vars can be any combination of characters and numbers
	//SOLUTION both changechoice and newservice are changed to chars, and retrieved using buffer safe getchar() method
	newservice = getchar();

	if (changechoice == '1') num1 = newservice;
	if (changechoice == '2') num2 = newservice;
	if (changechoice == '3') num3 = newservice;
	if (changechoice == '4') num4 = newservice;
	if (changechoice == '5') num5 = newservice;
	return;
}