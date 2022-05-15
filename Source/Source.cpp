// Benjamin Russell
// RussellMusicStore - Database manipulation demonstration for premade db of the same name. 
// Last updated: 2022/05/14

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include "sqlite3.h"

using namespace std;

void PrintMenu(int c);
void AddToStore(sqlite3* db);
void AddToEmployee(sqlite3* db);
void AddToCustomer(sqlite3* db);
void AddToProduct(sqlite3* db);
void AddToMusic(sqlite3* db);
void AddToInstrument(sqlite3* db);
sqlite3_stmt* RunQuery(sqlite3* db, string q);
bool Finalize(sqlite3* db, string q);
bool CheckIfValidCurrency(string const& input);

int main()
{
	sqlite3* mydb;
	int rc;
	// Open database 
	rc = sqlite3_open("RussellMusicStore.db", &mydb);
	if (rc) {
		cout << "Unable to open database: " << sqlite3_errmsg(mydb) << endl;
		return 0;
	}
	string uin;
	int choice, num;
	cout << "Welcome to Russell Music Store Database Management.\n";
	// Menu management, there's a few layers to this, managed through the various PrintMenu switch options.
	while (1) {
		// Adding data to one of the 6 tables that lack major FK dependencies. 
		cout << "Please select an option, or type -1 to quit.\n";
		PrintMenu(0);
		getline(cin, uin);
		try { choice = stoi(uin); }
		catch (...) { choice = 0; }
		switch (choice) {
		case 1:
			PrintMenu(1);
			while (1) {
				getline(cin, uin);
				if (uin == "-1") { break; }
				try { num = stoi(uin); }
				catch (...) { num = 0; }
				if (num > 0 && num <= 6) { 
					switch (num) {
					case 1: AddToStore(mydb); break;
					case 2: AddToEmployee(mydb); break;
					case 3: AddToCustomer(mydb); break;
					case 4: AddToProduct(mydb); break;
					case 5: AddToMusic(mydb); break;
					case 6: AddToInstrument(mydb);
					}
					break;
				}
				else { cout << "Invalid choice. Please try again.\n"; }
			}
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case -1:
			return 0;
		default:
			cout << "Command not recognized. Please try again.\n";
			break;
		}
	}
}

void PrintMenu(int c) {
	switch (c) {
	case 0: // Main menu.
		cout << "1. Add data to table.\n";
		cout << "2. Update data in table.\n";
		cout << "3. Delete data from table.\n";
		cout << "4. View information in table.\n";
		cout << "5. Create new Sales entry.\n";
		cout << "6. Create new Rentals entry.\n";
		cout << "7. Create new Services entry.\n";
		break;
	case 1: // Add to table.
		cout << "Please select which table you would like to add data to. Type -1 to cancel.\n";
		cout << "NOTE: Tables not listed can be added to elsewhere.\n";
		cout << "1. Store\n";
		cout << "2. Employee\n";
		cout << "3. Customer\n";
		cout << "4. Product\n";
		cout << "5. Music\n";
		cout << "6. Instrument\n";
		break;
	}

	
}

void AddToStore(sqlite3* db) {
	string sname, smail, sphone, sadd;

	cout << "Creating a new store. At any point you may type -1 to cancel.\n";
	cout << "Please type the name of the new store.\n";
	getline(cin, sname);
	if (sname == "-1") { return; }
	cout << "Please type the email address of the new store.\n";
	getline(cin, smail);
	if (smail == "-1") { return; }
	cout << "Please type the phone number of the new store.\n";
	getline(cin, sphone);
	if (sphone == "-1") { return; }
	cout << "Please type the address of the new store.\n";
	getline(cin, sadd);
	if (sadd == "-1") { return; }

	cout << "Starting Store add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating store. . .\n";
	string query = "INSERT INTO STORE (STORE_NAME, STORE_EMAIL, STORE_PHONE, STORE_ADDRESS) ";
	query += "VALUES ('" + sname + "', "; // STORE_NAME
	query += "'" + smail + "', "; // STORE_EMAIL
	query += "'" + sphone + "', "; // STORE_PHONE
	query += "'" + sadd + "');"; // STORE_ADDRESS
	Finalize(db, query);
}

