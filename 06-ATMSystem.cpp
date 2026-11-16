#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

const string ClientsFileName = "Clients.txt";


// تعريف دالة القائمة الرئيسية مسبقاً لاستخدامها داخل التوابع الأخرى
void ShowATMMenu();
void Login();

enum enTransactionsOption
{
    eDeposit = 1,
    eNormalWithdrow = 2,
    eQuickWithdrow = 3,
    eCheckBalance = 4,
    Logout = 5
};

enum enQuickWithdrowNumbers
{
    eTwinty = 1,
    eFifty = 2,
    eOneHanderd = 3,
    eTwoHanderd = 4,
    eFourHanderd = 5,
    eSixHanderd = 6,
    eEightHanderd = 7,
    eOnethousand = 8,
};

// هيكل بيانات العميل
struct sClient
{
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double AccountBalance = 0;
    bool MarkForDelete = false;
};
sClient CurrentClient;

// تفكيك السلسلة النصية إلى عناصر Vector بناءً على المحدد (Delim)
vector<string> SplitString(string S1, string Delim = "#//#")
{
    vector<string> vString;
    short pos = 0;
    string sWord;

    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, pos);
        if (sWord != "")
        {
            vString.push_back(sWord);
        }
        S1.erase(0, pos + Delim.length());
    }

    if (S1 != "")
    {
        vString.push_back(S1);
    }

    return vString;
}

// تحويل السطر من الملف إلى هيكل بيانات client
sClient ConvertLinetoRecord(string Line, string Seperator = "#//#")
{
    sClient Client;
    vector<string> vClientData = SplitString(Line, Seperator);

    if (vClientData.size() >= 5)
    {
        Client.AccountNumber = vClientData[0];
        Client.PinCode = vClientData[1];
        Client.Name = vClientData[2];
        Client.Phone = vClientData[3];
        Client.AccountBalance = stod(vClientData[4]);
    }

    return Client;
}

// تحويل بيانات العميل إلى سطر نصي لحفظه في الملف
string ConvertRecordToLine(sClient Client, string Seperator = "#//#")
{
    string stClientRecord = "";
    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance);

    return stClientRecord;
}

// تحميل بيانات العملاء من الملف
vector<sClient> LoadCleintsDataFromFile(string FileName)
{
    vector<sClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in); // فتح الملف للقراءة فقط

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord(Line);
            vClients.push_back(Client);
        }

        MyFile.close();
    }

    return vClients;
}

void PrintClientRecordBalanceLine(sClient Client)
{
    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(40) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.AccountBalance;

}


// البحث عن عميل بواسطة رقم الحساب
bool FindClientByAccountNumberAndPinCode(string AccountNumber,string PinCode, vector<sClient> vClients, sClient& Client)
{
    for (sClient C : vClients)
    {
        if (C.AccountNumber == AccountNumber && C.PinCode == PinCode)
        {
            Client = C;
            return true;
        }
    }
    return false;
}

// التحقق من وجود رقم الحساب مسبقاً
bool ClientExistsByAccountNumber(string AccountNumber, string FileName)
{
    vector<sClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord(Line);
            if (Client.AccountNumber == AccountNumber)
            {
                MyFile.close();
                return true;
            }
        }
        MyFile.close();
    }
    return false;
}

// قراءة بيانات عميل جديد من المستخدم
sClient ReadNewClient()
{
    sClient Client;

    cout << "Enter Account Number? ";
    getline(cin >> ws, Client.AccountNumber);

    while (ClientExistsByAccountNumber(Client.AccountNumber, ClientsFileName))
    {
        cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
        getline(cin >> ws, Client.AccountNumber);
    }

    cout << "Enter PinCode? ";
    getline(cin, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter Account Balance? ";
    cin >> Client.AccountBalance;

    return Client;
}

// تغيير بيانات العميل أثناء التحديث
sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;
    Client.AccountNumber = AccountNumber;

    cout << "\n\nEnter PinCode? ";
    getline(cin >> ws, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter Account Balance? ";
    cin >> Client.AccountBalance;

    return Client;
}


// حفظ بيانات العملاء المتبقية إلى الملف بعد الحذف أو التحديث
vector<sClient> SaveCleintsDataToFile(string FileName, vector<sClient> vClients)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out); // إعادة كتابة الملف بالكامل

    string DataLine;

    if (MyFile.is_open())
    {
        for (sClient C : vClients)
        {
            if (C.MarkForDelete == false)
            {
                DataLine = ConvertRecordToLine(C);
                MyFile << DataLine << endl;
            }
        }
        MyFile.close();
    }

    return vClients;
}

