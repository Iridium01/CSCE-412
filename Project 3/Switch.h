#pragma once

#ifndef SWITCH_H
#define SWITCH_H

#include "LoadBalancer.h"
#include "Request.h"
#include <string>
#include <vector>

/**
 * @class Switch
 * @brief Routes requests to appropriate load balancers and manages their operations.
 */
class Switch {
private:
    /**
     * @brief Load balancer for streaming jobs.
     */
    LoadBalancer streaming_lb;

    /**
     * @brief Load balancer for processing jobs.
     */
    LoadBalancer processing_lb;

public:
    /**
     * @brief Constructs a Switch with two load balancers.
     * @param streaming_id ID for the streaming load balancer.
     * @param streaming_servers Initial number of servers for the streaming load balancer.
     * @param processing_id ID for the processing load balancer.
     * @param processing_servers Initial number of servers for the processing load balancer.
     */
    Switch(const std::string& streaming_id, int streaming_servers, const std::string& processing_id, int processing_servers)
        : streaming_lb(streaming_id, streaming_servers)
        , processing_lb(processing_id, processing_servers) {
        std::ofstream of;
        of.open("output_log.txt", std::ios::out);
        of << "Starting Load Balancer and Server Log\n";
        of.close();
    }

    /**
     * @brief Routes a request to the appropriate load balancer.
     * @param req The request to route.
     * @param clock_cycle The current clock cycle.
     */
    void route_request(const Request& req, int clock_cycle) {
        if (req.job_type == 'S') {
            streaming_lb.add_request(req, clock_cycle);
        } else if (req.job_type == 'P') {
            processing_lb.add_request(req, clock_cycle);
        }
    }

    /**
     * @brief Advances the simulation by one clock cycle.
     * @param clock_cycle The current clock cycle.
     */
    void tick(int clock_cycle) {
        streaming_lb.tick(clock_cycle);
        processing_lb.tick(clock_cycle);
    }

    /**
     * @brief Print statistics for both load balancers.
     */
    void update_stats() {
        std::ofstream stats_file("load_balancer_stats.txt", std::ios::out);
        streaming_lb.update_stats(stats_file);
        processing_lb.update_stats(stats_file);
        stats_file.close();
    }

    /**
     * @brief Blocks an IP address in both load balancers.
     * @param ip The IP address to block.
     * @param clock_cycle The current clock cycle.
     */
    void block_ip(const std::string& ip, int clock_cycle) {
        streaming_lb.block_ip(ip, clock_cycle);
        processing_lb.block_ip(ip, clock_cycle);
    }
};

#endif