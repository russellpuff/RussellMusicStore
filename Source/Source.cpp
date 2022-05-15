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
void UpdateTable(sqlite3* db, int u);
void DeleteTableEntry(sqlite3*, int d);
sqlite3_stmt* RunQuery(sqlite3* db, string q);
vector<string> MakeVector(sqlite3* db, string& q, sqlite3_stmt* s);
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
		case 1: // Add to table
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
		case 2: // Update table
			PrintMenu(2);
			while (1) {
				getline(cin, uin);
				if (uin == "-1") { break; }
				try { num = stoi(uin); }
				catch (...) { num = 0; }
				if (num > 0 && num <= 9) {
					UpdateTable(mydb, num);
					break;
				}	
				else { cout << "Invalid choice. Please try again.\n"; }
			}
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
	string tables[9] = { "1. Store", "2. Employee", "3. Customer", "4. Product", "5. Music", "6. Instrument", "7. Sales", "8. Rentals", "9. Services" };
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
		for (int i = 0; i <= 5; ++i) { cout << tables[i] << endl; }
		break;
	case 2: // Update table.
		cout << "Please select which table you would like to update the data of. Type -1 to cancel.\n";
		for (string s : tables) { cout << s << endl; }
		break;
	case 3: // Delete data from table.
		cout << "Please select which table you would like to delete data from. Type -1 to cancel.\n";
		for (string s : tables) { cout << s << endl; }
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
	stores = MakeVector(db, query, ate);
	if (stores.size() == 0) { return; }
	sqlite3_finalize(ate);

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
	vector<string> custtypes;

	// Get all available customer types (FK). 
	string query = "SELECT CUSTOMER_TYPE FROM CUST_TYPE";
	sqlite3_stmt* atc = RunQuery(db, query);
	custtypes = MakeVector(db, query, atc);
	if (custtypes.size() == 0) { return; }
	sqlite3_finalize(atc);

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
	for (int i = 0; i < custtypes.size(); ++i) { cout << i + 1 << ". " << custtypes[i] << endl; }
	int num;
	while (1) {
		getline(cin, ctype);
		if (ctype == "-1") { return; }
		try { num = stoi(ctype); }
		catch (...) { num = -1; }
		if (num > 0 && num <= custtypes.size()) { 
			ctype = custtypes[num - 1]; 
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
		if (pstock == "-1") { return; }
		try { num = stoi(pstock); }
		catch (...) { num = -1; }
		if (num >= 0) { break; }
		else { cout << "Invalid number. Please try again.\n"; }
	}

	cout << "Starting Product add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating product. . .\n";
	string query = "INSERT INTO PRODUCT (PRODUCT_NAME, PRODUCT_PRICE, PRODUCT_BRAND, PRODUCT_STOCK) ";
	query += "VALUES ('" + pname + "', "; // PRODUCT_NAME
	query += pprice + ", "; // PRODUCT_PRICE
	query += "'" + pbrand + "', "; // PRODUCT_BRAND
	query += pstock + ");"; // PRODUCT_STOCK
	Finalize(db, query);
}

