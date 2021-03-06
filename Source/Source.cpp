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
void DeleteTableEntry(sqlite3* db, int d);
void ViewTableRow(sqlite3* db, int r);
void AddToSales(sqlite3* db);
void AddToRentals(sqlite3* db);
void AddToServices(sqlite3* db);
sqlite3_stmt* RunQuery(sqlite3* db, string q);
vector<string> MakeVector(sqlite3* db, string& q, sqlite3_stmt* s);
void TypeSelector(vector<string> v, string& t);
void WhatAndPK(string wp[2], int w);
int SubMenuChoice();
bool Finalize(sqlite3* db, string q, bool f = true);
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
				num = SubMenuChoice();
				if (num == -1) { break; }
				if (num <= 6) {
					switch (num) {
					case 1: AddToStore(mydb); break;
					case 2: AddToEmployee(mydb); break;
					case 3: AddToCustomer(mydb); break;
					case 4: AddToProduct(mydb); break;
					case 5: AddToMusic(mydb); break;
					case 6: AddToInstrument(mydb);
					}
					break;
				} else { cout << "Invalid choice. Please try again.\n"; }
			}
			break;
		case 2: // Update table
			PrintMenu(2);
			num = SubMenuChoice();
			if (num == -1) { break; }
			UpdateTable(mydb, num);
			break;
		case 3: // Delete from table.
			PrintMenu(3);
			num = SubMenuChoice();
			if (num == -1) { break; }
			DeleteTableEntry(mydb, num);
			break;
		case 4: // View table info.
			PrintMenu(4);
			num = SubMenuChoice();
			if (num == -1) { break; }
			ViewTableRow(mydb, num);
			break;
		case 5: // Create new sales entry.
			AddToSales(mydb);
			break;
		case 6: // Create new rentals entry.
			AddToRentals(mydb);
			break;
		case 7: // Create new services entry.
			AddToServices(mydb);
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
	case 4: // View row in table.
		cout << "Please select which table you would like to view data in. Type -1 to cancel\n";
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
	TypeSelector(custtypes, ctype);
	
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
	TypeSelector(mustypes, mtype);

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
	TypeSelector(insttypes, itype);

	cout << "Please indicate the instrument's condition by selecting a number from the following list:\n";
	TypeSelector(instconds, icond);

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
	string whatpk[2];// whatpk[0] = the table being edited, whatpk[1] = name of that table's primary key
	string pkin, uin, update; // , pkin = the id of the table being edited, 
	vector<string> pks; // Collection of valid pks from table.
	vector<string> atts; // Attributes that can be edited.

	WhatAndPK(whatpk, u);
	// Switch statement prepares table-specific terminology for sql statement. 
	switch (u) {
	case 1: atts = { "STORE_NAME", "STORE_EMAIL", "STORE_PHONE", "STORE_ADDRESS" }; break;
	case 2: atts = { "STORE_ID", "EMPLOYEE_FIRSTNAME", "EMPLOYEE_LASTNAME", "EMPLOYEE_EMAIL" ,"EMPLOYEE_PHONE" }; break;
	case 3: atts = { "CUSTOMER_FIRSTNAME", "CUSTOMER_LASTNAME", "CUSTOMER_ADDRESS", "CUSTOMER_PHONE", "CUSTOMER_TYPE" }; break;
	case 4: atts = { "PRODUCT_NAME", "PRODUCT_PRICE", "PRODUCT_BRAND", "PRODUCT_STOCK" }; break;
	case 5: atts = { "PRODUCT_NAME", "PRODUCT_PRICE", "PRODUCT_STOCK", "MUSIC_ARTIST", "MUSIC_GENRE", "MUSIC_RELEASEDATE", "MUSIC_TYPE" }; break;
	case 6: atts = { "PRODUCT_NAME", "PRODUCT_PRICE", "PRODUCT_BRAND", "PRODUCT_STOCK", "INSTRUMENT_TYPE", "INSTRUMENT_CONDITION" }; break;
	case 7: atts = { "CUSTOMER_ID", "PRODUCT_ID", "QUANTITY", "PRODUCT_PRICE", "PAYMENT_METHOD", "DISCOUNT" }; break;
	case 8: atts = { "CUSTOMER_ID", "PRODUCT_ID", "CHECKOUT_DATE", "CHECKIN_DATE", "MONTHLY_FEE", "PRODUCT_PRICE", "TOTAL_PAID" };  break;
	case 9: atts = { "CUSTOMER_ID", "SERVICE_DESCRIPTION", "SERVICE_DATE", "SERVICE_PRICE", "SERVICE DISCOUNT" }; break;
	}

	string query = "SELECT " + whatpk[1] + " FROM " + whatpk[0];
	sqlite3_stmt* up = RunQuery(db, query);
	pks = MakeVector(db, query, up);
	if (pks.size() == 0) { return; }
	sqlite3_finalize(up);

	cout << "Please type the " + whatpk[1] + " of the " + whatpk[0] + " you wish to update. Type -1 to cancel.\n";
	while (1) {
		getline(cin, pkin);
		if (pkin == "-1") { return; }
		if (count(pks.begin(), pks.end(), pkin)) { break; }
		else { cout << "Invalid " + whatpk[1] + ". Please try again.\n"; }
	}
	cout << "Please indicate which attribute of this " + whatpk[0] + " you wish to update.\n";
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

	cout << "Updating " + whatpk[0] + ". . .\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	query = "UPDATE " + whatpk[0];
	query += " SET " + update;
	query += " WHERE " + whatpk[1] + " = " + pkin;
	cout << "Updating table " + whatpk[0] + ".\n";
	Finalize(db, query);
}

