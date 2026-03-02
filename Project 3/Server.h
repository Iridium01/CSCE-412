#pragma once

#ifndef SERVER_H
#define SERVER_H

#include "Request.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

class Server {
    private:
        Request current_request;
        bool is_busy = false;
        int remaining_time = 0;
        std::string output_log;
        std::string id;

        void log_event(int clock_cycle, const std::string& action) {
            std::ofstream of(output_log, std::ios::app);
            of << clock_cycle << ": [Server       " << std::left << std::setw(10) << id << "] " << action << '\n';
            of.close();
        }

    public:
        Server(const std::string& server_id, const std::string& log="output_log.txt")
            : id(server_id)
            , output_log(log) {}

        void process_request(const Request& req, int clock_cycle) {
            current_request = req;
            is_busy = true;
            remaining_time = req.time;
            std::string event_desc = "Received request (" + std::string(1, req.job_type) + " " + std::to_string(req.time) + "ms)";
            log_event(clock_cycle, event_desc);
        }

        void tick(int clock_cycle) {
            if (is_busy) {
                remaining_time--;
                if (remaining_time <= 0) {
                    is_busy = false; // Request is complete
                    std::string event_desc = "Completed request (" + std::string(1, current_request.job_type) + " " + std::to_string(current_request.time) + "ms)";
                    log_event(clock_cycle, event_desc);
                }
            }
        }

        bool is_free() const {
            return !is_busy;
        }

        std::string get_id() const {
            return id; // Return the server's unique ID
        }
};

#endif