void AddToMusic(sqlite3* db) {
	string mname, mprice, mstock, martist, mgenre, mrelease, mtype;
	vector<string> mustypes;

	// Get available music types (FK)
	string query = "SELECT MUSIC_TYPE FROM MUS_TYPE";
	sqlite3_stmt* atm = RunQuery(db, query);
	mustypes = MakeVector(db, query, atm);
	if (mustypes.size() == 0) { return; }
	sqlite3_finalize(atm);

	cout << "Creating a new music product. At any point you may type -1 to cancel.\n";
	cout << "Please type the new music product's name.\n";
	getline(cin, mname);
	if (mname == "-1") { return; }
	cout << "Please type the new music product's artist/composer.\n";
	getline(cin, martist);
	if (martist == "-1") { return; }
	cout << "Please type the new music product's genre.\n";
	getline(cin, mgenre);
	if (mgenre == "-1") { return; }
	cout << "Please type the new music product's release date.\n";
	getline(cin, mrelease);
	if (mrelease == "-1") { return; }
	cout << "Please type the new music product's price. The currency symbol is not necessary.\n";
	while (1) {
		getline(cin, mprice);
		if (mprice == "-1") { return; }
		if (CheckIfValidCurrency(mprice)) { break; }
		else { cout << "Invalid price. Please try again.\n"; }
	}
	cout << "Please type the new music product's stock levels.\n";
	int num;
	while (1) {
		getline(cin, mstock);
		if (mstock == "-1") { return; }
		try { num = stoi(mstock); }
		catch (...) { num = -1; }
		if (num >= 0) { break; }
		else { cout << "Invalid number. Please try again.\n"; }
	}
	cout << "Please indicate the music's type by selecting a number from the following list:\n";
	for (int i = 0; i < mustypes.size(); ++i) { cout << i + 1 << ". " << mustypes[i] << endl; }
	while (1) {
		getline(cin, mtype);
		if (mtype == "-1") { return; }
		try { num = stoi(mtype); }
		catch (...) { num = -1; }
		if (num > 0 && num <= mustypes.size()) {
			mtype = mustypes[num - 1];
			break;
		}
		else { cout << "Invalid choice. Please try again.\n"; }
	}

	cout << "Starting Music add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating music. . .\n";
	query = "INSERT INTO MUSIC (PRODUCT_NAME, PRODUCT_PRICE, PRODUCT_STOCK, MUSIC_ARTIST, MUSIC_GENRE, MUSIC_RELEASEDATE, MUSIC_TYPE) ";
	query += "VALUES ('" + mname + "', "; // PRODUCT_NAME
	query += mprice + ", "; // PRODUCT_PRICE
	query += mstock + ", "; // PRODUCT_STOCK
	query += "'" + martist + "', "; // MUSIC_ARTIST
	query += "'" + mgenre + "', "; // MUSIC_GENRE
	query += "'" + mrelease + "', "; // MUSIC_RELEASEDATE
	query += "'" + mtype + "');"; // MUSIC_TYPE
	Finalize(db, query);
}

void AddToInstrument(sqlite3* db) {
	string iname, iprice, ibrand, istock, itype, icond;
	vector<string> insttypes, instconds;

	// Get available instrument conditions (FK)
	string query = "SELECT INSTRUMENT_CONDITION FROM CONDITION";
	sqlite3_stmt* ati = RunQuery(db, query);
	instconds = MakeVector(db, query, ati);
	if (instconds.size() == 0) { return; }

	// Get available instrument types (FK)
	query = "SELECT INSTRUMENT_TYPE FROM INST_TYPE";
	ati = RunQuery(db, query);
	insttypes = MakeVector(db, query, ati);
	if (insttypes.size() == 0) { return; }
	sqlite3_finalize(ati);

	cout << "Creating a new instrument. At any point you may type -1 to cancel.\n";
	cout << "Please type the new instrument's name.\n";
	getline(cin, iname);
	if (iname == "-1") { return; }
	cout << "Please type the new instrument's brand.\n";
	getline(cin, ibrand);
	if (ibrand == "-1") { return; }
	cout << "Please type the new instrument's price. The currency symbol is not necessary.\n";
	while (1) {
		getline(cin, iprice);
		if (iprice == "-1") { return; }
		if (CheckIfValidCurrency(iprice)) { break; }
		else { cout << "Invalid price. Please try again.\n"; }
	}
	cout << "Please type the new music product's stock levels.\n";
	int num;
	while (1) {
		getline(cin, istock);
		if (istock == "-1") { return; }
		try { num = stoi(istock); }
		catch (...) { num = -1; }
		if (num >= 0) { break; }
		else { cout << "Invalid number. Please try again.\n"; }
	}
	cout << "Please indicate the instrument's type by selecting a number from the following list:\n";
	for (int i = 0; i < insttypes.size(); ++i) { cout << i + 1 << ". " << insttypes[i] << endl; }
	while (1) {
		getline(cin, itype);
		if (itype == "-1") { return; }
		try { num = stoi(itype); }
		catch (...) { num = -1; }
		if (num > 0 && num <= insttypes.size()) {
			itype = insttypes[num - 1];
			break;
		}
		else { cout << "Invalid choice. Please try again.\n"; }
	}
	cout << "Please indicate the instrument's condition by selecting a number from the following list:\n";
	for (int i = 0; i < instconds.size(); ++i) { cout << i + 1 << ". " << instconds[i] << endl; }
	while (1) {
		getline(cin, icond);
		if (icond == "-1") { return; }
		try { num = stoi(icond); }
		catch (...) { num = -1; }
		if (num > 0 && num <= instconds.size()) {
			icond = instconds[num - 1];
			break;
		}
		else { cout << "Invalid choice. Please try again.\n"; }
	}

	cout << "Starting Instrument add process.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Creating instrument. . .\n";
	query = "INSERT INTO INSTRUMENT (PRODUCT_NAME, PRODUCT_PRICE, PRODUCT_BRAND, PRODUCT_STOCK, INSTRUMENT_TYPE, INSTRUMENT_CONDITION) ";
	query += "VALUES ('" + iname + "', "; // PRODUCT_NAME
	query += iprice + ", "; // PRODUCT_PRICE
	query += "'" + ibrand + "', "; // PRODUCT_BRAND
	query += istock + ", "; // PRODUCT_STOCK
	query += "'" + itype + "', "; // INSTRUMENT_TYPE
	query += "'" + icond + "');"; // INSTRUMENT_CONDITION
	Finalize(db, query);
}