void DeleteTableEntry(sqlite3* db, int d)
{
	string whatpk[2]; // whatpk[0] = the table being edited, whatpk[1] = name of that table's primary key
	string pkin, uin; // pkin = user's selected id
	vector<string> pks; // collection of available primary key values
	WhatAndPK(whatpk, d);
	
	string query = "SELECT " + whatpk[1] + " FROM " + whatpk[0];
	sqlite3_stmt* del = RunQuery(db, query);
	pks = MakeVector(db, query, del);
	if (pks.size() == 0) { return; }
	sqlite3_finalize(del);

	cout << "Please type the " + whatpk[1] + " of the " + whatpk[0] + " you wish to delete. Type -1 to cancel.\n";
	while (1) {
		getline(cin, pkin);
		if (pkin == "-1") { return; }
		if (count(pks.begin(), pks.end(), pkin)) { break; }
		else { cout << "Invalid " + whatpk[1] + ". Please try again.\n"; }
	}
	cout << "ALERT: Deleting a row from a table cannot be undone. If you do not know whatpk[0] data you are deleting, you can view it from the main menu.\n";
	cout << "Type \"I understand\" to finalize deletion. Otherwise, type -1 to cancel.\n";
	while (1) {
		getline(cin, uin);
		if (uin == "-1") { return; }
		if (uin == "I understand") { break; }
		else { cout << "Not recognized. Please try again.\n"; }
	}

	cout << "Deleting row from " + whatpk[0] + ". . .\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	query = "DELETE FROM " + whatpk[0];
	query += " WHERE " + whatpk[1] + " = " + pkin;
	Finalize(db, query);
}

