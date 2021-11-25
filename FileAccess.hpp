//FileAccess class --> used to read from database and write to it

#pragma once

class FileAccess
{

public:

    //makes sure file exists and can be opened
    FileAccess(int, char*[]);

    //close the file
    ~FileAccess(){m_sfile.close();}
    
    //to make sure database is still accessible
    void CheckConnection();
    
    //to read account information for all clients from database
    stringstream FetchFileData();
    
    //to add a new entry into the database
    void EnterIntoDatabase(const string&);
    
    //to update accounts on database with a match result
    void UpdateAccounts(const map<string,bool>&, const char, const char, const char);
    
    //to get the total number of registered accounts
    int GetNumClients();

private:

    //source file object to read and write
    fstream m_sfile;
    
    //source filename
    string m_filename;
};