void AddToEmployee(sqlite3* db) {
	string efname, elname, email, ephone, store;
	bool hasmail, hasphone;
	vector<string> stores; // STORE_ID may be automatically generated, but it's not predictable enough to hard code parameters. 

	string query = "SELECT STORE_ID FROM STORE";
	sqlite3_stmt* ate = RunQuery(db, query);
	if (ate != nullptr) {
		while (sqlite3_step(ate) == SQLITE_ROW) {
			stores.push_back(reinterpret_cast<const char*>(sqlite3_column_text(ate, 0)));
		}
	}
	else { return; }
	sqlite3_finalize(ate);
	query.clear();

	cout << "Creating a new employee. At any point you may type -1 to cancel.\n";
	cout << "Please type the new employee's first name.\n";
	getline(cin, efname);
	if (efname == "-1") { return; }
	cout << "Please type the new employee's last name.\n";
	getline(cin, elname);
	if (elname == "-1") { return; }
	cout << "Please type the new employee's email. If they don't have one, type \"none\".\n";
	getline(cin, email);
	if (email == "-1") { return; }
	if (email == "none") { hasmail = false; }
	else { hasmail = true; }
	cout << "Please type the new employee's phone number. If they don't have one, type \"none\".\n";
	getline(cin, ephone);
	if (ephone == "-1") { return; }
	if (ephone == "none") { hasphone = false; }
	else { hasphone = true; }
	cout << "Please type the Store ID of the store this employee works at.\n";
	while (1) {
		getline(cin, store);
		if (store == "-1") { return; }
		if (count(stores.begin(), stores.end(), store)) { break; }
		else { cout << "Invalid store ID. Please try again.\n"; }
	}

	cout << "Starting Employee add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating employee. . .\n";
	query = "INSERT INTO EMPLOYEE (STORE_ID, EMPLOYEE_FIRSTNAME, EMPLOYEE_LASTNAME, EMPLOYEE_EMAIL, EMPLOYEE_PHONE) ";
	query += "VALUES (" + store + ", "; // STORE_ID
	query += "'" + efname + "', "; // EMPLOYEE_FIRSTNAME
	query += "'" + elname + "', "; // EMPLOYEE_LASTNAME
	if (hasmail) { query += "'" + email + "', "; } // EMPLOYEE_EMAIL
	else { query += "NULL, "; } // Insert null if hasmail is false.
	if (hasphone) { query += "'" + ephone + "); "; } // EMPLOYEE_PHONE
	else { query += "NULL);"; } // Insert null if hasphone is false.
	Finalize(db, query);
}

void AddToCustomer(sqlite3* db) {
	string cfname, clname, cadd, cphone, ctype;
	bool hasadd;
	vector<string> types;

	// Get all available customer types (FK). 
	string query = "SELECT CUSTOMER_TYPE FROM CUST_TYPE";
	sqlite3_stmt* atc = RunQuery(db, query);
	if (atc != nullptr) {
		while (sqlite3_step(atc) == SQLITE_ROW) {
			types.push_back(reinterpret_cast<const char*>(sqlite3_column_text(atc, 0)));
		}
	}
	else { return; }
	sqlite3_finalize(atc);
	query.clear();

	cout << "Creating a new customer. At any point you may type -1 to cancel.\n";
	cout << "Please type the new customer's first name.\n";
	getline(cin, cfname);
	if (cfname == "-1") { return; }
	cout << "Please type the new customer's last name.\n";
	getline(cin, clname);
	if (clname == "-1") { return; }
	cout << "Please type the new customer's address. If they don't have one, type \"none\".\n";
	getline(cin, cadd);
	if (cadd == "-1") { return; }
	if (cadd == "none") { hasadd = false; }
	else { hasadd = true; }
	cout << "Please type the customer's phone number. This is mandatory.\n";
	getline(cin, cphone);
	if (cphone == "-1") { return; }
	cout << "Please indicate the customer's type by selecting a number from the following list:\n";
	for (int i = 0; i < types.size(); ++i) { cout << i + 1 << ". " << types[i] << endl; }
	int num;
	while (1) {
		getline(cin, ctype);
		try { num = stoi(ctype); }
		catch (...) { num = -1; }
		if (num > 0 && num <= types.size()) { 
			ctype = types[num - 1]; 
			break;
		}
		else { cout << "Invalid choice. Please try again.\n"; }
	}
	
	cout << "Starting Customer add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating customer. . .\n";
	query = "INSERT INTO CUSTOMER (CUSTOMER_FIRSTNAME, CUSTOMER_LASTNAME, CUSTOMER_ADDRESS, CUSTOMER_PHONE, CUSTOMER_TYPE) ";
	query += "VALUES ('" + cfname + "', "; // CUSTOMER_FIRSTNAME
	query += "'" + clname + "', "; // CUSTOMER_LASTNAME
	if (hasadd) { query += "'" + cadd + "', "; } // CUSTOMER_ADDRESS
	else { query += "NULL, "; }  // Insert null if hasadd is false.
	query += "'" + cphone + "', "; // CUSTOMER_PHONE
	query += "'" + ctype + "');"; // CUSTOMER_TYPE
	Finalize(db, query);
}