void ViewTableRow(sqlite3* db, int r)
{
	string whatpk[2];
	string pkin;
	vector<string> atts, pks, results;
	WhatAndPK(whatpk, r);

	// Set descriptions for viewable attributes. Includes all columns in each table except the primary key.
	switch (r) {
	case 1: atts = { "Store name: ", "Store email: ", "Store phone: ", "Store address: " }; break;
	case 2: atts = { "Store ID: ", "First name: ", "Last name: ", "Employee email: ", "Employee phone: " }; break;
	case 3: atts = { "First name: ", "Last name: ", "Customer address: ", "Customer phone: ", "Rentals: ", "Customer type: " }; break;
	case 4: atts = { "Product name: ", "Price: $", "Brand: ", "Product stock: " }; break;
	case 5: atts = { "Music name: ", "Price: $", "Product stock: ", "Artist: ", "Genre: ", "Release date: ", "Music type: " }; break;
	case 6: atts = { "Instrument name: ", "Price: $", "Brand: ", "Product stock: ", "Instrument type: ", "Condition: " }; break;
	case 7: atts = { "Customer ID: ", "Product ID: ", "Quantity: ", "Price: ", "Payment method: ", "Discount: " }; break;
	case 8: atts = { "Customer ID; ", "Product ID: ", "Check-out date: ", "Check-in date: ", "Monthly fee: $", "Product price: $", "Total paid: $" }; break;
	case 9: atts = { "Customer ID: ", "Service description: ", "Service date: ", "Service price: $", "Discount: $" }; break;
	}

	string query = "SELECT " + whatpk[1] + " FROM " + whatpk[0];
	sqlite3_stmt* see = RunQuery(db, query);
	pks = MakeVector(db, query, see);
	if (pks.size() == 0) { return; }

	cout << "Please type the " + whatpk[1] + " of the " + whatpk[0] + " you wish to view. Type -1 to cancel.\n";
	while (1) {
		getline(cin, pkin);
		if (pkin == "-1") { return; }
		if (count(pks.begin(), pks.end(), pkin)) { break; }
		else { cout << "Invalid " + whatpk[1] + ". Please try again.\n"; }
	}

	query = "SELECT * from " + whatpk[0];
	query += " WHERE " + whatpk[1] + " = " + pkin;
	see = RunQuery(db, query);
	sqlite3_step(see);
	int ct = sqlite3_column_count(see);
	for (int i = 1; i < ct; ++i) { results.push_back(reinterpret_cast<const char*>(sqlite3_column_text(see, i))); }
	if (results.size() != atts.size()) { cout << "Something went wrong with gathering results.\n"; return; }
	cout << "--------------------------------------\n";
	for (int j = 0; j < results.size(); ++j) { cout << atts[j] + results[j] << endl; }
	cout << "--------------------------------------\n\n";
	sqlite3_finalize(see);
}

