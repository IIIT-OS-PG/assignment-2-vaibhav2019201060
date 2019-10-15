#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <thread>
using namespace std;
long filesize = 0;

void *server_thread(void *arg)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int *temp_port = (int *)arg;
    int port_no = *temp_port;
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
    serveraddr.sin_port = htons(port_no);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //cout << "this is server address " << serveraddr.sin_addr.s_addr << endl;

    int bind_ID = bind(server_socket_ID, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if (bind_ID < 0)
    {
        cout << "Error in Binding in Server" << endl;
        exit(1);
    }
    int listen_ID = listen(server_socket_ID, 10);
    if (listen_ID < 0)
    {
        cout << "Error in listening on assigned port" << endl;
    }
    //int accept_socket_ID = accept(server_socket_ID, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    while (1)
    {
        int accept_socket_ID = accept(server_socket_ID, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (accept_socket_ID < 0)
        {
            cout << "Can't complete access call on server" << endl;
            exit(1);
        }
        else
        {
            //cout << "accept call completed on server" << endl;
        }
        int recv_data;
        char data_buffer[10000];
        recv_data = recv(accept_socket_ID, data_buffer, sizeof(data_buffer), 0);
        if (recv_data < 0)
        {
            cout << "Error in receiving the data" << endl;
        }
        data_buffer[recv_data] = '\0';
        printf("download request for file: %s", data_buffer);
        FILE *fp = fopen(data_buffer, "rb");
        //clientthreads[i] = thread(client_function, download_ports[i], filename);
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        rewind(fp);
        fclose(fp);
        string filesizestring = to_string(file_size);
        int n = filesizestring.length() + 1;
        char filesizechar[n];
        strcpy(filesizechar, filesizestring.c_str());
        //char *msg = "Hello world";
        int send_data = send(accept_socket_ID, filesizechar, sizeof(filesizechar), 0);
        if (send_data < 0)
        {
            cout << "Could not send data" << endl;
        }
        char pno_buffer[10000];
        int recv_pno;
        recv_pno = recv(accept_socket_ID, pno_buffer, sizeof(pno_buffer), 0);
        if (recv_pno < 0)
        {
            cout << "Error in receiving the data" << endl;
        }
        pno_buffer[recv_pno] = '\0';
        char *token = strtok(pno_buffer, " ");
        string input[1000];
        int kl = 0;
        while (token != NULL)
        {
            input[kl] = token;
            kl++;
            //printf("token %s\n", token);
            token = strtok(NULL, " ");
        }
        int cno = stoi(input[1]);
        FILE *fc;
        fc = fopen(data_buffer, "r");
        int psize;
        if ((ftell(fc) - (cno * 10)) < 10)
        {
            psize = ftell(fc) - (cno * 10);
        }
        else
        {
            psize = 10;
        }
        char send_part[10000];
        int totalread;
        fseek(fc, cno * 10, SEEK_SET);
        //long long int chunk_size = minAmong(ftell(fp) - chunk_number * CHUNK_SIZE, CHUNK_SIZE) while (chunk_size > 0 && (number_of_characters = fread(buffer, sizeof(char), chunk_size, fp)) > 0)
        while (psize > 0 && (totalread = fread(send_part, sizeof(char), psize, fp)) > 0)
        {
            if (send(fc, send_part, psize, 0) == -1)
            {
                perror("Error: Sending chunk data");
                //pthread_exit(NULL);
            }
            bzero(send_part, 10);
            psize -= totalread;
        }
        fclose(fc);
        int send_part_final = send(accept_socket_ID, send_part, sizeof(send_part), 0);
        if (send_part_final < 0)
        {
            cout << "Could not send data" << endl;
        }
    }
    close(server_socket_ID);
}
void download_function(int port, int pno, char *filename)
{
    int sock = 0;
    char strmsg[10000];
    //string addr = "127.0.0.1";
    //char *IAddr = "127.0.0.1";
    char buffer[1024] = {0};
    int client_socket_ID = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket_ID < 0)
    {
        cout << "Cannot Create socket in client" << endl;
        exit(1);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Convert IPv4 and IPv6 addresses from text to binary form
    /* if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    } */

    if (connect(client_socket_ID, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        //printf("\nConnection Failed \n");
        cout << "Client couldn't connect" << endl;
        //exit(1);
    }
    else
    {
        string pcno = to_string(pno);
        string filenamee = filename;
        string send_data = pcno + " " + filenamee;
        int n = send_data.length();
        char char_data[n + 1];
        strcpy(char_data, send_data.c_str());
        int download_data = send(client_socket_ID, char_data, sizeof(char_data), 0);
        if (download_data < 0)
        {
            cout << "Could not send data" << endl;
        }
        FILE *fc;
        fc = fopen(filename, "r+");
        int final_data_count;
        char final_buffer[10000];
        final_data_count = recv(client_socket_ID, final_buffer, sizeof(final_buffer), 0);
        if (final_data_count < 0)
        {
            cout << "Error in receiving the data" << endl;
        }
        final_buffer[final_data_count] = '\0';
        int psize;
        if ((ftell(fc) - (pno * 10)) < 10)
        {
            psize = ftell(fc) - (pno * 10);
        }
        else
        {
            psize = 10;
        }
        fseek(fc, pno * 10, SEEK_SET);
        char final_download_buffer[10000];
        int write_chars = recv(client_socket_ID, final_download_buffer, psize, 0);
        while (write_chars > 0)
        {
            fwrite(final_download_buffer, sizeof(char), write_chars, fc);
            write_chars -= psize;
            bzero(final_download_buffer, psize);
        }
        fclose(fc);
    }
}
void client_function(int port, string filename)
{
    int sock = 0;
    char strmsg[10000];
    //string addr = "127.0.0.1";
    //char *IAddr = "127.0.0.1";
    char buffer[1024] = {0};
    int client_socket_ID = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket_ID < 0)
    {
        cout << "Cannot Create socket in client" << endl;
        exit(1);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Convert IPv4 and IPv6 addresses from text to binary form
    /* if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    } */

    if (connect(client_socket_ID, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        //printf("\nConnection Failed \n");
        cout << "Client couldn't connect" << endl;
        //exit(1);
    }
    else
    {
        string data = filename;
        cout << "string for tracker is: " << data << endl;
        int n = data.length();
        char char_data[n + 1];
        strcpy(char_data, data.c_str());
        int send_data = send(client_socket_ID, char_data, sizeof(char_data), 0);
        if (send_data < 0)
        {
            cout << "Could not send data" << endl;
        }
        cout << "waiting for server" << endl;
        int recv_data;
        char data_buffer[10000];
        recv_data = recv(client_socket_ID, data_buffer, sizeof(data_buffer), 0);
        if (recv_data < 0)
        {
            cout << "Error in receiving the data" << endl;
        }
        data_buffer[recv_data] = '\0';
        //close(client_socket_ID);
        //printf("%s\n", buffer);
        //string file_size = data_buffer;
        char *temp;
        long filesizetemp = strtol(data_buffer, &temp, 10);
        if (filesize < filesizetemp)
        {
            filesize = filesizetemp;
        }
        if (*temp != '\0')
        {
            cout << "cannot convert filesize from server" << endl;
        }
        //printf("received size from %d port: %l\n", port, filesize);
        cout << "Received size from " << port << " port: " << filesize << endl;
    }
    close(client_socket_ID);
}
void client_tracker_function(int port, string command, string filename, int port_server)
{
    int sock = 0;
    //string addr = "127.0.0.1";
    //char *IAddr = "127.0.0.1";
    char buffer[1024] = {0};
    int client_socket_ID = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket_ID < 0)
    {
        cout << "Cannot Create socket in client" << endl;
        exit(1);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Convert IPv4 and IPv6 addresses from text to binary form
    /* if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    } */

    cout << "port number for tracker is " << port << endl;

    if (connect(client_socket_ID, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        //printf("\nConnection Failed \n");
        cout << "Client couldn't connect" << endl;
        //exit(1);
    }
    else
    {
        if (command.compare("upload_file") == 0)
        {
            string portserver = to_string(port_server);
            string data = command + " " + filename + " " + portserver;
            cout << "string for tracker is: " << data << endl;
            int n = data.length();
            char char_data[n + 1];
            strcpy(char_data, data.c_str());
            int send_data = send(client_socket_ID, char_data, sizeof(char_data), 0);
            if (send_data < 0)
            {
                cout << "Could not send data" << endl;
            }
            int recv_data;
            char data_buffer[10000];
            recv_data = recv(client_socket_ID, data_buffer, sizeof(data_buffer), 0);
            if (recv_data < 0)
            {
                cout << "Error in receiving the data" << endl;
            }
            data_buffer[recv_data] = '\0';
            close(client_socket_ID);
            //printf("%s\n", buffer);
            printf("this is client: received from tracker '%s'\n", data_buffer);
        }
        else if (command.compare("download_file") == 0)
        {
            string portserver = to_string(port_server);
            string data = command + " " + filename + " " + portserver;
            cout << "string for tracker is: " << data << endl;
            int n = data.length();
            char char_data[n + 1];
            strcpy(char_data, data.c_str());
            int xx = filename.length();
            char char_filename[n + 1];
            strcpy(char_filename, filename.c_str());
            int send_data = send(client_socket_ID, char_data, sizeof(char_data), 0);
            if (send_data < 0)
            {
                cout << "Could not send data" << endl;
            }
            else
            {
                cout << "download requested" << endl;
            }
            int recv_data;
            char data_buffer[10000];
            recv_data = recv(client_socket_ID, data_buffer, sizeof(data_buffer), 0);
            if (recv_data < 0)
            {
                cout << "Error in receiving the data" << endl;
            }
            data_buffer[recv_data] = '\0';
            close(client_socket_ID);
            //printf("%s\n", buffer);
            printf("this is client: received from tracker '%s'\n", data_buffer);
            int download_ports[1000];
            int k = 0;
            char *token = strtok(data_buffer, " ");
            while (token != NULL)
            {
                download_ports[k] = stoi(token);
                k++;
                printf("token %s\n", token);
                token = strtok(NULL, " ");
            }
            for (int i = 0; i < k; i++)
            {
                cout << "file port " << download_ports[i] << endl;
            }
            thread clientthreads[k];
            for (int i = 0; i < k; i++)
            {
                clientthreads[i] = thread(client_function, download_ports[i], filename);
            }
            for (int i = 0; i < k; i++)
            {
                clientthreads[i].join();
            }
            cout << "filesize is " << filesize << endl;
            int partsize = 10;
            long numberofparts = filesize / partsize;
            if (filesize % partsize != 0)
            {
                numberofparts++;
            }
            FILE *dp = fopen(char_filename, "wb");
            fseek(dp, filesize - 1, SEEK_SET);
            fputc('\0', dp);
            rewind(dp);
            fclose(dp);
            thread downloadthreads[k];
            for (int i = 0; i < numberofparts; i++)
            {
                x = i % k;
                downloadthreads[x] = thread(download_function, download_ports[x], i, char_filename);
                if (x == (k - 1))
                {
                    for (int jj = 0; jj < k; jj++)
                    {
                        downloadthreads[jj].join();
                    }
                }
            }
        }
    }
    close(client_socket_ID);
}
void *client_thread(void *arg)
{
    while (1)
    {
        int tracker_port = 10000;
        cout << "Give input or output command with filename: ";
        string command;
        cin >> command;
        string filename;
        cin >> filename;
        int port_server;
        cin >> port_server;
        if (command.compare("upload_file") == 0)
        {
            thread client_upload_thread = thread(client_tracker_function, tracker_port, command, filename, port_server);
            client_upload_thread.join();
        }
        else if (command.compare("download_file") == 0)
        {
            thread client_download_thread = thread(client_tracker_function, tracker_port, command, filename, port_server);
            client_download_thread.join();
        }
        else
        {
            cout << "command is wrong" << endl;
        }
    }
}

int main(int argc, char const *argv[])
{
    int portno;
    cout << "Enter port no for server ";
    cin >> portno;
    //int yes = 1;
    pthread_t thread_server;
    pthread_create(&thread_server, NULL, server_thread, &portno);
    pthread_t thread_client;
    pthread_create(&thread_client, NULL, client_thread, NULL);

    pthread_join(thread_server, NULL);
    pthread_join(thread_client, NULL);
}