#include "Switch.h"
#include "ini.h"
#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <map>

using namespace std;

/**
 * @file main.cpp
 * @brief Entry point for the Load Balancer simulation.
 */

/**
 * @brief Main function to run the simulation.
 * @return int Exit status of the program.
 */
int main() {
    // Parse the config file
    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;
    file.read(ini);

    // Extract RequestGenerator configuration
    string ip_in = ini.get("RequestGenerator").get("ip_in");
    string ip_out = ini.get("RequestGenerator").get("ip_out");
    int min_time = stoi(ini.get("RequestGenerator").get("min_time"));
    int max_time = stoi(ini.get("RequestGenerator").get("max_time"));
    vector<char> job_types = {'P', 'S'};

    // Get user input for initial setup
    int runtime;
    size_t num_servers;

    cout << "Enter the runtime (in clock cycles): ";
    cin >> runtime;

    cout << "Enter the initial number of servers: ";
    cin >> num_servers;

    // Initialize the Switch with two load balancers
    Switch job_switch("stream_lb", num_servers, "process_lb", num_servers);

    // Block IPs from config
    for (auto it = ini["BlockedIPs"].begin(); it != ini["BlockedIPs"].end(); it++) {
        job_switch.block_ip(it->second, 0);
    }

    // Random number generators for requests
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> time_dist(min_time, max_time);
    uniform_int_distribution<> job_type_dist(0, 1);

    // Fill the queue with initial requests
    for (size_t i = 0; i < num_servers * 100; i++) {
        Request req = {
            ip_in, ip_out, time_dist(gen), job_types[job_type_dist(gen)]
        };
        job_switch.route_request(req, 0);
    }

    cout << "Added " << num_servers * 100 << " initial requests to the load balancer." << endl;

    /**
     * @brief Starts the simulation loop for the specified runtime.
     */
    // Adjust the simulation loop to use user-defined runtime
    cout << "Starting simulation for " << runtime << " clock cycles..." << endl;

    for (int clock = 1; clock <= runtime; clock++) {
        job_switch.tick(clock);
    }

    cout << "Simulation completed." << endl;

    // Update load balancer stats at the end of the simulation
    job_switch.update_stats();
    cout << "Load balancer statistics updated in load_balancer_stats.txt" << endl;

    return 0;
}