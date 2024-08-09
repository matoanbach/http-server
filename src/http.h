#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <map>
#include <sys/time.h>
#include <queue>

#include <algorithm>
#include <cctype>

using namespace std;

const int BUF_SIZE = 1024;
const string OK = "HTTP/1.1 200 OK\r\n\r\n";
const string NOT_FOUND = "HTTP/1.1 404 Not Found\r\n\r\n";
class HTTP
{
    // GLOBAL VARIABLES
    bool is_running = true;
    vector<pthread_t> thread_ids;
    pthread_t sndtid;
    vector<int> clients;

    int myfd;
    int my_port = 4221;
    struct sockaddr_in myaddr;

    // Message queues
    queue<string> client_queue;

    // struct sockaddr_in server_addr;
    pthread_mutex_t lock_x;

    void *my_recv_func(void *arg);     // non static means each thread will run each of this function
    static void *recv_func(void *arg); // static means we can reuse this function many times

    int handle_command(int, string);
    int on_get(int, map<string, vector<string>>);
    int on_post(int, map<string, vector<string>>);
    int on_echo(int fd, string echo_string);

    // String manipulation
    map<string, vector<string>> extract(string, string);
    vector<string> extract2(string, string);

    HTTP();

    static HTTP *instance;

public:
    static HTTP *getInstance();
    int run(int, char **);
};