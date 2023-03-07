#include <Python.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <cmath>
#include <string>

using namespace std;


/*
Description:
	To call this function, simply pass the function name in Python that you wish to call.
Example:
	callProcedure("printsomething");
Output:
	Python will print on the screen: Hello from python!
Return:
	None
*/
void callProcedure(string pName)
{
	char* procname = new char[pName.length() + 1];
	std::strcpy(procname, pName.c_str());

	Py_Initialize();
	PyObject* my_module = PyImport_ImportModule("PythonCode");
	PyErr_Print();
	PyObject* my_function = PyObject_GetAttrString(my_module, procname);
	PyObject* my_result = PyObject_CallObject(my_function, NULL);
	Py_Finalize();

	delete[] procname;
}


/*
Description:
	To call this function, pass the name of the Python functino you wish to call and the string parameter you want to send
Example:
	int x = callIntFunc("PrintMe","Test");
Output:
	Python will print on the screen:
		You sent me: Test
Return:
	100 is returned to the C++
*/
int callIntFunc(string proc, string param)
{
	char* procname = new char[proc.length() + 1];
	std::strcpy(procname, proc.c_str());

	char* paramval = new char[param.length() + 1];
	std::strcpy(paramval, param.c_str());


	PyObject* pName, * pModule, * pDict, * pFunc, * pValue = nullptr, * presult = nullptr;
	// Initialize the Python Interpreter
	Py_Initialize();
	// Build the name object
	pName = PyUnicode_FromString((char*)"PythonCode");
	// Load the module object
	pModule = PyImport_Import(pName);
	// pDict is a borrowed reference 
	pDict = PyModule_GetDict(pModule);
	// pFunc is also a borrowed reference 
	pFunc = PyDict_GetItemString(pDict, procname);
	if (PyCallable_Check(pFunc))
	{
		pValue = Py_BuildValue("(z)", paramval);
		PyErr_Print();
		presult = PyObject_CallObject(pFunc, pValue);
		PyErr_Print();
	}
	else
	{
		PyErr_Print();
	}
	//printf("Result is %d\n", _PyLong_AsInt(presult));
	Py_DECREF(pValue);
	// Clean up
	Py_DECREF(pModule);
	Py_DECREF(pName);
	// Finish the Python Interpreter
	Py_Finalize();

	// clean 
	delete[] procname;
	delete[] paramval;


	return _PyLong_AsInt(presult);
}

/*
Description:
	To call this function, pass the name of the Python function you wish to call and the string parameter you want to send
Example:
	int x = callIntFunc("doublevalue",5);
Return:
	25 is returned to the C++
*/
int callIntFunc(string proc, int param)
{
	char* procname = new char[proc.length() + 1];
	std::strcpy(procname, proc.c_str());

	PyObject* pName, * pModule, * pDict, * pFunc, * pValue = nullptr, * presult = nullptr;
	// Initialize the Python Interpreter
	Py_Initialize();
	// Build the name object
	pName = PyUnicode_FromString((char*)"PythonCode");
	// Load the module object
	pModule = PyImport_Import(pName);
	// pDict is a borrowed reference 
	pDict = PyModule_GetDict(pModule);
	// pFunc is also a borrowed reference 
	pFunc = PyDict_GetItemString(pDict, procname);
	if (PyCallable_Check(pFunc))
	{
		pValue = Py_BuildValue("(i)", param);
		PyErr_Print();
		presult = PyObject_CallObject(pFunc, pValue);
		PyErr_Print();
	}
	else
	{
		PyErr_Print();
	}
	//printf("Result is %d\n", _PyLong_AsInt(presult));
	Py_DECREF(pValue);
	// Clean up
	Py_DECREF(pModule);
	Py_DECREF(pName);
	// Finish the Python Interpreter
	Py_Finalize();

	// clean 
	delete[] procname;

	return _PyLong_AsInt(presult);
}

/*
Description:
	Provides input validation and prints menu, accepting only integers 1 through 4.
*/
int printMenu()
{
	int menuChoice;

	while (true)
	{
		cout << "1: Display purchase list" << endl
			<< "2: Display sale count of a specific item" << endl
			<< "3: Display item purchases histogram" << endl
			<< "4: Quit " << endl;
		if ((cin >> menuChoice) && (menuChoice > 0) && (menuChoice < 5))
		{
			break;
		}
		else
		{
			cout << "Input not valid, please enter a numnber between 1 and 4" << endl;
			cin.clear();
		}
	}
	return menuChoice;
}

/*
Description:
	Gets single item, confirms choice with user. Then calls the Python method that returns the number of that item sold (not case sensitive)
*/
void getSingleItem()
{
	string item;
	int itemNum, confirm;
	//loop runs until they enter an item name and confirm it's the item they want to search for
	while (true)
	{
		cout << "Which item would you like to count?" << endl
			<< "Enter a single word name" << endl;
		cin >> item;
		cout << "Check the number of sales of " << item << "?" << endl
			<< "Enter 1 for yes, 2 to enter a different item name" << endl;
		//loop runs until they provide a confirmation 1, or 2
		while (true)
		{
			if ((cin >> confirm) && ((confirm == 1) || (confirm == 2)))
			{
				break;
			}
			else
			{
				cout << "Confirmation not recognized" << endl
					<< "Please enter 1 to check the number of sales of " << item << endl
					<< "Or enter 2 to enter a different item name" << endl;
				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
		if (confirm == 1)
		{
			break;
		}

	}
	itemNum = callIntFunc("getSingleItem", item);
	if (itemNum == 0)
	{
		cout << "item not found on sales list" << endl;
	}
	else
	{
		cout << itemNum << " units of " << item << " sold." << endl;
	}
	return;
}

void printHistogram() 
{
	ifstream inFS;
	string fileName;
	int fileNum;
	inFS.open("frequency.dat");
	//validates file opened correctly
	if (!inFS.is_open()) {
		cout << "Failed to open frequency.dat" << endl;
		return;
	}
	//runs until end of file
	while (!inFS.eof())
	{
		inFS >> fileName;
		inFS >> fileNum;
		cout << fileName << " ";
		//prints an * for each item sold according to frequency.dat
		for (int i = 0; i < fileNum; ++i)
		{
			cout << "*";
		
		}
		cout << endl;
	
	}
	inFS.close();
	return;
}



int main()
{
	bool keepRunning = true;
	int switchController;
	while (keepRunning)
	{
		switchController = printMenu();
		switch (switchController)
		{
		case 1:
			callProcedure("getAllItems");
			break;
		case 2:
			getSingleItem();
			break;
		case 3:
			callProcedure("writeFrequencyFile");
			printHistogram();
			break;
		case 4:
			keepRunning = false;
			break;
		default:
			break;

		}
	}

	return 0;
}