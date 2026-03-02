# Load Balancer Simulation

## Project Description
This project simulates a load balancer system designed to manage server resources dynamically, handle incoming requests, and log system events. The simulation includes the following features:

- **Logging**: Detailed event logging for load balancers and servers.
- **Dynamic Server Management**: Servers are added or removed based on the request queue size.
- **IP Blocking**: Requests from specific IP addresses can be blocked.
- **Job Routing**: Requests are routed to different load balancers based on job type (e.g., streaming or processing).

## Key Components
- **LoadBalancer**: Manages servers, processes requests, and dynamically adjusts server count.
- **Server**: Represents individual servers that process requests.
- **Switch**: Routes requests to appropriate load balancers and manages their operations.
- **Request**: Represents a single request with attributes like IP addresses, job type, and processing time.

## How to Run
1. Compile the project using the provided Makefile.
2. Run the simulation and provide runtime parameters (e.g., number of servers, clock cycles).
3. View the generated logs and statistics for analysis.