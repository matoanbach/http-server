#include "http.h"
using namespace std;

HTTP *HTTP::instance = nullptr;

HTTP::HTTP() {};

void *HTTP::recv_func(void *arg)
{
    return instance->my_recv_func(arg);
}

void *HTTP::my_recv_func(void *arg)
{
    int fd = *(int *)arg;
    char buf[BUF_SIZE];
    string message = "";
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    cout << "server: started my_receive_func()" << endl;
    while (is_running)
    {
        memset(buf, 0, sizeof(buf));
        int len = recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *)&addr, &addr_len);
        if (len > 0)
        {
            handle_command(fd, buf);
        }
    }
    pthread_exit(NULL);
}

int HTTP::handle_command(int fd, string raw_buf_string)
{
    map<string, vector<string>> package = extract(raw_buf_string, "\r\n");
    if (package.find("get") != package.end())
        return on_get(fd, package);
    else if (package.find("post") != package.end())
        return on_post(fd, package);
    return -1;
}

int HTTP::on_get(int fd, map<string, vector<string>> package)
{
    vector<string> endpoints = extract2(package["get"][0], "/");
    // for (auto pair : package)
    // {
    //     cout << pair.first << " ";
    //     // cout << pair.second.size();
    //     for (auto e : pair.second)
    //         cout << e << " ";
    //     cout << endl;
    // }

    if (endpoints[0] == "")
    {
        cout << "/" << endl;
        return send(fd, OK.c_str(), OK.size(), 0);
    }
    else if (endpoints[0] == "echo")
    {
        cout << "/echo" << endl;
        return on_echo(fd, endpoints[1]);
    }
    else if (endpoints[0] == "user-agent")
    {
        return on_echo(fd, package["user-agent"][0]);
    }
    else
        return send(fd, NOT_FOUND.c_str(), NOT_FOUND.size(), 0);
    return -1;
}

int HTTP::on_echo(int fd, string echo_string)
{
    string echo_template = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ";
    echo_template += to_string(echo_string.size()) + "\r\n\r\n" + echo_string;

    return send(fd, echo_template.c_str(), echo_template.size(), 0);
}

int HTTP::on_post(int, map<string, vector<string>>)
{
    return 0;
}

HTTP *HTTP::getInstance()
{
    if (instance == nullptr)
    {
        instance = new HTTP();
    }
    return instance;
}

map<string, vector<string>> HTTP::extract(string raw_buf_string, string delimeter)
{
    map<string, vector<string>> tokens;
    size_t start = 0;
    size_t end = raw_buf_string.find(delimeter);
    while (end != string::npos)
    {
        // cout << raw_buf_string.substr(start, end - start);
        vector<string> string_array = extract2(raw_buf_string.substr(start, end - start), " ");

        // string field = string_array[0];
        std::transform(string_array[0].begin(), string_array[0].end(), string_array[0].begin(), [](unsigned char c)
                       { return tolower(c); }); // to lower case

        // each field will have ":" to be ignored
        string field = (strncmp(&string_array[0].back(), ":", 1) == 0) ? string_array[0].substr(0, string_array[0].size() - 1) : string_array[0];
        vector<string> value(string_array.begin() + 1, string_array.end());

        tokens.insert({field, value});
        start = end + 2;
        end = raw_buf_string.find(delimeter, start);
    }
    for (auto pair : tokens)
    {
        cout << pair.first << " ";
        // cout << pair.second.size();
        for (auto e : pair.second)
            cout << e << " ";
        cout << endl;
    }
    return tokens;
}

vector<string> HTTP::extract2(string raw_buf_string, string delimeter)
{
    vector<string> tokens;
    size_t start = 0;
    size_t end = raw_buf_string.find(delimeter);
    while (end != string::npos)
    {
        if (raw_buf_string.substr(start, end - start).size() > 0)
            tokens.push_back(raw_buf_string.substr(start, end - start));
        start = end + 1;
        end = raw_buf_string.find(delimeter, start);
    }

    tokens.push_back(raw_buf_string.substr(start));
    return tokens;
}

int HTTP::run(int argc, char **argv)
{
    cout << unitbuf;
    cerr << unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    cout << "Logs from your program will appear here!\n";

    // int myfd = socket(AF_INET, SOCK_STREAM, 0); // O indicates default a protocol
    myfd = socket(AF_INET, SOCK_STREAM, 0); // O indicates default a protocol

    if (myfd < 0)
    {
        cerr << "Failed to create server socket\n";
        return 1;
    }
    cout << "1" << endl;
    // mind this error (Address already in use) at this point
    int reuse = 1;
    if (setsockopt(myfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)))
    {
        cerr << "setsockopt failed\n";
        return 1;
    }

    // struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(4221);
    cout << "2" << endl;
    if (bind(myfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
    {
        cerr << "Failed to bind to port " << my_port << "\n";
        return 1;
    }
    cout << "3" << endl;
    int connection_backlog = 5;
    if (listen(myfd, connection_backlog))
    {
        cerr << "listen failed\n";
        return 1;
    }
    cout << "4" << endl;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while (true)
    {
        cout << "is_running" << endl;
        client_fd = accept(myfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd >= 0)
        {
            pthread_t new_tid;
            thread_ids.push_back(new_tid);
            clients.push_back(client_fd);
            pthread_create(&new_tid, NULL, recv_func, &client_fd);
        }
    }

    for (auto e : clients)
        close(e);
    for (auto e : thread_ids)
        pthread_join(e, NULL);

    pthread_join(sndtid, NULL);
    close(myfd);
    return 0;
}
