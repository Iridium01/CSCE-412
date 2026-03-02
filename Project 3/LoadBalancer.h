#pragma once

#ifndef LOAD_BALANCER_H

#include "Request.h"
#include "Server.h"
#include <vector>
#include <queue>
#include <fstream>
#include <string>
#include <algorithm>

/**
 * @class LoadBalancer
 * @brief Manages servers and request queues, dynamically adjusts server count, and logs events.
 */
class LoadBalancer {
    private:
        // functional variables
        std::vector<Server> servers;
        std::queue<Request> request_queue;
        std::string id;
        std::vector<std::string> blocked_ips;
        size_t server_id_counter = 1;

        // log variables
        std::string output_log;
        size_t total_requests = 0;
        size_t min_request_time = 2147483647;
        size_t max_request_time = 0;
        double avg_request_time = 0.0;
        size_t servers_created = 0;
        size_t servers_removed = 0;
        size_t max_servers = 0;

        /**
         * @brief Logs an event to the output log file.
         * @param clock_cycle The current clock cycle.
         * @param action Description of the event.
         */
        void log_event(int clock_cycle, const std::string& action) {
            std::ofstream of(output_log, std::ios::app);
            of << clock_cycle << ": [LoadBalancer " << std::left << std::setw(10) << id << "] " << action << '\n';
            of.close();
        }

        // helper function to add servers at initialization
        // and dynamically during simulation
        /**
         * @brief Adds a new server to the load balancer.
         * @param clock_cycle The current clock cycle.
         * @details The server ID is generated based on the load balancer type (streaming or processing) and a counter.
         */
        void add_server(int clock_cycle) {
            std::string server_id;
            if (id == "stream_lb") {
                server_id = "S" + std::to_string(server_id_counter++);
            }
            else {
                server_id = "P" + std::to_string(server_id_counter++);
            }

            servers.emplace_back(server_id, output_log);

            servers_created++;
            max_servers = std::max(max_servers, servers.size());
            std::string event_desc = "Server added: " + server_id;
            log_event(clock_cycle, event_desc);
        }
        
        // helper function to dynamically remove servers during simulation
        // should only remove the first inactive server
        /**
         * @brief Removes an inactive server from the load balancer.
         * @param clock_cycle The current clock cycle.
         * @details The function iterates through the list of servers and removes the first one that is not busy.
         */
        void remove_server(int clock_cycle) {
            for (size_t i = 0; i < servers.size(); i++) {
                if (servers[i].is_free()) {
                    std::string server_id = servers[i].get_id();
                    servers.erase(servers.begin() + i);

                    servers_removed++;
                    std::string event_desc = "Inactive server removed: " + server_id;
                    log_event(clock_cycle, event_desc);

                    return;
                }
            }
        }

    public:
        /**
         * @brief Constructs a LoadBalancer with the given ID and initial servers.
         * @param id Unique identifier for the load balancer.
         * @param initial_servers Number of servers to initialize.
         * @param log_file Path to the log file.
         */
        LoadBalancer(const std::string id, size_t initial_servers, const std::string& log_file = "output_log.txt")
            : id(id)
            , output_log(log_file) {
            
            // add initial servers
            for (size_t i = 0; i < initial_servers; i++) {
                add_server(0);
            }
        }

        /**
         * @brief Adds a request to the load balancer's queue.
         * @param req The request to add.
         * @param clock_cycle The current clock cycle.
         * @details If the request's source IP is in the blocked list, it will be rejected. Otherwise it is added to the queue.
         */
        void add_request(const Request& req, int clock_cycle) {
            if (std::find(blocked_ips.begin(), blocked_ips.end(), req.ip_in) != blocked_ips.end()) {
                std::string event_desc = "Blocked request (" + std::string(1, req.job_type) + " " + std::to_string(req.time) + "ms) from IP: " + req.ip_in;
                log_event(clock_cycle, event_desc);
                return;
            }

            request_queue.push(req);
            total_requests++;
            min_request_time = std::min(min_request_time, static_cast<size_t>(req.time));
            max_request_time = std::max(max_request_time, static_cast<size_t>(req.time));
            avg_request_time += req.time;
            std::string event_desc = "Request (" + std::string(1, req.job_type) + " " + std::to_string(req.time) + "ms) added";
            log_event(clock_cycle, event_desc);
        }
        
        /**
         * @brief Advances the simulation by one clock cycle, processing requests and adjusting servers as needed.
         * @param clock_cycle The current clock cycle.
         * @details The function checks each server for availability and assigns requests from the queue. It also dynamically adds or removes servers based on the queue size and current server count, with a cooldown period of 10 cycles to prevent rapid fluctuations.
         */
        void tick(int clock_cycle) {
            // Assign tasks and increment clock time for all servers
            for (auto& server : servers) {
                if (server.is_free() && !request_queue.empty()) {
                    std::string event_desc = "Request (" + std::string(1, request_queue.front().job_type) + " " + std::to_string(request_queue.front().time) + "ms) pushed to server: " + server.get_id();
                    log_event(clock_cycle, event_desc);

                    server.process_request(request_queue.front(), clock_cycle);
                    request_queue.pop();
                }
                else {
                    server.tick(clock_cycle);
                }
            }

            // Dynamically adjust num servers
            size_t queue_size = request_queue.size();
            if (queue_size > 80 * servers.size()) {
                static int wait_cycles = 0;

                if (++wait_cycles >= 10) {
                    add_server(clock_cycle);
                    wait_cycles = 0;
                }
            }
            else if (queue_size < 50 * servers.size() && servers.size() > 1) {
                static int wait_cycles = 0;

                if (++wait_cycles >= 10) {
                    remove_server(clock_cycle);
                    wait_cycles = 0;
                }
            }
        }

        /**
         * @brief Print statistics for the load balancer to a file.
         * @param stats_file An open ofstream to write the statistics to.
         */
        void update_stats(std::ofstream& stats_file) {
            if (stats_file.is_open()) {
                stats_file << "Load Balancer ID:    " << id << "\n";
                stats_file << "Start queue size:    " << total_requests << "\n";
                stats_file << "End queue size:      " << request_queue.size() << "\n";
                stats_file << "Min request time:    " << min_request_time << "ms\n";
                stats_file << "Max request time:    " << max_request_time << "ms\n";
                stats_file << "Avg request time:    " << (avg_request_time / total_requests) << "ms\n";
                stats_file << "Servers Created:     " << servers_created << "\n";
                stats_file << "Servers Removed:     " << servers_removed << "\n";
                stats_file << "Max Servers Running: " << max_servers << "\n\n";
            }
        }

        /**
         * @brief Blocks an IP address in both load balancers.
         * @param ip The IP address to block.
         * @param clock_cycle The current clock cycle.
         */
        void block_ip(const std::string& ip, int clock_cycle) {
            blocked_ips.push_back(ip);

            std::string event_desc = "Blocked IP: " + ip;
            log_event(clock_cycle, event_desc);
        }
};

#endif