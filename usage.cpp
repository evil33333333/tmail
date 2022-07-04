#include <vector>
#include <fstream>
#include <iostream>
#include <tmail/tmail.h>
#include <tmail/inbox.h>

int main()
{
    T_Mail t_mail;
    T_Mail::Client* client = t_mail.MakeClient("email@mail.tm", "*******");
    std::vector<std::string> verification_messages;
    if (client->ok) 
    {
            std::cout << "[!] Created the email :)" << std::endl;
        
            // Get the current clients inbox
            Inbox inbox = client->GetInbox();
            if (inbox.ok)
            {
                for (auto const& msg : inbox.preview_messages)
                {
                    if (msg.subject.find("verification code") != std::string::npos)
                    {
                        verification_messages.push_back(msg.subject);
                    }
                }
            }
    }
    // Deletes the account from mail.tm and frees heap memory
    t_mail.DeleteClient(client);
    
    
    std::ofstream file("verification_codes.txt");
    if (!file.is_open()) 
    {
        std::cout << "[X] Could not open a file :(" << std::endl;   
    }
    for (auto const& msg : verification_messages)
    {
        file << msg << std::endl;   
    }
    file.close();
    
    std::cout << "[!] Written all received verification messages to a file :)" << std::endl;
    return 0;
    
    
}
