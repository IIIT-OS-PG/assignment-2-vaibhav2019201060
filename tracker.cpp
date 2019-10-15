#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <unordered_map>
#include <bits/stdc++.h>
#include <string>
using namespace std;
unordered_map<string, string> datamap;
void upload_function(char *fn, char *portt, int accept_socket_ID)
{
    string filename = fn;
    string port = portt;

    if (datamap.find(filename) == datamap.end())
    { // key is not present in map.
        datamap[filename] = port;
    }
    else
    { // key is present in map
        datamap[filename] = datamap[filename] + " " + port;
    }
    string returnvalue = "file uploaded successfully";
    cout << "file received, return data for client is: " << returnvalue << endl;
    int n = returnvalue.length();
    char char_data[n + 1];
    strcpy(char_data, returnvalue.c_str());
    int send_data = send(accept_socket_ID, char_data, sizeof(char_data), 0);
    if (send_data < 0)
    {
        cout << "Could not send data back to the client" << endl;
    }
}
void download_function(char *fn, int accept_socket_ID)
{
    string filename = fn;
    string returnvalue;
    if (datamap.find(filename) == datamap.end())
    { // key is not present in map.
        returnvalue = "No client has requested file";
    }
    else
    { // key is present in map
        unordered_map<string, string>::iterator pi;
        for (pi = datamap.begin(); pi != datamap.end(); pi++)
        {
            cout << "filename: " << pi->first << " , portnos: " << pi->second << endl;
            string checkstring = pi->first;
            if ((checkstring.compare(filename)) == 0)
            {
                returnvalue = pi->second;
            }
        }
    }
    int n = returnvalue.length();
    char char_data[n + 1];
    strcpy(char_data, returnvalue.c_str());
    int send_data = send(accept_socket_ID, char_data, sizeof(char_data), 0);
    if (send_data < 0)
    {
        cout << "Could not send data back to the client" << endl;
    }
}
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int server_socket_ID = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket_ID < 0)
    {
        cout << "Can't create Socket in Server" << endl;
        exit(1);
    }
    /*  if (setsockopt(server_socket_id, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)))
    {
        cout << "Error in sersockopt in server" << endl;
        //perror("setsockopt");
        exit(1);
    } */
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(10000);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //cout << "this is server address " << serveraddr.sin_addr.s_addr << endl;

    int bind_ID = bind(server_socket_ID, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if (bind_ID < 0)
    {
        cout << "Error in Binding in Server" << endl;
        exit(1);
    }
    int listen_ID = listen(server_socket_ID, 15);
    if (listen_ID < 0)
    {
        cout << "Tracker can't listen on given port" << endl;
    }
    else
    {
        cout << "Tracker started on port 10000" << endl;
    }
    while (1)
    {
        int accept_socket_ID = accept(server_socket_ID, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (accept_socket_ID < 0)
        {
            cout << "Can't complete access call on server" << endl;
            exit(1);
        }
        int recv_data;
        char data_buffer[10000];
        recv_data = recv(accept_socket_ID, data_buffer, sizeof(data_buffer), 0);
        if (recv_data < 0)
        {
            cout << "Error in receiving the data" << endl;
        }
        data_buffer[recv_data] = '\0';
        //printf("%s\n", buffer);
        printf("receive data at tracker is'%s'\n", data_buffer);
        //char *msg = "Hello world";
        char *token = strtok(data_buffer, " ");
        string input[1000];
        int k = 0;
        while (token != NULL)
        {
            input[k] = token;
            k++;
            //printf("token %s\n", token);
            token = strtok(NULL, " ");
        }
        if (input[0].compare("upload_file") == 0)
        {
            char *input1 = new char[input[1].length() + 1];
            strcpy(input1, input[1].c_str());
            char *input2 = new char[input[2].length() + 1];
            strcpy(input2, input[2].c_str());

            thread upload_thread = thread(upload_function, input1, input2, accept_socket_ID);
            upload_thread.join();
        }
        else if (input[0].compare("download_file") == 0)
        {
            char *input1 = new char[input[1].length() + 1];
            strcpy(input1, input[1].c_str());

            thread upload_thread = thread(download_function, input1, accept_socket_ID);
            upload_thread.join();
        }
    }
    close(server_socket_ID);
    return 0;
}