#include "stdafx.h"

FileAccess::FileAccess(int a_argc, char* a_argv[])
{
    //check that there is exactly one run time parameter
    if (a_argc != 2)
    {
        cerr << "No Database File Specified!" << endl;
        exit(1);
    }
    
    m_filename = a_argv[1];
    
    //make sure we can connect to the database (i.e. file exists)
    CheckConnection();
}

/**/
/*
 FileAccess::CheckConnection FileAccess::CheckConnection

NAME

        CheckConnection - to make sure the database is accessible
 
SYNOPSIS

        void FileAccess::CheckConnection();
            
DESCRIPTION

        This function opens the database file and closes it to make sure
        it is accessible. If not accesible, program terminates with
        exit code 1 to prevent unexpected behavior from the server.

RETURNS

        None
 
*/
/**/

void FileAccess::CheckConnection()
{
    //open the file
    m_sfile.open(m_filename);
    
    if (!m_sfile)
    {
        cerr << "Database Connection Could Not Be Established, Server Terminated!" << endl;
        
        //end of program
        exit(1);
    }
    
    //close the file
    m_sfile.close();
}
/*void FileAccess::CheckConnection();*/


/**/
/*
 FileAccess::FetchFileData FileAccess::FetchFileData

NAME

        FetchFileData - to read all accounts information from the database to a buffer
 
SYNOPSIS

        stringstream FileAccess::FetchFileData();
            
DESCRIPTION

        This function opens the database file, reads all the account information
        from the file into a buffer and then closes the file.

RETURNS

        The buffer holding the database information
 
*/
/**/

stringstream FileAccess::FetchFileData()
{
    //make sure we are still connected to database
    CheckConnection();
    
    //open the file
    m_sfile.open(m_filename);
    
    //buffer to hold account information
    stringstream buffer;

    //read all accounts information from file to buffer
    buffer << m_sfile.rdbuf();
    
    //close because file may be opened in a different mode
    m_sfile.close();
    
    return buffer;
}
/*stringstream FileAccess::FetchFileData();*/


/**/
/*
 FileAccess::EnterIntoDatabase FileAccess::EnterIntoDatabase

NAME

        EnterIntoDatabase - to add a new client information into the database
 
SYNOPSIS

        void FileAccess::EnterIntoDatabase(const string& a_accountInfo);
            a_accountInfo --> the account information to be insereted into the database
            
DESCRIPTION

        This function opens the file and adds a new username=password and
        a new fresh match record (0-0) into the database.

RETURNS

        None
 
*/
/**/

void FileAccess::EnterIntoDatabase(const string& a_accountInfo)
{
    //make sure we are still connected to database
    CheckConnection();
    
    //open file for appending information
    m_sfile.open(m_filename, fstream::app);

    //write account information to the file
    m_sfile<<endl<<a_accountInfo;
    
    //close file as it may be opened in a different mode
    m_sfile.close();
}
/*void FileAccess::EnterIntoDatabase(const string& a_accountInfo);*/


/**/
/*
 FileAccess::UpdateAccounts FileAccess::UpdateAccounts

NAME

        UpdateAccounts - to update accounts on the database with a match result
 
SYNOPSIS

        void FileAccess::UpdateAccounts(const map<string,bool>& a_outcome, const char a_userPassDelim,
                                 const char a_credentialDelim, const char a_scoreDelim);
 
            a_outcome         --> the map containing player username and whether they won or lost the match
            a_userPassDelim   --> the delimeter to seperate username and password
            a_credentialDelim --> the delimeter to seperate client credentials from their record
            a_scoreDelim      --> the delimeter to seperate the number of client wins and losses
            
DESCRIPTION

        This function opens the database file and updates the match records of the
        clients that were involved in a completed match, either by adding a win or
        loss to their record. The other client account information is untouched.
        

RETURNS

        None
 
*/
/**/
void FileAccess::UpdateAccounts(const map<string,bool>& a_outcome, const char a_userPassDelim,
                                const char a_credentialDelim, const char a_scoreDelim)
{
    //the old file data
    stringstream currentFile = FetchFileData();
    
    //the new file data
    stringstream newFile;

    //line read from database
    string accountInfo;
    
    while (getline (currentFile, accountInfo))
    {
        //parse the username from account information on file
        string userFromFile = accountInfo.substr(0, accountInfo.find(a_userPassDelim, 0));
        
        //for every player in the completed match
        for (auto elem: a_outcome)
        {
            //if the username matches this player
            if (elem.first == userFromFile)
            {
                //if the player won the match
                if (elem.second)
                {
                    //find the position of first delimiter
                    int firstDelPos = (int)accountInfo.find(a_credentialDelim);
                    
                    //find the position of second delimiter
                    int secondDelPos = (int)accountInfo.find(a_scoreDelim);
                    
                    //get the number of wins
                    string numWins = accountInfo.substr(firstDelPos + 1, secondDelPos - firstDelPos - 1);
                    
                    //convert string to integer
                    int wins = stoi(numWins);
                    
                    //make sure wins do not overflow when incrementing
                    if (wins != INT_MAX)
                        wins++;
                    
                    //update the number of wins
                    accountInfo.replace(firstDelPos + 2, numWins.size() - 1, to_string(wins));
                }
                
                //if player lost the  match
                else
                {
                    //get the number of losses
                    int delPos = (int)accountInfo.find(a_scoreDelim);
                    string numLoss = accountInfo.substr(delPos + 1, accountInfo.size() - delPos);
                    
                    //convert string to integer
                    int losses = stoi(numLoss);
                    
                    //make sure losses do not overflow when incrementing
                    if (losses != INT_MAX)
                        losses++;
                    
                    //update the number of losses
                    accountInfo.replace(delPos + 1, numLoss.size(), to_string(losses));
                }
            }
        }
        
        //record the changed or unchanged account info
        newFile << accountInfo << endl;
    }
    
    //open the file and erase current database information
    m_sfile.open(m_filename, ofstream::out | ofstream::trunc);
    
    //overwrite the database with the updated accounts information
    m_sfile<<newFile.str();
    
    //close the file (to open in a different mode possibly)
    m_sfile.close();
}
/*void FileAccess::UpdateAccounts(const map<string,bool>& a_outcome, const char a_userPassDelim,
 const char a_credentialDelim, const char a_scoreDelim);*/


/**/
/*
 FileAccess::GetNumClients FileAccess::GetNumClients

NAME

        GetNumClients - to get the total number of registered accounts
 
SYNOPSIS

        int FileAccess::GetNumClients();
            
DESCRIPTION

        This function finds the total number of clients that are stored
        in the database by counting the non-empty lines in the database file.
        

RETURNS

        The total number of clients in the database
 
*/
/**/

int FileAccess::GetNumClients()
{
    //total number of clients in the database
    int numClients = 0;
    
    //read all database information to a buffer
    stringstream data = FetchFileData();
    
    //a line read from the buffer
    string line;
    
    //as long as end of buffer is not reached
    while (getline(data, line))
    {
        if (!line.empty())
            numClients++;
    }
        
    return numClients;
}
/*int FileAccess::GetNumClients();*/
