#include<iostream>
#include<vector>
#include<fstream>
#include<string>

using namespace std;

void ShowMainMenu();
void Login();

struct stClient
{
	string AccountNumber, PinCode, Name, Phone;
	float AccountBalance{ 0.0 };
};

enum enMainMenuOptions
{
	eQuickWithdraw = 1, eNormalWithraw, eDeposite, eCheckBalance, eExit
};

stClient CurrentClient;
const string ClientsFileName = "../Clients.txt";

vector<string> Split(string Line, string Delimiter)
{
	vector<string> vString;
	string Word;
	short Pos{ 0 };
	while ((Pos = Line.find(Delimiter)) != string::npos)
	{
		Word = Line.substr(0, Pos);
		if (Word != "")
			vString.push_back(Word);
		Line.erase(0, Pos + Delimiter.length());
	}
	if (Line != "")
		vString.push_back(Line);
	return vString;
}

stClient ConvertLinToRecord(string Line, string Delimiter = "#//#")
{
	stClient Client;
	vector<string> vClientsData = Split(Line, Delimiter);
	Client.AccountNumber = vClientsData.at(0);
	Client.PinCode = vClientsData.at(1);
	Client.Name = vClientsData.at(2);
	Client.Phone = vClientsData.at(3);
	Client.AccountBalance = stod(vClientsData.at(4));
	return Client;
}

vector<stClient> LoadClientsDataFromFile(string FileName)
{
	vector<stClient> vClients;
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string Line;
		stClient Client;
		while (getline(MyFile, Line))
		{
			Client = ConvertLinToRecord(Line);
			vClients.push_back(Client);
		}
		MyFile.close();
	}
	return vClients;
}

bool FindClientByAccountNumberAndPinCode(string AccountNumber, string PinCode, stClient& CurrentClient)
{
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFileName);
	for (stClient& C : vClients)
		if (C.AccountNumber == AccountNumber && C.PinCode == PinCode)
		{
			CurrentClient = C;
			return true;
		}
	return false;
}

bool LoadClientInfo(string AccountNumber, string PinCode)
{
	return FindClientByAccountNumberAndPinCode(AccountNumber, PinCode, CurrentClient);
}

enMainMenuOptions ReadMainMenuOptions()
{
	short Choice{ 0 };
	cout << "\nPlease enter your choice: ";
	while (!(cin >> Choice))
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "\nInvalid input. Please enter a number between 1 and 5." << endl;
	}
	return enMainMenuOptions(Choice);
}

void GoBackToMainMenu()
{
	cout << "\nPress any key to go back to the main menu...";
	system("pause>0");
	ShowMainMenu();
}

short ReadQuickWithdrawOptions()
{
	short Choice{ 0 };
	cout << "\nPlease enter your choice: ";
	while (!(cin >> Choice))
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "\nInvalid input. Please enter a number between 1 and 8." << endl;
	}
	return Choice;
}

short GetQuickWithdrawAnount(short QuickWithdrawOption)
{
	switch (QuickWithdrawOption)
	{
	case 1: return 20;
	case 2: return 50;
	case 3: return 100;
	case 4: return 200;
	case 5: return 400;
	case 6: return 600;
	case 7: return 800;
	case 8: return 1000;
	default:
		ShowMainMenu();
		return 0;
	}
}

string ConvertRecordToLine(stClient& C, string Delimiter = "#//#")
{
	return C.AccountNumber + Delimiter + C.PinCode + Delimiter +
		C.Name + Delimiter + C.Phone + Delimiter + to_string(C.AccountBalance);
}

vector<stClient> SaveClientsDataToFile(string FileName, vector<stClient>& vClients)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);
	if (MyFile.is_open())
		for (stClient& C : vClients)
			MyFile << ConvertRecordToLine(C) << endl;
	MyFile.close();
	return vClients;
}

bool DepositeBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector<stClient>& vClients)
{
	char Answer{ 'n' };
	cout << "Are you sure you want to perform this transaction? (y/n): ";
	cin >> Answer;

	if (toupper(Answer) == 'Y')
		for (stClient& C : vClients)
			if (C.AccountNumber == AccountNumber)
			{
				C.AccountBalance += Amount;
				SaveClientsDataToFile(ClientsFileName, vClients);
				cout << "\n\nDone Successfully. New balance is: " << C.AccountBalance;
				return true;
			}
	return false;
}

void PerformQuickWithdrawOptions(short QuickWithdrawOption)
{
	if (QuickWithdrawOption > 9 || QuickWithdrawOption < 1)
	{
		cout << "Invalid Option!\n";
		return;
	}
	short WithdrawBalance = GetQuickWithdrawAnount(QuickWithdrawOption);
	if (WithdrawBalance > CurrentClient.AccountBalance)
	{
		cout << "The amount exceeds your balance!\n";
		cout << "Your balance is: " << CurrentClient.AccountBalance << "\n";
		cout << "Press any key to go back to Main Menu...\n";
		system("pause>0");
		ShowMainMenu();
	}
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFileName);
	DepositeBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithdrawBalance * -1, vClients);
	CurrentClient.AccountBalance -= WithdrawBalance;
}

