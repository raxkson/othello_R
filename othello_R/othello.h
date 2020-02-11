#ifndef OTHELLO_H
#define OTHELLO_H
#include "client.h"
#include "msg.h"
#include <iostream>

class othello {
public:

    std::string default_ip = "127.0.0.1";
    int default_port = 6068;
    othello()
        :client()
    {

    }
    /*
    othello()
        :client(default_ip, default_port)
        , board(), color(), token()
    {
        auto accept_msg = client->recv_data();
        auto codendata = parse_msg(accept_msg);
        auto code = codendata.first;
        auto data = codendata.second;

        if (code != "accept") {
            std::cout << "ERROR: accept msg error:" << accept_msg << std::endl;
            exit(-1);
        }

        board = data["board"];
        color = data["color"];
        token = data["token"];
    }
    */

    othello(std::string ip, int port)
        : client(new Client(ip, port))
        , board(), color(), token()
    {
        //log.write("Server connection success!");
        std::cout << "Server connection success!" << std::endl;
        auto accept_msg = client->recv_data();
        auto codendata = parse_msg(accept_msg);
        auto code = codendata.first;
        auto data = codendata.second;

        if (code != "accept") {
            std::cout << "ERROR: accept msg error:" << accept_msg << std::endl;
            exit(-1);
        }

        board = data["board"];
        color = data["color"];
        token = data["token"];

    }

    std::pair<std::string, std::map<std::string, std::string> >
    wait_for_turn()
    {
        std::cout << "wait for turn" << std::endl;
        while (true)
        {
            auto msg = client->recv_data();
            auto codendata = parse_msg(msg);
            auto code = codendata.first;
            auto data = codendata.second;
            std::cout << "print code: " <<code << std::endl;
            if (code == "turn") {
                std::cout << "turn :" << data["available"] << std::endl;
                return std::make_pair(code, data);
            }
            else if (code == "update") {
                std::cout << "now updating" << std::endl;
                board = data["board"];
                return std::make_pair(code, data);
            }
            else if (code == "end") {
                return std::make_pair(code, data);
            }
            else {
                std::cout << "invalid code " + code << std::endl;
                exit(-1);
            }
        }
    }

    void move(std::string& cell_corr)
    {
        auto msg = gen_move_msg(cell_corr, token);
        client->send_data(msg);
    }

    std::string board, color;
private:
    Client *client;
    std::string token;
};
#endif // OTHELLO_H
