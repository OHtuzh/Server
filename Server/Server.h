#pragma once

#include "../OMFL-parser/lib/parser.h"

#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include <filesystem>
#include <string>

#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>

const size_t kDatagramSize = 4;
typedef std::array<uint8_t, kDatagramSize> datagram_t;

class Server {
 private:
    sockaddr_in server_socket_;
    socklen_t server_address_length_;
    datagram_t datagram_;
    int socket_description_;
    size_t limit_;
    std::vector<sockaddr_in> receivers_;
    int current_;

 public:
    Server(const std::filesystem::path& config_file)
        :
        socket_description_(socket(AF_INET, SOCK_DGRAM, 0)),
        current_(0) {
        auto root = omfl::parse(config_file);
        if (!root.valid()) {
            throw std::runtime_error("Bad config file");
        }

        server_socket_.sin_family = AF_INET;
        server_socket_.sin_port = htons(root.Get("port").AsInt());
        server_socket_.sin_addr.s_addr = htonl(INADDR_ANY);

        limit_ = root.Get("limit").AsInt();
        int number_of_receivers = root.Get("number_of_receivers").AsInt();
        auto list_of_addresses = root.Get("receivers");
        for (int i = 0; i < number_of_receivers; i++) {
            std::string address = list_of_addresses[i].AsString();
            std::string ip{address.begin(), address.begin() + address.find(':')};

            sockaddr_in sock{};
            sock.sin_family = AF_INET;
            sock.sin_port = std::stoi(std::string{address.begin() + address.find(':') + 1, address.end()});
            inet_pton(AF_INET, ip.c_str(), &sock.sin_addr);
            receivers_.push_back(sock);
        }

    }

    inline ssize_t GetDatagram() {
        return recvfrom(socket_description_,
                        datagram_.data(),
                        kDatagramSize,
                        0,
                        (sockaddr*) &server_socket_,
                        &server_address_length_);
    }

    inline void ReadDatagram() {
        while (GetDatagram() != 4) {}
    }

    void SendDatagram() {
        sendto(
            socket_description_,
            datagram_.data(),
            kDatagramSize,
            0,
            (sockaddr*) &receivers_[current_],
            sizeof(server_socket_));
        current_ = (current_ + 1) % receivers_.size();
    };

    void Work() {
        using namespace std::chrono_literals;
        limit_ *= receivers_.size();

        server_address_length_ = sizeof(server_socket_);
        size_t executed = 0;
        std::chrono::high_resolution_clock::time_point start;
        while (true) {
            ReadDatagram();
            SendDatagram();
            executed++;
            if (std::chrono::high_resolution_clock::time_point() - start >= 1s) {
                executed = 0;
                start = std::chrono::high_resolution_clock::time_point();
            }
            if (executed == limit_) {
                std::this_thread::sleep_for(1s - (std::chrono::high_resolution_clock::time_point() - start));
                start = std::chrono::high_resolution_clock::time_point();
                executed = 0;
                start = std::chrono::high_resolution_clock::time_point();
            }
        }

    }

};