void UpdateTable(sqlite3* db, int u)
{
	string what, pkin, uin, update, pk; // what = the table being edited, pkin = the id of the table being edited, pk = name of that table's primary key
	vector<string> pks; // Collection of valid pks from table.
	vector<string> atts; // Attributes that can be edited.

	// Switch statement prepares table-specific terminology for sql statement. 
	switch (u) {
	case 1: // Store
		what = "STORE";
		atts = { "STORE_NAME", "STORE_EMAIL", "STORE_PHONE", "STORE_ADDRESS" };
		pk = "STORE_ID";
		break;
	case 2: // Employee
		what = "EMPLOYEE";
		atts = { "STORE_ID", "EMPLOYEE_FIRSTNAME", "EMPLOYEE_LASTNAME", "EMPLOYEE_EMAIL" ,"EMPLOYEE_PHONE" };
		pk = "EMPLOYEE_ID";
		break;
	case 3: // Customer
		what = "CUSTOMER";
		atts = { "CUSTOMER_FIRSTNAME", "CUSTOMER_LASTNAME", "CUSTOMER_ADDRESS", "CUSTOMER_PHONE", "CUSTOMER_TYPE" };
		pk = "CUSTOMER_ID";
		break;
	case 4: // Product
		what = "PRODUCT";
		atts = { "PRODUCT_NAME", "PRODUCT_PRICE", "PRODUCT_BRAND", "PRODUCT_STOCK" };
		pk = "PRODUCT_ID";
		break;
	case 5: // Music
		what = "MUSIC";
		atts = { "PRODUCT_NAME", "PRODUCT_PRICE", "PRODUCT_STOCK", "MUSIC_ARTIST", "MUSIC_GENRE", "MUSIC_RELEASEDATE", "MUSIC_TYPE"};
		pk = "PRODUCT_ID";
		break;
	case 6: // Instrument
		what = "INSTRUMENT";
		atts = { "PRODUCT_NAME", "PRODUCT_PRICE", "PRODUCT_BRAND", "PRODUCT_STOCK", "INSTRUMENT_TYPE", "INSTRUMENT_CONDITION"};
		pk = "PRODUCT_ID";
		break;
	case 7: // Sales
		what = "SALES";
		atts = { "CUSTOMER_ID", "PRODUCT_ID", "QUANTITY", "PRODUCT_PRICE", "PAYMENT_METHOD", "DISCOUNT" };
		pk = "ORDER_ID";
		break;
	case 8: // Rentals
		what = "RENTALS";
		atts = { "CUSTOMER_ID", "PRODUCT_ID", "CHECKOUT_DATE", "CHECKIN_DATE", "MONTHLY_FEE", "PRODUCT_PRICE", "TOTAL_PAID" };
		pk = "RENTAL_ID";
		break;
	case 9: // Services
		what = "SERVICES";
		atts = { "SERVICE_DESCRIPTION", "SERVICE_PRICE", "SERVICE_DATE", "CUSTOMER_ID" };
		pk = "SERVICE_ID";
		break;
	}

	string query = "SELECT " + pk + " FROM " + what;
	sqlite3_stmt* up = RunQuery(db, query);
	pks = MakeVector(db, query, up);
	if (pks.size() == 0) { return; }

	cout << "Please type the " + pk + " of the " + what + " you wish to update. Type -1 to cancel.\n";
	while (1) {
		getline(cin, pkin);
		if (pkin == "-1") { return; }
		if (count(pks.begin(), pks.end(), pkin)) { break; }
		else { cout << "Invalid " + pk + ". Please try again.\n"; }
	}
	cout << "Please indicate which attribute of this " + what + " you wish to update.\n";
	cout << "You can cancel the operation by typing -1. The operation will repeat until you type\"done\".\n";
	for (int i = 0; i < atts.size(); ++i) { cout << i + 1 << ". " << atts[i] << endl; }
	int num;
	while (1) {
		getline(cin, uin);
		if (uin == "-1") { return; }
		if (uin == "done") { break; }
		try { num = stoi(uin); }
		catch (...) { num = -1; }
		if (num > 0 && num <= atts.size()) {
			--num;
			cout << "Please enter new value for " + atts[num] << endl;
			getline(cin, uin);
			if (uin == "-1") { return; }
			update += atts[num] + " = '" + uin + "', ";
		}
		else { cout << "Invalid choice. Please try again.\n"; }
		cout << "Select a new attribute to update.\n";
	}
	if (update.length() == 0) { cout << "No updates were queued. Aborting operation.\n"; return; }
	// Remove leftover ", "
	update.pop_back();
	update.pop_back();
	query = "UPDATE " + what;
	query += " SET " + update;
	query += " WHERE " + pk + " = " + pkin;
	cout << "Updating table " + what + ".\n";
	Finalize(db, query);
}