void AddToProduct(sqlite3* db) {
	string pname, pprice, pbrand, pstock;
	cout << "Creating a new product. At any point you may type -1 to cancel.\n";
	cout << "Please type the new product's name.\n";
	getline(cin, pname);
	if (pname == "-1") { return; }
	cout << "Please type the new product's brand.\n";
	getline(cin, pbrand);
	if (pbrand == "-1") { return; }
	cout << "Please type the new product's price. The currency symbol is not necessary.\n";
	while (1) {
		getline(cin, pprice);
		if (pprice == "-1") { return; }
		if (CheckIfValidCurrency(pprice)) { break; }
		else { cout << "Invalid price. Please try again.\n"; }
	}
	cout << "Please type the new product's stock levels.\n";
	int num;
	while (1) {
		getline(cin, pstock);
		try { num = stoi(pstock); }
		catch (...) { num = -1; }
		if (num >= 0) { break; }
		else { cout << "Invalid number. Please try again.\n"; }
	}

	cout << "Starting Product add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating product. . .\n";
	string query = "INSERT INTO PRODUCT (PRODUCT_NAME, PRODUCT_PRICE, PRODUCT_BRAND, PRODUCT_STOCK) ";
	query += "VALUES ('" + pname + "', ";
	query += pprice + ", ";
	query += "'" + pbrand + "', ";
	query += pstock + ");";
	Finalize(db, query);
}

void AddToMusic(sqlite3* db) {
	string mname, mprice, mstock, martist, mgenre, mrelease, mtype;

}

void AddToInstrument(sqlite3* db) {

}

sqlite3_stmt* RunQuery(sqlite3* db, string q) {
	sqlite3_stmt* s;
	if (sqlite3_prepare_v2(db, q.c_str(), -1, &s, NULL) != SQLITE_OK)
	{
		string m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(s);
		cout << "There was an error: " << m_strLastError << endl << endl;
		return nullptr;
	}
	else { return s; }
}

bool Finalize(sqlite3* db, string q) {
	// Bool is to return status in cases where finalize occurs before end of function. 
	// Otherwise this can be run without needing to use the returned value.
	cout << "Finalizing. . .\n";
	if (sqlite3_exec(db, q.c_str(), NULL, NULL, NULL) != SQLITE_OK)
	{
		string m_strLastError = sqlite3_errmsg(db);
		cout << "There was an error: " << m_strLastError << endl;
		cout << "Aborting operation.\n\n";
		sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
		return false;
	}
	else { 
		cout << "Operation complete! Returning to main menu.\n"; 
		sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
		return true;
	}
	
}

bool CheckIfValidCurrency(string const& input) {
	regex validCurrency("([0-9]{0,4})\.([0-9]{0,2})"); // Lazy
	return regex_match(input, validCurrency);
}