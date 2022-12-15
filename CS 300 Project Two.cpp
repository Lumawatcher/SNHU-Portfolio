#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;



//structure used for storing course information
struct Course {
	string courseId;
	string courseName;
	string prereqOne;
	string prereqTwo;

	//initialized for empty course objects
	Course() {
		courseId = "";
		prereqOne = "";
		prereqTwo = "";
	}

	Course(string Id, string name) {
		courseId = Id;
		courseName = name;
		prereqOne = "";
		prereqTwo = "";
	}
};

//BST Node definition
struct Node {
	Course course;
	Node* left;
	Node* right;


	//default constructor
	Node() {
		left = nullptr;
		right = nullptr;
	}

	//Destructor
	~Node() {
		delete left;
		delete right;
	}


	//overloaded constructor with course object
	Node(Course newCourse) {
		course = newCourse;
		left = nullptr;
		right = nullptr;
	}

};


// BST definition
class BinarySearchTree {
private:
	Node* root;

	void addNode(Node* node, Course course);
	void inOrder(Node* node);
	void printCourse(Course course);
	
public:
	BinarySearchTree();
	~BinarySearchTree();
	void search(string courseId);
	void inOrder();
	void insert(Course course);
};

/*
* default constructor
*/
BinarySearchTree::BinarySearchTree() {
	root = nullptr;
}

/*
* Destructor
*/
BinarySearchTree::~BinarySearchTree() {
	delete root;
}



/*
* adds a bid to first available node
* @param node is the current node in the tree
* @param course is the course object to be added to the tree
*/
void BinarySearchTree::addNode(Node* node, Course course) {
	Node* currentNode = node;

	//If the new courseID is less than our current course ID, checks for empty left pointer. 
	if (course.courseId < currentNode->course.courseId) {
		//if empty left pointer is found, assigns left pointer with new node and returns
		if (currentNode->left == nullptr) {
			Node* newNode;
			newNode = new Node(course);
			currentNode->left = newNode;
			return;
		}

		//if empty left pointer is not found, recursively calls addNode() until an empty node is found
		else {
			currentNode = currentNode->left;
			addNode(currentNode, course);
		}
	}


	//If the new courseID is greater than our current course ID, checks for empty left pointer. 
	if (course.courseId > currentNode->course.courseId) {
		//if empty right pointer is found, assigns right pointer with new node and returns
		if (currentNode->right == nullptr) {
			Node* newNode;
			newNode = new Node(course);
			currentNode->right = newNode;
			return;
		}

		//if empty right pointer is not found, recursively calls addNode() until an empty node is found
		else {
			currentNode = currentNode->right;
			addNode(currentNode, course);
		}
	}
	
	//for closing recursive calls
	return;
}

/*
* Function for printing nodes from least to greatest
* @param node is the current node we're checking, starts from root using public function call
*/
void BinarySearchTree::inOrder(Node* node) {
	//Checks that we passed it a valid object
	if (node != nullptr) {
		//recursively calls down left side
		inOrder(node->left);

		//prints current node
		printCourse(node->course);

		//Recursively calls down right side 
		inOrder(node->right);
	}

	//for closing recursive calls
	return;
}

/*
* for printing course information consistently
*/
void BinarySearchTree::printCourse(Course course) {
	cout << course.courseId << ", " << course.courseName << endl;
}

/*
* Used to search for a node via courseId, prints result
*/
void BinarySearchTree::search(string courseId) {
	Node* currentNode = root;

	while (currentNode != nullptr) {
		if (currentNode->course.courseId == courseId) {
			printCourse(currentNode->course);
			//checks for and prints prerequisites
			if (currentNode->course.prereqOne != "") {
				cout << "Prerequisites: " << currentNode->course.prereqOne;
				if (currentNode->course.prereqTwo != "") {
					cout << ", " << currentNode->course.prereqTwo;
				}
				cout << endl;
			}
			return;
		}

		else if (courseId > currentNode->course.courseId) {
			currentNode = currentNode->right;
		}

		else {
			currentNode = currentNode->left;
		}
	}

	//If no match is found, prints an error message
	cout << "No matching course found" << endl;
	return ;
}

/*
* Public Function Call, allows recursive calls
*/
void BinarySearchTree::inOrder() {
	inOrder(root);
}

/*
* Public function call, checks for null root
*/
void BinarySearchTree::insert(Course course) {
	if (root == nullptr) {
		root = new Node(course);
		return;
	}

	else {
		addNode(root, course);
	}

	return;
}

/*
* For parsing csv file
*/
void CSVParse(BinarySearchTree* bst, string fileName) {
	fstream fin;
	fin.open(fileName, ios::in);
	vector<string> row;
	string line, word;
	Course newCourse;

	//Iterates until the end of the file
	while (!fin.eof()) {
		//clears the vector for previous lines
		row.clear();

		//gets the next whole line
		getline(fin, line);
		
		//adds line to string stream so it can be parsed
		stringstream stream(line);

		//iterates until end of line, adds each word in the line to the row vector
		while (getline(stream, word, ',')) {
			row.push_back(word);
		}

		//checks row size, generates objects and adds prereqs as appropriate
		if (row.size() >= 2){
			newCourse = Course(row[0], row[1]);
		}

		if (row.size() == 3) {
			newCourse.prereqOne = row[2];
		}

		else if (row.size() == 4) {
			newCourse.prereqOne = row[2];
			newCourse.prereqTwo = row[3];
		}
		bst->insert(newCourse);
	}

	fin.close();
	return;
}



/*
* main() function
*/
int main() {
	//Declaring new BST to hold course objects.
	BinarySearchTree* bst;
	bst = new BinarySearchTree();
	
	//declaring the csvPath and current courseID
	string csvPath, courseId;
	cout << "Welcome to the course planner." << endl
		<< "Please input the file name that contains the course data." << endl;
	cin >> csvPath;


	//Menu starts here
	int choice = 0;
	while (choice != 9) {

		cout << "1. Load Data Structure." << endl
			<< "2. Print Course List." << endl
			<< "3. Print Course" << endl
			<< "9. Exit" << endl << endl
			<< "What would you like to do?" << endl;
		cin >> choice;

		
		switch (choice) {
		case 1:
			//clears bst and populates with list
			bst->~BinarySearchTree();
			bst = new BinarySearchTree();
			CSVParse(bst, csvPath);
			break;

		case 2:
			//Prints course list alphabetically by course id.
			bst->inOrder();
			break;

		case 3:
			//prompt course ID to search for, call search(course ID), print course
			cout << "which course would you like to print?" << endl;
			cin >> courseId;
			bst->search(courseId);
			break;

		
		case 9:
			//For exiting
			break;

		default:
			cout << choice << " is not a valid selection." << endl;
			break;

		}
	}


	return 0;
}