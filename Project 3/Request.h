#pragma once
#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @struct Request
 * @brief Represents a request with input/output IPs, processing time, and job type.
 */
struct Request {
    std::string ip_in;
    std::string ip_out;
    int time;
    char job_type;
};

#endif