// إضافة سطر جديد للملف
void AddDataLineToFile(string FileName, string stDataLine)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);

    if (MyFile.is_open())
    {
        MyFile << stDataLine << endl;
        MyFile.close();
    }
}




short ReadATMOption()
{
    cout << "Choose what do you want to do? [1 to 5]? ";
    short Choice = 0;
    cin >> Choice;
    return Choice;
}


void GoBackToTransactionMenu()
{
    cout << "\n\nPress any key to go back to Transaction Menu...";
    system("pause>0");
    ShowATMMenu();
}
// العودة للقائمة الرئيسية





int ReadDepositeAmount()
{
    int NumberDeposit;
    cout << "\nPlease enter Deposite amount? ";
    cin >> NumberDeposit;
    return NumberDeposit;
}

int ReadQuickWithdrowAmount()
{
    short Number;
    cout << "\nPlease enter Number Withdrow from [1] to [8]? ";
    cin >> Number;

    if (Number == enQuickWithdrowNumbers::eTwinty)
        return 20;
    if (Number == enQuickWithdrowNumbers::eFifty)
        return 50;
    if (Number == enQuickWithdrowNumbers::eOneHanderd)
        return 100;
    if (Number == enQuickWithdrowNumbers::eTwoHanderd)
        return 200;
    if(Number == enQuickWithdrowNumbers::eFourHanderd)
        return 400;
    if(Number == enQuickWithdrowNumbers::eSixHanderd)
        return 600;
    if(Number == enQuickWithdrowNumbers::eEightHanderd)
        return 800;
    if (Number == enQuickWithdrowNumbers::eOnethousand)
        return 1000;
    else
        return -2;





}

int CalculateDeposit(sClient& Client, int NumberDeposit)
{
    return Client.AccountBalance += NumberDeposit;
}

sClient ExcuteDeposit(sClient& Client, int NumberDeposit)
{

    while (NumberDeposit < 20)
    {
        cout << "Number Deposit (" << NumberDeposit <<
            ") Is a few , Please enter Number bigger than 50 L.E? ";
        cin >> NumberDeposit;
    }
    CalculateDeposit(Client, NumberDeposit);
    return Client;
}

sClient ExcuteWithdraw(sClient& Client, int NumberWithdraw)
{
    while (NumberWithdraw < 20)
    {
        cout << "Number Withdrow (" << NumberWithdraw <<
            ") Is a few , Please enter Number bigger than 50 L.E? ";
        cin >> NumberWithdraw;
    }
    CalculateDeposit(Client, NumberWithdraw * -1);
    return Client;

}

void ShowDepositScreen()
{
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    char MakeSure = 'n';
    cout << "\n-------------------------------------\n";
    cout << "\t\tDeposit Screen\n";
    cout << "\n-------------------------------------\n";

    double NumberDeposit = ReadDepositeAmount();


    cout << "Are You Sure You want Perform this Transaction? y/n ? ";
    cin >> MakeSure;
    if (MakeSure == 'Y' || MakeSure == 'y')
    {
        ExcuteDeposit(CurrentClient, NumberDeposit);
        for (sClient& C : vClients)
        {
            if (C.AccountNumber == CurrentClient.AccountNumber)
            {
                C = CurrentClient;
                break;
            }
        }
        SaveCleintsDataToFile(ClientsFileName, vClients);
    }


}

void ShowNormalWithdrowScreen()
{
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    char MakeSure = 'n';
    cout << "\n-------------------------------------\n";
    cout << "\t\tNormal Withdrow Screen\n";
    cout << "\n-------------------------------------\n";
    int Multiple5 = 0;
    
    cout << "Enter an amount Multiple of 5's? ";
    cin >> Multiple5;

    while (Multiple5 % 5 != 0 || Multiple5 < 20)
    {
        cout << "Enter an amount Multiple of 5's? ";
        cin >> Multiple5;
    }

    while (Multiple5 > CurrentClient.AccountBalance)
    {
        cout << "Amount Exceeds the balance, you can withdraw up to : " << CurrentClient.AccountBalance << endl;
        cout << "Please enter another amount? ";
        cin >> Multiple5;
    }

    

    cout << "\n\nAre You Sure You want Perform thie Transaction? y/n ? ";
    cin >> MakeSure;
    if (MakeSure == 'Y' || MakeSure == 'y')
    {
        ExcuteWithdraw(CurrentClient, Multiple5);
        for (sClient& C : vClients)
        {
            if (C.AccountNumber == CurrentClient.AccountNumber)
            {
                C = CurrentClient;
                break;
            }
        }
        SaveCleintsDataToFile(ClientsFileName, vClients);
    }

}