void AddToSales(sqlite3* db)
{
	string cid, pid, pmeth, prodcat, uin;
	float discmult = 0, pprice, pdisc;
	int stock, qty;
	vector<string> customers, pmethods, prod, mus, inst;

	// Get available customer ids.
	string query = "SELECT CUSTOMER_ID FROM CUSTOMER";
	sqlite3_stmt* ats = RunQuery(db, query);
	customers = MakeVector(db, query, ats);
	if (customers.size() == 0) { return; }

	// Get available payment methods.
	query = "SELECT PAYMENT_METHOD FROM PAYMENT";
	ats = RunQuery(db, query);
	pmethods = MakeVector(db, query, ats);
	if (pmethods.size() == 0) { return; }

	// Get available products.
	query = "SELECT PRODUCT_ID FROM PRODUCT";
	ats = RunQuery(db, query);
	prod = MakeVector(db, query, ats);
	if (prod.size() == 0) { return; }

	// Get available music.
	query = "SELECT PRODUCT_ID FROM MUSIC";
	ats = RunQuery(db, query);
	mus = MakeVector(db, query, ats);
	if (mus.size() == 0) { return; }

	// Get available instruments.
	query = "SELECT PRODUCT_ID FROM INSTRUMENT";
	ats = RunQuery(db, query);
	inst = MakeVector(db, query, ats);
	if (inst.size() == 0) { return; }
	
	// Combine vectors.
	prod.reserve(mus.size() + inst.size());
	prod.insert(prod.end(), mus.begin(), mus.end());
	prod.insert(prod.end(), inst.begin(), inst.end());
	// Free memory from newly unused vectors.
	mus = vector<string>(); 
	inst = vector<string>();

	cout << "Please type the customer ID of the customer for this sales transaction. You can type -1 at any time to cancel.\n";
	while (1) {
		getline(cin, cid);
		if (cid == "-1") { return; }
		if (count(customers.begin(), customers.end(), cid)) { break; }
		else { cout << "Invalid customer ID. Please try again.\n"; }
	}

	query = "SELECT CUSTOMER_TYPE FROM CUSTOMER ";
	query += "WHERE CUSTOMER_ID = " + cid;
	ats = RunQuery(db, query);
	sqlite3_step(ats);
	uin = reinterpret_cast<const char*>(sqlite3_column_text(ats, 0));
	if (uin == "VIP") { discmult = -0.15; } // VIP customers have a 15% discount. 
	sqlite3_reset(ats);

	cout << "Please type the product ID of the product being purchased in this sales transaction.\n";
	while (1) {
		getline(cin, pid);
		if (cid == "-1") { return; }
		if (count(prod.begin(), prod.end(), pid)) { break; }
		else { cout << "Invalid product ID. Please try again.\n"; }
	}

	// Needlessly elaborate work to grab product price and quantity from one of 3 tables based on pid.
	int temp = stoi(pid);
	if (temp > 0 && temp <= 99) { prodcat = "PRODUCT"; }
	else if (temp > 1000 && temp <= 9999) { prodcat = "MUSIC"; }
	else { prodcat = "INSTRUMENT"; }
	query = "SELECT PRODUCT_PRICE, PRODUCT_STOCK FROM " + prodcat;
	query += " WHERE PRODUCT_ID = " + pid;
	ats = RunQuery(db, query);

	sqlite3_step(ats);
	uin = reinterpret_cast<const char*>(sqlite3_column_text(ats, 0));
	pprice = stof(uin); // Funky business happening with the cast. 
	uin = reinterpret_cast<const char*>(sqlite3_column_text(ats, 1));
	stock = stoi(uin);
	sqlite3_finalize(ats);
	// End complicated work I could have avoided if my db was better designed.

	cout << "Please input the quantity of the product being purchased in this sales transaction.\n";
	while (1) {
		getline(cin, uin);
		if (uin == "-1") { return; }
		try { qty = stoi(uin); }
		catch (...) { qty = -1; }
		if (qty > 0) {
			if (qty <= stock) { break; }
			else { cout << "Error: quantity is higher than product's current stock. Please input a new value.\n"; }
		} 
		else { cout << "Invalid quantity. Please try again.\n"; }
	}

	cout << "Please indicate the payment method by selecting a number from the following list:\n";
	TypeSelector(pmethods, pmeth);

	cout << "Please type the discount for this transaction as a negative currency amount. The currency symbol isn't necessary.\n";
	cout << "This number is added to automatic discounts. Do not include automatic discounts.\n";
	while (1) {
		getline(cin, uin);
		if (uin == "-1") { return; }
		if (CheckIfValidCurrency(uin)) { 
			pdisc = stof(uin);
			if (pdisc <= 0) { break; }
			else { cout << "Error: Discount must be zero or a negative number.\n"; }
		}
		else { cout << "Invalid discount. Please try again.\n"; }
	}
	pdisc += (pprice * discmult);
	pdisc = round(pdisc * 100.0) / 100.0;

	// GIVE ME A DRINK BAR-TENDER
	cout << "Creating sales invoice.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Assemblin invoice. . .\n";
	query = "INSERT INTO SALES (CUSTOMER_ID, PRODUCT_ID, QUANTITY, PRODUCT_PRICE, PAYMENT_METHOD, DISCOUNT) ";
	query += "VALUES (" + cid + ", ";  // CUSTOMER_ID
	query += pid + ", "; // PRODUCT_ID
	query += to_string(qty) + ", "; // QUANTITY
	query += to_string(pprice) + ", "; // PRODUCT_PRICE
	query += "'" + pmeth + "', "; // PAYMENT_METHOD

	int precisionVal = 2; // Corrects horrifically rounded float value. 
	string discount = to_string(pdisc).substr(0, to_string(pdisc).find(".") + precisionVal + 1);
	query += discount + "); "; // DISCOUNT

	cout << "Updating stock levels. . .\n";
	query += "UPDATE " + prodcat;
	query += " SET PRODUCT_STOCK = PRODUCT_STOCK - " + to_string(qty);
	query += " WHERE PRODUCT_ID = " + pid + ";";
	Finalize(db, query);
}

