#pragma once

#ifndef SWITCH_H
#define SWITCH_H

#include "LoadBalancer.h"
#include "Request.h"
#include <string>
#include <vector>

class Switch {
private:
    LoadBalancer streaming_lb;
    LoadBalancer processing_lb;

public:
    Switch(const std::string& streaming_id, int streaming_servers, const std::string& processing_id, int processing_servers)
        : streaming_lb(streaming_id, streaming_servers)
        , processing_lb(processing_id, processing_servers) {
        std::ofstream of;
        of.open("output_log.txt", std::ios::out);
        of << "Starting Load Balancer and Server Log\n";
        of.close();
    }

    void route_request(const Request& req, int clock_cycle) {
        if (req.job_type == 'S') {
            streaming_lb.add_request(req, clock_cycle);
        } else if (req.job_type == 'P') {
            processing_lb.add_request(req, clock_cycle);
        }
    }

    void tick(int clock_cycle) {
        streaming_lb.tick(clock_cycle);
        processing_lb.tick(clock_cycle);
    }

    void update_stats() {
        std::ofstream stats_file("load_balancer_stats.txt", std::ios::out);
        streaming_lb.update_stats(stats_file);
        processing_lb.update_stats(stats_file);
        stats_file.close();
    }

    void block_ip(const std::string& ip, int clock_cycle) {
        streaming_lb.block_ip(ip, clock_cycle);
        processing_lb.block_ip(ip, clock_cycle);
    }
};

#endif