void ShowQuickWithdrowScreen()
{
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);

    char MakeSure = 'n';
    cout << "\n-------------------------------------\n";
    cout << "\t\tQuick Withdrow Screen\n";
    cout << "\n-------------------------------------\n";
    cout << "\t[1] 20\t\t[2] 50\n\t[3] 100\t\t[4] 200\n\t[5] 400\t\t[6] 600\n\t[7] 800\t\t[8] 1000\n\t[9] Exit()\n";
    cout << "\n-------------------------------------\n";
    double Balance = 0;
    Balance += CurrentClient.AccountBalance;
    cout << "\n\Your Balance is = " << Balance << endl;

    double NumberWithdrow = ReadQuickWithdrowAmount();

    if (NumberWithdrow == -2)
        GoBackToTransactionMenu();

    while (NumberWithdrow > CurrentClient.AccountBalance)
    {
        cout << "Amount Exceeds the balance, you can withdraw up to : " << CurrentClient.AccountBalance << endl;
        cout << "Please enter another amount? ";
        cin >> NumberWithdrow;
    }
    cout << "\n\nAre You Sure You want Perform this Transaction? y/n ? ";
    cin >> MakeSure;
    if (MakeSure == 'Y' || MakeSure == 'y')
    {
        ExcuteWithdraw(CurrentClient, NumberWithdrow);
        for (sClient& C : vClients)
        {
            if (C.AccountNumber == CurrentClient.AccountNumber)
            {
                C = CurrentClient;
                break;
            }
        }
        SaveCleintsDataToFile(ClientsFileName, vClients);
    }

}



void ShowCheckBalanceScreen()
{
    cout << "=============================\n";
    cout << "\tCheck Balance Screen\n";
    cout << "=============================\n";

    double Balance = 0;
    Balance += CurrentClient.AccountBalance;
    cout << "\n\Your Balance is = " << Balance << endl;
}


void PerformATMOption(enTransactionsOption TransactionsOption)
{
    switch (TransactionsOption)
    {
    case enTransactionsOption::eDeposit:
    {
        system("cls");
        ShowDepositScreen();
        GoBackToTransactionMenu();
        break;
    }
    case enTransactionsOption::eNormalWithdrow:
        system("cls");
        ShowNormalWithdrowScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionsOption::eQuickWithdrow:
        system("cls");
        ShowQuickWithdrowScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionsOption::eCheckBalance:
        system("cls");
        ShowCheckBalanceScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionsOption::Logout:
        system("cls");
        Login();
        break;

    }
}


void ShowATMMenu()
{
    system("cls");
    cout << "==================================================\n";
    cout << "              ATM Menu Screen             \n";
    cout << "==================================================\n";
    cout << "\t\t[1] Deposit.\n";
    cout << "\t\t[2] Normal Withdrow.\n";
    cout << "\t\t[3] Quick Withdrow.\n";
    cout << "\t\t[4] Check Balance.\n";
    cout << "\t\t[5] Logout.\n";
    cout << "==================================================\n";

    PerformATMOption((enTransactionsOption)ReadATMOption());
}

void ShowLoginScreen()
{
    system("cls");
    cout << "_________________________________\n";
    cout << "          Login Screen            \n";
    cout << "_________________________________\n";
}


void Login()
{
    ShowLoginScreen();
    string AccountNumber, PinCode;
    vector <sClient> vClients;

    do
    {
        cout << "Enter AccountNumber? ";
        cin >> AccountNumber;
        cout << "Enter PinCode? ";
        cin >> PinCode;

        vClients = LoadCleintsDataFromFile(ClientsFileName);
        if (FindClientByAccountNumberAndPinCode(AccountNumber ,PinCode,vClients, CurrentClient))
        {
            ShowATMMenu();
        }
        else
        {
            ShowLoginScreen();
            cout << "Invalid Username/Password!\n";

        }
    } while (!FindClientByAccountNumberAndPinCode(AccountNumber, PinCode, vClients, CurrentClient));


}

int main()
{
    Login();
    system("pause>0");
    return 0;
}