void AddToRentals(sqlite3* db)
{
	string cid, pid, outdate, indate, pprice, mfee, tpaid, rentals;
	vector<string> customers, products;

	// Get available customers.
	string query = "SELECT CUSTOMER_ID FROM CUSTOMER";
	sqlite3_stmt* atr = RunQuery(db, query);
	customers = MakeVector(db, query, atr);
	if (customers.size() == 0) { return; }

	// Get available instruments.
	query = "SELECT PRODUCT_ID FROM INSTRUMENT";
	atr = RunQuery(db, query);
	products = MakeVector(db, query, atr);
	if (products.size() == 0) { return; }

	cout << "Please type the customer ID of the customer for this rental. You can type -1 at any time to cancel.\n";
	while (1) {
		getline(cin, cid);
		if (cid == "-1") { return; }
		if (count(customers.begin(), customers.end(), cid)) { break; }
		else { cout << "Invalid customer ID. Please try again.\n"; }
	}

	cout << "Please type the product ID of the instrument being rented.\n";
	while (1) {
		getline(cin, pid);
		if (cid == "-1") { return; }
		if (count(products.begin(), products.end(), pid)) { break; }
		else { cout << "Invalid product ID. Please try again.\n"; }
	}

	cout << "Please type the date the instrument was checked out, in format MM/DD/YYYY.\n";
	getline(cin, outdate);
	if (outdate == "-1") { return; }
	cout << "Please type the date the instrument was checked in, in format MM/DD/YYYY.\n";
	cout << "If the instrument has not been checked in yet, type \"none\".\n";
	getline(cin, indate);
	if (indate == "-1") { return; }

	cout << "Please type the monthly fee for this rental. The currency symbol is not required.\n";
	while (1) {
		getline(cin, mfee);
		if (mfee == "-1") { return; }
		if (CheckIfValidCurrency(mfee)) { break; }
		else { cout << "Invalid monthly fee. Please try again.\n"; }
	}

	cout << "Please type the total amount paid on this rental so far.\n";
	while (1) {
		getline(cin, tpaid);
		if (tpaid == "-1") { return; }
		if (CheckIfValidCurrency(tpaid)) { break; }
		else { cout << "Invalid price. Please try again.\n"; }
	}

	query = "SELECT PRODUCT_PRICE FROM INSTRUMENT ";
	query += "WHERE PRODUCT_ID = " + pid;
	atr = RunQuery(db, query);
	sqlite3_step(atr);
	pprice = reinterpret_cast<const char*>(sqlite3_column_text(atr, 0));

	cout << "Creating rental record.\n";
	sqlite3_exec(db, "BEGIN;", NULL, NULL, NULL);
	cout << "Assembling rental. . .\n";
	query = "INSERT INTO RENTALS (CUSTOMER_ID, PRODUCT_ID, CHECKOUT_DATE, CHECKIN_DATE, MONTHLY_FEE, PRODUCT_PRICE, TOTAL_PAID) ";
	query += "VALUES (" + cid + ", ";
	query += pid + ", ";
	query += "'" + outdate + "', ";
	if (indate == "none") { query += "'N/A', "; }
	else { query += "'" + indate + "'. "; }
	query += mfee + ", ";
	query += pprice + ", ";
	query += tpaid + "); ";
	Finalize(db, query);

	query = "SELECT RENTAL_ID FROM RENTALS ";
	query += "WHERE CUSTOMER_ID = " + cid;
	atr = RunQuery(db, query);
	rentals = "'{";
	while (sqlite3_step(atr) == SQLITE_ROW) {
		rentals += reinterpret_cast<const char*>(sqlite3_column_text(atr, 0));
		rentals += ",";
	}
	rentals.pop_back(); // delete remaining ","
	rentals += "}'";
	if (!sqlite3_finalize(atr)) { return; }

	query = "UPDATE CUSTOMER ";
	query += "SET CUSTOMER_RENTALS = " + rentals;
	query += " WHERE CUSTOMER_ID = " + cid;
	Finalize(db, query, false);
}

