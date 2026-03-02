#pragma once

#ifndef SERVER_H
#define SERVER_H

#include "Request.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

/**
 * @class Server
 * @brief Represents a server that processes requests.
 */
class Server {
    private:
        /**
         * @brief The current request being processed by the server.
         */
        Request current_request;

        /**
         * @brief Indicates whether the server is busy processing a request.
         */
        bool is_busy = false;

        int remaining_time = 0;
        std::string output_log;
        std::string id;

        /**
         * @brief Logs an event to the output log file.
         * @param clock_cycle The current clock cycle.
         * @param action Description of the event.
         */
        void log_event(int clock_cycle, const std::string& action) {
            std::ofstream of(output_log, std::ios::app);
            of << clock_cycle << ": [Server       " << std::left << std::setw(10) << id << "] " << action << '\n';
            of.close();
        }

    public:
        /**
         * @brief Constructs a Server with the given ID and log file.
         * @param server_id Unique identifier for the server.
         * @param log Path to the log file.
         */
        Server(const std::string& server_id, const std::string& log="output_log.txt")
            : id(server_id)
            , output_log(log) {}

        /**
         * @brief Processes a request.
         * @param req The request to process.
         * @param clock_cycle The current clock cycle.
         */
        void process_request(const Request& req, int clock_cycle) {
            current_request = req;
            is_busy = true;
            remaining_time = req.time;
            std::string event_desc = "Received request (" + std::string(1, req.job_type) + " " + std::to_string(req.time) + "ms)";
            log_event(clock_cycle, event_desc);
        }
        

        /**
         * @brief Advances the server's internal clock by one cycle.
         * @param clock_cycle The current clock cycle.
         */
        void tick(int clock_cycle) {
            if (is_busy) {
                remaining_time--;
                if (remaining_time <= 0) {
                    is_busy = false;
                    std::string event_desc = "Completed request (" + std::string(1, current_request.job_type) + " " + std::to_string(current_request.time) + "ms)";
                    log_event(clock_cycle, event_desc);
                }
            }
        }

        /**
         * @brief Checks if the server is free.
         * @return True if the server is not busy, false otherwise.
         */
        bool is_free() const {
            return !is_busy;
        }

        /**
         * @brief Gets the ID of the server.
         * @return The server ID.
         */
        std::string get_id() const {
            return id;
        }
};

#endif