void DeleteTableEntry(sqlite3*, int d)
{
	string what, pkin, pk; // what = table being edited, pkin = user's selected id, pk = the primary key of the table being edited
	vector<string> pks; // collection of available primary key values
	switch (d) {
	case 1: // Store
		what = "STORE";
		pk = "STORE_ID";
		break;
	case 2: // Employee
		what = "EMPLOYEE";
		pk = "EMPLOYEE_ID";
		break;
	case 3: // Customer
		what = "CUSTOMER";
		pk = "CUSTOMER_ID";
		break;
	case 4: // Product
		what = "PRODUCT";
		pk = "PRODUCT_ID";
		break;
	case 5: // Music
		what = "MUSIC";
		pk = "PRODUCT_ID";
		break;
	case 6: // Instrument
		what = "INSTRUMENT";
		pk = "PRODUCT_ID";
		break;
	case 7: // Sales
		what = "SALES";
		pk = "ORDER_ID";
		break;
	case 8: // Rentals
		what = "RENTALS";
		pk = "RENTAL_ID";
		break;
	case 9: // Services
		what = "SERVICES";
		pk = "SERVICE_ID";
		break;
	}
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

vector<string> MakeVector(sqlite3* db, string& q, sqlite3_stmt* s) {
	vector<string> v;
	if (s != nullptr) {
		while (sqlite3_step(s) == SQLITE_ROW) {
			v.push_back(reinterpret_cast<const char*>(sqlite3_column_text(s, 0)));
		}
	}
	sqlite3_reset(s);
	q.clear();
	return v;
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