void AddToServices(sqlite3* db)
{
	string cid, sdesc, sdate, sprice, uin;
	float disc;
	vector<string> customers;

	// Get available customers.
	string query = "SELECT CUSTOMER_ID FROM CUSTOMER";
	sqlite3_stmt* atv = RunQuery(db, query);
	customers = MakeVector(db, query, atv);
	if (customers.size() == 0) { return; }
	sqlite3_finalize(atv);

	cout << "Please type the customer ID of the customer for this service. You can type -1 at any time to cancel.\n";
	while (1) {
		getline(cin, cid);
		if (cid == "-1") { return; }
		if (count(customers.begin(), customers.end(), cid)) { break; }
		else { cout << "Invalid customer ID. Please try again.\n"; }
	}

	cout << "Please type the description of this service.\n";
	getline(cin, sdesc);
	if (sdesc == "-1") { return; }

	cout << "Please input the date this service was performed in format MM/DD/YYYY.\n";
	getline(cin, sdate);
	if (sdate == "-1") { return; }

	cout << "Please type the price for this service. The currency symbol is not necessary.\n";
	while (1) {
		getline(cin, sprice);
		if (sprice == "-1") { return; }
		if (CheckIfValidCurrency(sprice)) { break; }
		else { cout << "Invalid price. Please try again.\n"; }
	}

	cout << "Please type the discount for this transaction as a negative currency amount. The currency symbol isn't necessary.\n";
	while (1) {
		getline(cin, uin);
		if (uin == "-1") { return; }
		if (CheckIfValidCurrency(uin)) {
			disc = stof(uin);
			if (disc <= 0) { break; }
			else { cout << "Error: Discount must be zero or a negative number.\n"; }
		}
		else { cout << "Invalid discount. Please try again.\n"; }
	}

	query = "INSERT INTO SERVICES (CUSTOMER_ID, SERVICE_DESCRIPTION, SERVICE_DATE, SERVICE_PRICE, DISCOUNT) ";
	query += "VALUES (" + cid + ", ";
	query += "'" + sdesc + "', ";
	query += "'" + sdate + "', ";
	query += sprice + ", ";
	query += to_string(disc) + ");";
	Finalize(db, query);
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

void TypeSelector(vector<string> v, string& t)
{
	for (int i = 0; i < v.size(); ++i) { cout << i + 1 << ". " << v[i] << endl; }
	int num;
	while (1) {
		getline(cin, t);
		if (t == "-1") { return; }
		try { num = stoi(t); }
		catch (...) { num = -1; }
		if (num > 0 && num <= v.size()) {
			t = v[num - 1];
			break;
		}
		else { cout << "Invalid choice. Please try again.\n"; }
	}
}

void WhatAndPK(string wp[2], int w)
{
	switch (w) {
	case 1: // Store
		wp[0] = "STORE";
		wp[1] = "STORE_ID";
		break;
	case 2: // Employee
		wp[0] = "EMPLOYEE";
		wp[1] = "EMPLOYEE_ID";
		break;
	case 3: // Customer
		wp[0] = "CUSTOMER";
		wp[1] = "CUSTOMER_ID";
		break;
	case 4: // Product
		wp[0] = "PRODUCT";
		wp[1] = "PRODUCT_ID";
		break;
	case 5: // Music
		wp[0] = "MUSIC";
		wp[1] = "PRODUCT_ID";
		break;
	case 6: // Instrument
		wp[0] = "INSTRUMENT";
		wp[1] = "PRODUCT_ID";
		break;
	case 7: // Sales
		wp[0] = "SALES";
		wp[1] = "ORDER_ID";
		break;
	case 8: // Rentals
		wp[0] = "RENTALS";
		wp[1] = "RENTAL_ID";
		break;
	case 9: // Services
		wp[0] = "SERVICES";
		wp[1] = "SERVICE_ID";
		break;
	}
}

int SubMenuChoice()
{
	string uin;
	int num;
	while (1) {
		getline(cin, uin);
		try { num = stoi(uin); }
		catch (...) { num = 0; }
		if ((num > 0 && num <= 9) || num == -1) { return num; }
		else { cout << "Invalid choice. Please try again.\n"; }
	}
}

bool Finalize(sqlite3* db, string q, bool f) {
	// Bool is to return status in cases where finalize occurs before end of function. 
	// Otherwise this can be run without needing to use the returned value.
	if (f) { cout << "Finalizing. . .\n"; }
	if (sqlite3_exec(db, q.c_str(), NULL, NULL, NULL) != SQLITE_OK)
	{
		string m_strLastError = sqlite3_errmsg(db);
		cout << "There was an error: " << m_strLastError << endl;
		cout << "Aborting operation.\n\n";
		sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
		return false;
	}
	else { 
		if (f) { cout << "Operation complete! Returning to main menu.\n"; }
		sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
		return true;
	}
	
}

bool CheckIfValidCurrency(string const& input) {
	regex validCurrency("-?([0-9]{0,4})\.([0-9]{0,2})"); // Lazy
	return regex_match(input, validCurrency);
}