void ShowQuickWithdrawScreen()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\tQuick Withdraw\n";
	cout << "===========================================\n";
	cout << "\t[1] 20\t\t[2] 50\n";
	cout << "\t[3] 100\t\t[4] 200\n";
	cout << "\t[5] 400\t\t[6] 600\n";
	cout << "\t[7] 800\t\t[8] 1000\n";
	cout << "\t[9] Exit\n";
	cout << "===========================================\n";
	cout << "Your balance is: " << CurrentClient.AccountBalance << endl;
	PerformQuickWithdrawOptions(ReadQuickWithdrawOptions());
}

float ReadWithdrawAmount()
{
	float Amount{ 0.0 };
	cout << "\nPlease enter the amount you want to withdraw: ";
	while (!(cin >> Amount) || Amount <= 0)
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "\nInvalid input. Please enter a valid amount." << endl;
	}
	return Amount;
}

void PerformNormalWithdrawScreen()
{
	float WithdrawBalance = ReadWithdrawAmount();
	if (WithdrawBalance > CurrentClient.AccountBalance)
	{
		cout << "The amount exceeds your balance!\n";
		cout << "Your balance is: " << CurrentClient.AccountBalance << "\n";
		cout << "Press any key to go back to Main Menu...\n";
		system("pause>0");
		ShowMainMenu();
	}
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFileName);
	DepositeBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithdrawBalance * -1, vClients);
	CurrentClient.AccountBalance -= WithdrawBalance;
}

void ShowNormalWithdrawScreen()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\t\b\b\b\b\bNormal Withdraw Screen\n";
	cout << "===========================================\n";
	PerformNormalWithdrawScreen();
}

float ReadDepositAmount()
{
	float Amount{ 0.0 };
	cout << "\nPlease enter the amount you want to deposit: ";
	while (!(cin >> Amount) || Amount <= 0)
	{
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "\nInvalid input. Please enter a valid amount." << endl;
	}
	return Amount;
}

void PerformDepositOption()
{
	float DepositBalance = ReadDepositAmount();
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFileName);
	DepositeBalanceToClientByAccountNumber(CurrentClient.AccountNumber, DepositBalance, vClients);
	CurrentClient.AccountBalance += DepositBalance;
}

void ShowDepositeScreen()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\tDeposit Screen\n";
	cout << "===========================================\n";
	PerformDepositOption();
}

void ShowCheckBalanceScreen()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\t\b\b\bCheck Balance Screen\n";
	cout << "===========================================\n";
	cout << "Your balance is: " << CurrentClient.AccountBalance << endl;
}

void PerformMainMenuOptions(enMainMenuOptions MainMenuOptions)
{
	switch (MainMenuOptions)
	{
	case enMainMenuOptions::eQuickWithdraw:
	{
		system("cls");
		ShowQuickWithdrawScreen();
		GoBackToMainMenu();
		break;
	}
	case enMainMenuOptions::eNormalWithraw:
	{
		system("cls");
		ShowNormalWithdrawScreen();
		GoBackToMainMenu();
		break;
	}
	case enMainMenuOptions::eDeposite:
	{
		system("cls");
		ShowDepositeScreen();
		GoBackToMainMenu();
		break;
	}
	case enMainMenuOptions::eCheckBalance:
	{
		system("cls");
		ShowCheckBalanceScreen();
		GoBackToMainMenu();
		break;
	}
	case enMainMenuOptions::eExit:
	{
		system("cls");
		cout << "Thank you for using our ATM service. Goodbye!" << endl;
		Login();
		break;
	}
	}
}

void ShowMainMenu()
{
	system("cls");
	cout << "===========================================\n";
	cout << "\t\t\b\b\bATM Main Menu Screen\n";
	cout << "===========================================\n";
	cout << "\t[1] Quick Withdraw.\n";
	cout << "\t[2] Normal Withdraw.\n";
	cout << "\t[3] Deposit\n";
	cout << "\t[4] Check Balance.\n";
	cout << "\t[5] Logout.\n";
	cout << "===========================================\n";
	PerformMainMenuOptions(ReadMainMenuOptions());
}

void Login()
{
	bool LoginFailed{ false };
	string AccountNumber, PinCode;
	
	do
	{
		system("cls");
		cout << "Welcome to ATM System\n";
		cout << "\n-------------------------------\n";
		cout << "\tLogin Screen";
		cout << "\n-------------------------------\n";

		if (LoginFailed)
			cout << "\nInvalid account number or pin code. Please try again." << endl;

		cout << "\nPlease enter your account number: ";
		getline(cin >> ws, AccountNumber);

		cout << "\nPlease enter your pin code: ";
		getline(cin, PinCode);

		LoginFailed = !LoadClientInfo(AccountNumber, PinCode);

	} while (LoginFailed);
	ShowMainMenu();
}

int main()
{
	Login();
}