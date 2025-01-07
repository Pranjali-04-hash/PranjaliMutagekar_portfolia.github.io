//Header Files
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <tuple>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <set>
#include <utility>
#include <string>
#include <cmath>
#include <unordered_map>
#include <numeric>
#include <conio.h>


using namespace std;

// Feedback class to handle general user feedback
// Constants for max agents and orders
const int MAX_AGENTS = 100;
const int numLocations = 30; // there are 30 locations


// Structure to represent an agent
struct Agent
{
    int id;
    std::string name;
    std::string phone;
    bool isAvailable;
    int assignedOrders;
    int completedOrders;
};


// Structure to represent an order
struct Order
{
    int id;
    int agentID;
    std::string description;
    bool isCompleted;
    std::time_t assignedTime;
    std::time_t completionTime;
};


// Structure for a vehicle
struct Vehicle
{
    int id;
    int currentLocation; // Current location index of the vehicle
    double maxCapacity;
    double currentLoad;
    bool isAvailable;
};


// Structure for an order,for assigning to vehicles
struct Order2
{
    int id;
    int deliveryLocation; // Delivery location index
    double weight;
};


// Assuming agentMap and agentOrders are defined globally
extern std::map<int, Agent> agentMap;
extern std::map<int, std::vector<Order>> agentOrders;


// Helper function to format time
std::string formatTime(std::time_t time)
{
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}


// Global variables
std::vector<Agent> agents;
std::map<int, Agent> agentMap; // Map to quickly access agents by ID
std::vector<Order> orders;
std::map<int, std::vector<Order>> agentOrders; // Map to store orders assigned to each agent


// Define location names
std::map<int, std::string> locationNames =
{

    {0, "Angol"},
    {1, "Udyambag"},
    {2, "Kamalapur"},
    {3, "Desur"},
    {4, "Channamma Circle"},
    {5, "Bagalkot Galli"},
    {6, "Gokak Galli"},
    {7, "Gadag Galli"},
    {8, "Jamboti Galli"},
    {9, "Khanbhag"},
    {10, "Tilakwadi"},
    {11, "Malmaruti"},
    {12, "APMC Yard"},
    {13, "Fort Galli"},
    {14, "Gandhi Bazar"},
    {15, "Shivaji Nagar"},
    {16, "Venkatapur"},
    {17, "Ambedkar Nagar"},
    {18, "Samyukta Nagar"},
    {19, "RPD Cross"},
    {20, "Vidhya Nagar"},
    {21, "Azad Nagar"},
    {22, "Desai Galli"},
    {23, "Jayanagar"},
    {24, "Anand Nagar"},
    {25, "Ramteerth Nagar"},
    {26, "Mahalaxmi Nagar"},
    {27, "Vidhyanagar"},
    {28, "Katipalla"},
    {29, "Murarji Peth"}

};


// 30x30 distance matrix (example values, replace with actual distances)
std::vector<std::vector<double>>dist =
{

    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7},
    {5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6},
    {6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5},
    {7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4},
    {8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1},
    {1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7},
    {5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6},
    {6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5},
    {7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4},
    {8, 7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7},
    {5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6},
    {6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5},
    {7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4},
    {8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1}

};


std::unordered_map<int, std::string> unorderedLocationNames(locationNames.begin(), locationNames.end());


// Function prototypes
void saveAgents();
void loadAgents();
void loadOrders();
void displayMainMenu();
void displayAgentMenu();
void displayOrderMenu();
void registerAgent();
void loginAgent();
void updateAvailability(int agentID, bool available);
void assignOrderToAgent(int agentID, const std::string& description);
void displayAgentList();
void displayAgentDetails(int agentID);
void displayAgentOrderHistory(int agentID);
void displayAgentPerformanceStatistics();
void displayOrderList();
void displayOrderDetails(int orderID);
void markOrderAsCompleted(int orderID);
bool validateLocations(int source, int destination, const std::vector<std::vector<double>>& distanceMatrix);
double adjustSpeedForTimeOfDay(double averageSpeed, const std::string& timeOfDay);
std::vector<std::vector<std::pair<int, int>>> calculateDeliveryTime(const std::vector<std::vector<double>>& distanceMatrix, double adjustedSpeed);
void displayOptimalRoute(const std::vector<std::vector<double>>& distanceMatrix, int source, int destination);
void displayRouteDetails(const std::vector<std::vector<double>>& distanceMatrix, const std::vector<std::vector<std::pair<int, int>>>& deliveryTimeMatrix, double costPerKm, double costPerHour, double fuelEfficiency, int source, int destination);
void displayLocations(const std::unordered_map<int, std::string>& locationNames); // Prototype for displayLocations
void assignVehiclesToOrders(std::vector<Vehicle>& vehicles, std::vector<Order2>& orders, const std::vector<std::vector<double>>& dist, const std::unordered_map<int, std::string>& locationNames);
void resetVehicle(Vehicle& vehicle, int initialLocation);
void updateVehicleLocation(Vehicle& vehicle, int newLocation, const std::unordered_map<int, std::string>& locationNames);
void dijkstra(int source, std::vector<int>& distances, std::vector<bool>& visited, std::vector<int>& previous);
std::vector<int> reconstructPath(int source, int destination, const std::vector<int>& previous);
void savePathToFile(const std::vector<int>& path, const std::unordered_map<int, std::string>& locationNames, const std::string& filename);
std::vector<int> loadPathFromFile(const std::string& filename);
void visualizeMap(const std::vector<int>& path);


int module3()
{

    // Populate location names for demonstration
    locationNames =
    {

        {0, "Angol"},
        {1, "Udyambag"},
        {2, "Kamalapur"},
        {3, "Desur"},
        {4, "Channamma Circle"},
        {5, "Bagalkot Galli"},
        {6, "Gokak Galli"},
        {7, "Gadag Galli"},
        {8, "Jamboti Galli"},
        {9, "Khanbhag"},
        {10, "Tilakwadi"},
        {11, "Malmaruti"},
        {12, "APMC Yard"},
        {13, "Fort Galli"},
        {14, "Gandhi Bazar"},
        {15, "Shivaji Nagar"},
        {16, "Venkatapur"},
        {17, "Ambedkar Nagar"},
        {18, "Samyukta Nagar"},
        {19, "RPD Cross"},
        {20, "Vidhya Nagar"},
        {21, "Azad Nagar"},
        {22, "Desai Galli"},
        {23, "Jayanagar"},
        {24, "Anand Nagar"},
        {25, "Ramteerth Nagar"},
        {26, "Mahalaxmi Nagar"},
        {27, "Vidhyanagar"},
        {28, "Katipalla"},
        {29, "Murarji Peth"}

    };

    // Initialize the distance matrix with some values (example)
    dist =
    {

    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7},
    {5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6},
    {6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5},
    {7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4},
    {8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1},
    {1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7},
    {5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6},
    {6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5},
    {7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4},
    {8, 7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1},
    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
    {2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8},
    {4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7},
    {5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6},
    {6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5},
    {7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4},
    {8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1}

    };

    loadAgents();
    loadOrders();

    // Convert distance matrix to double type
    std::vector<std::vector<double>> distanceMatrix(dist.begin(), dist.end());

   int mainChoice; // Declare mainChoice
    while (true)
    {

        displayMainMenu();
        std::cin >> mainChoice;

        switch (mainChoice)
        {

            case 1: // Agent Menu
            {
                int agentChoice;
                while (true)
                {
                    displayAgentMenu();
                    std::cin >> agentChoice;

                    switch (agentChoice)
                    {
                        case 1:
                            registerAgent();
                            break;
                        case 2:
                            loginAgent();
                            break;
                        case 3:
                        {
                            int agentID;
                            bool available;
                            std::cout << "Enter Agent ID: ";
                            std::cin >> agentID;
                            std::cout << "Enter availability (1 for available, 0 for unavailable): ";
                            std::cin >> available;
                            updateAvailability(agentID, available);
                            break;
                        }
                        case 4:
                        {
                            int agentID;
                            std::string description;
                            std::cout << "Enter Agent ID to assign order: ";
                            std::cin >> agentID;
                            std::cin.ignore(); // Clear newline character
                            std::cout << "Enter Order Description: ";
                            std::getline(std::cin, description);
                            assignOrderToAgent(agentID, description);
                            break;
                        }
                        case 5:
                            displayAgentList();
                            break;
                        case 6:
                        {
                            int agentID;
                            std::cout << "Enter Agent ID to view details: ";
                            std::cin >> agentID;
                            displayAgentDetails(agentID);
                            break;
                        }
                        case 7:
                        {
                            int agentID;
                            std::cout << "Enter Agent ID to view order history: ";
                            std::cin >> agentID;
                            displayAgentOrderHistory(agentID);
                            break;
                        }
                        case 8:
                            displayAgentPerformanceStatistics();
                            break;
                        case 9:
                            std::cout << "Returning to main menu...\n";
                            break;
                        default:
                            std::cout << "Invalid choice. Please try again.\n";
                    }
                    if (agentChoice == 9) break;
                }
                break;
            }

            case 2: // Order Menu
            {
                int orderChoice;
                while (true)
                {
                    displayOrderMenu();
                    std::cin >> orderChoice;

                    switch (orderChoice)
                    {
                        case 1:
                            displayOrderList();
                            break;
                        case 2:
                        {
                            int orderID;
                            std::cout << "Enter Order ID to view details: ";
                            std::cin >> orderID;
                            displayOrderDetails(orderID);
                            break;
                        }
                        case 3:
                        {
                            int orderID;
                            std::cout << "Enter Order ID to mark as completed: ";
                            std::cin >> orderID;
                            markOrderAsCompleted(orderID);
                            break;
                        }
                        case 4:
                            std::cout << "Returning to main menu...\n";
                            break;
                        default:
                            std::cout << "Invalid choice. Please try again.\n";
                    }
                    if (orderChoice == 4) break;
                }
                break;
            }

            case 3: // Location and Path Management
            {
                int source, destination;

                // Display all locations
                std::cout << "\nLocations:\n";
                for (const auto& pair : locationNames)
                {
                    std::cout << pair.first << ". " << pair.second << std::endl;
                }

                // Input validation for source
                while (true)
                {
                    std::cout << "\nEnter the source location index (0-29): ";
                    std::cin >> source;
                    if (source >= 0 && source < locationNames.size()) break;
                    std::cout << "Invalid input. Please enter a number between 0 and 29." << std::endl;
                }

                // Input validation for destination
                while (true)
                {
                    std::cout << "Enter the destination location index (0-29): ";
                    std::cin >> destination;
                    if (destination >= 0 && destination < locationNames.size()) break;
                    std::cout << "Invalid input. Please enter a number between 0 and 29." << std::endl;
                }

                std::vector<int> distances(locationNames.size(), std::numeric_limits<int>::max());  // initialize distances with max value
                std::vector<bool> visited(locationNames.size(), false);  // initialize visited array
                std::vector<int> previous(locationNames.size(), -1);  // track the previous node in the path

                // Apply Dijkstra's algorithm from source node
                dijkstra(source, distances, visited, previous);

                // Display the shortest distance from source to destination
                std::cout << "\nShortest distance from location " << source << " (" << locationNames[source]
                          << ") to location " << destination << " (" << locationNames[destination] << ") is: "
                          << distances[destination] << " km" << std::endl;

                // Reconstruct and display the path
                std::vector<int> path = reconstructPath(source, destination, previous);

                std::cout << "Path: ";
                for (size_t i = 0; i < path.size(); ++i)
                {
                    std::cout << locationNames[path[i]];
                    if (i != path.size() - 1) std::cout << " -> ";
                }
                std::cout << std::endl;

                // Save the path to a file
                savePathToFile(path, unorderedLocationNames, "path.txt");

                // Load the path from a file (for demonstration purposes)
                std::vector<int> loadedPath = loadPathFromFile("path.txt");
                if (!loadedPath.empty())
                {
                    std::cout << "Loaded Path: ";
                    for (size_t i = 0; i < loadedPath.size(); ++i)
                    {
                        std::cout << locationNames[loadedPath[i]];
                        if (i != loadedPath.size() - 1) std::cout << " -> ";
                    }
                    std::cout << std::endl;
                }

                // Visualize the map and the path
                visualizeMap(path);
                break;
            }

            case 4: // Delivery Management
            {
                int source, destination;
                std::string timeOfDay;
                double averageSpeed, costPerKm, costPerHour, fuelEfficiency;

                // Display all locations
                std::cout << "\nAvailable Locations:\n";
                for (const auto& location : locationNames)
                {
                    std::cout << location.first << ": " << location.second << std::endl;
                }

                std::cout << "Enter source location index (0-29): ";
                std::cin >> source;
                std::cout << "Enter destination location index (0-29): ";
                std::cin >> destination;
                std::cout << "Enter average speed (km/h): ";
                std::cin >> averageSpeed;
                std::cout << "Enter time of day (morning, afternoon, evening): ";
                std::cin >> timeOfDay;
                std::cout << "Enter cost per km: ";
                std::cin >> costPerKm;
                std::cout << "Enter cost per hour: ";
                std::cin >> costPerHour;
                std::cout << "Enter fuel efficiency (km/l): ";
                std::cin >> fuelEfficiency;

                if (!validateLocations(source, destination, dist))
                {
                    return 1;
                }

                double adjustedSpeed = adjustSpeedForTimeOfDay(averageSpeed, timeOfDay);
                std::vector<std::vector<std::pair<int, int>>> deliveryTimeMatrix = calculateDeliveryTime(dist, adjustedSpeed);

                displayOptimalRoute(dist, source, destination);
                displayRouteDetails(dist, deliveryTimeMatrix, costPerKm, costPerHour, fuelEfficiency, source, destination);

                break;
            }

            case 5: // Vehicle and Order Assignment
            {
            // Display all locations
            displayLocations(unorderedLocationNames);

            // Input number of vehicles and their details
            int numVehicles;
            std::cout << "Enter the number of vehicles: ";
            std::cin >> numVehicles;

            std::vector<Vehicle> vehicles(numVehicles);
            for (int i = 0; i < numVehicles; ++i)
            {
                vehicles[i].id = i + 1;
                std::cout << "Enter details for Vehicle " << vehicles[i].id
                          << " (currentLocation maxCapacity currentLoad): ";
                std::cin >> vehicles[i].currentLocation
                         >> vehicles[i].maxCapacity
                         >> vehicles[i].currentLoad;
            }

            std::cout << std::endl;

            // Input number of orders and their details
            int numOrders;
            std::cout << "Enter the number of orders: ";
            std::cin >> numOrders;

            std::vector<Order2> orders(numOrders);
            for (int i = 0; i < numOrders; ++i)
            {
                orders[i].id = i + 1;
                std::cout << "Enter details for Order " << orders[i].id
                          << " (deliveryLocation weight): ";
                std::cin >> orders[i].deliveryLocation
                         >> orders[i].weight;
            }

            std::cout << "\nAssigning Vehicles to Orders:\n";
            std::cout << "==============================\n";

            // Assign vehicles to orders
            assignVehiclesToOrders(vehicles, orders, dist, unorderedLocationNames);

            // Reset a specific vehicle for demonstration
            resetVehicle(vehicles[0], 0);
            std::cout << "\nVehicle 1 has been reset to initial location and load.\n";

            // Update location of a specific vehicle
            updateVehicleLocation(vehicles[0], 2, unorderedLocationNames);

            break;
            }

            case 6: // Exit
                std::cout << "Exiting...\n";
                return 0;

            default:
                std::cout << "Invalid choice. Please try again.\n";
        }

    }

    return 0;
}


// Function to log the reset activity to a file
void logVehicleReset(const Vehicle& vehicle)
{

    std::ofstream logFile("vehicle_reset.log", std::ios_base::app);
    if (logFile.is_open())
    {
        std::time_t now = std::time(nullptr);
        logFile << "Vehicle ID: " << vehicle.id
                << " | Reset at: " << std::ctime(&now)
                << " | New Location: " << vehicle.currentLocation
                << " | New Load: " << vehicle.currentLoad
                << " | Availability: " << (vehicle.isAvailable ? "Yes" : "No") << "\n";
    }

}


// Extended resetVehicle function
void resetVehicle(Vehicle& vehicle, int initialLocation)
{
    vehicle.currentLoad = 0;             // Reset the current load to 0
    vehicle.currentLocation = initialLocation; // Reset to the specified location
    vehicle.isAvailable = true;           // Set the vehicle's availability to true

    // Log the reset activity
    logVehicleReset(vehicle);

    // Print confirmation message to the console
    std::cout << "Vehicle " << vehicle.id << " has been reset.\n"
              << "New Location: " << vehicle.currentLocation << "\n"
              << "New Load: " << vehicle.currentLoad << "\n"
              << "Availability: " << (vehicle.isAvailable ? "Yes" : "No") << std::endl;
}


// Function to log the save operation to a file
void logSaveOperation(const std::string& filename, bool success)
{

    std::ofstream logFile("save_path_log.txt", std::ios_base::app);
    if (logFile.is_open())
    {
        std::time_t now = std::time(nullptr);
        logFile << "Path save operation to " << filename << " at " << std::ctime(&now)
                << "Status: " << (success ? "Success" : "Failure") << "\n";
        logFile.close();
    }
    else
    {
        std::cerr << "Unable to open log file for writing." << std::endl;
    }

}


// Expanded savePathToFile function
void savePathToFile(const std::vector<int>& path, const std::unordered_map<int, std::string>& locationNames, const std::string& filename)
{

    std::ofstream file;
    try
    {
        file.open(filename);
        if (!file)
        {
            throw std::ios_base::failure("Failed to open file.");
        }

        for (int id : path)
        {
            if (locationNames.find(id) != locationNames.end())
            {
                file << locationNames.at(id) << "\n";  // Write location name to file
            }
            else
            {
                std::cerr << "Warning: Location ID " << id << " not found in location names map.\n";
            }
        }

        file.close();
        if (file.fail())
        {
            throw std::ios_base::failure("Failed to write to file.");
        }

        std::cout << "Path successfully saved to " << filename << std::endl;
        logSaveOperation(filename, true);

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        logSaveOperation(filename, false);
    }

}


// Function to log the display operation to a file
void logDisplayOperation(bool success)
{

    std::ofstream logFile("display_locations_log.txt", std::ios_base::app);
    if (logFile.is_open())
    {
        std::time_t now = std::time(nullptr);
        logFile << "Display operation at " << std::ctime(&now)
                << "Status: " << (success ? "Success" : "Failure") << "\n";
        logFile.close();
    }
    else
    {
        std::cerr << "Unable to open log file for writing." << std::endl;
    }

}


// Expanded displayLocations function
void displayLocations(const std::unordered_map<int, std::string>& locationNames)
{

    if (locationNames.empty())
    {
        std::cout << "No available locations to display." << std::endl;
        logDisplayOperation(false);
        return;
    }

    // Convert unordered_map to vector of pairs for sorting
    std::vector<std::pair<int, std::string>> locations(locationNames.begin(), locationNames.end());

    // Sort locations by name
    std::sort(locations.begin(), locations.end(),
              [](const std::pair<int, std::string>& a, const std::pair<int, std::string>& b)
              {
                  return a.second < b.second;
              }
              );

    std::cout << "Available Locations (sorted by name):" << std::endl;
    for (const auto& location : locations)
    {
        std::cout << location.first << ": " << location.second << std::endl;
    }
    std::cout << "==============================" << std::endl;

    logDisplayOperation(true);

}


// Define other function implementations here...
void assignVehiclesToOrders(std::vector<Vehicle>& vehicles, std::vector<Order2>& orders, const std::vector<std::vector<double>>& dist, const std::unordered_map<int, std::string>& locationNames)
{

    for (Order2& order : orders)
    {
        int bestVehicleIndex = -1;
        double minDistance = std::numeric_limits<double>::max();

        // Find the nearest vehicle with sufficient capacity
        for (size_t i = 0; i < vehicles.size(); ++i)
        {
            Vehicle& vehicle = vehicles[i];

            if (vehicle.currentLoad + order.weight <= vehicle.maxCapacity)
            {
                double distance = dist[vehicle.currentLocation][order.deliveryLocation];

                if (distance < minDistance)
                {
                    minDistance = distance;
                    bestVehicleIndex = i;
                }
            }
        }

        // Assign the best vehicle to the order
        if (bestVehicleIndex != -1)
        {
            Vehicle& assignedVehicle = vehicles[bestVehicleIndex];

            std::cout << "Order " << order.id << " is assigned to Vehicle " << assignedVehicle.id << std::endl;

            std::cout << "Delivering to: " << locationNames.at(order.deliveryLocation)
                      << " (Location Index: " << order.deliveryLocation << ")" << std::endl;

            // Update the vehicle's current load and location
            assignedVehicle.currentLoad += order.weight;
            assignedVehicle.currentLocation = order.deliveryLocation;
        }
        else
        {
            std::cout << "Order " << order.id << " could not be assigned to any vehicle!" << std::endl;
        }

        std::cout << std::endl;
    }

}


// Function to log the vehicle location update
void logVehicleLocationUpdate(const Vehicle& vehicle, int oldLocation, int newLocation, bool success)
{

    std::ofstream logFile("vehicle_location_update_log.txt", std::ios_base::app);
    if (logFile.is_open())
    {
        std::time_t now = std::time(nullptr);
        logFile << "Vehicle ID: " << vehicle.id
                << " | Old Location: " << oldLocation
                << " | New Location: " << newLocation
                << " | Update Time: " << std::ctime(&now)
                << " | Status: " << (success ? "Success" : "Failure") << "\n";
        logFile.close();
    }

    else
    {
        std::cerr << "Unable to open log file for writing." << std::endl;
    }

}

// Expanded updateVehicleLocation function
void updateVehicleLocation(Vehicle& vehicle, int newLocation, const std::unordered_map<int, std::string>& locationNames)
{

    // Validate new location
    if (locationNames.find(newLocation) == locationNames.end())
    {
        std::cerr << "Invalid new location. Update failed." << std::endl;
        logVehicleLocationUpdate(vehicle, vehicle.currentLocation, newLocation, false);
        return;
    }

    std::cout << "Vehicle " << vehicle.id << " moved from "
              << locationNames.at(vehicle.currentLocation) << " to "
              << locationNames.at(newLocation) << "." << std::endl;

    int oldLocation = vehicle.currentLocation;
    vehicle.currentLocation = newLocation;

    // Log the location update
    logVehicleLocationUpdate(vehicle, oldLocation, newLocation, true);

}


// Helper function to extract the integer part of the time in hours
int extractHours(double timeInHours)
{

    return static_cast<int>(timeInHours);

}


// Helper function to calculate the fractional part of the time in hours
double calculateFractionalPart(double timeInHours, int hours)
{

    return timeInHours - hours;

}


// Helper function to convert the fractional part to minutes
int convertFractionalPartToMinutes(double fractionalPart)
{

    return static_cast<int>(round(fractionalPart * 60));

}


// Main function to convert time in hours to hours and minutes
std::pair<int, int> convertTimeToHoursAndMinutes(double timeInHours)
{

    // Extract the integer part of the timeInHours to get the hours
    int hours = extractHours(timeInHours);

    // Calculate the fractional part of the timeInHours
    double fractionalPart = calculateFractionalPart(timeInHours, hours);

    // Convert the fractional part to minutes
    int minutes = convertFractionalPartToMinutes(fractionalPart);

    // Return the result as a pair of hours and minutes
    return {hours, minutes};

}


// Function to convert time in hours to hours and minutes
std::pair<int, int> convertTimeToHoursAndMinutes(double timeInHours);


// Helper function to calculate time in hours from distance and speed
double calculateTimeInHours(double distance, double speed)
{

    return distance / speed;

}

// Helper function to initialize the delivery time matrix
std::vector<std::vector<std::pair<int, int>>> initializeDeliveryTimeMatrix(int size)
{

    return std::vector<std::vector<std::pair<int, int>>>(size, std::vector<std::pair<int, int>>(size, {0, 0}));

}


// Function to calculate the delivery time matrix
std::vector<std::vector<std::pair<int, int>>> calculateDeliveryTime(const std::vector<std::vector<double>>& distanceMatrix, double averageSpeed)
{

    int n = distanceMatrix.size();

    // Initialize the delivery time matrix
    std::vector<std::vector<std::pair<int, int>>> deliveryTimeMatrix = initializeDeliveryTimeMatrix(n);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (i != j && distanceMatrix[i][j] > 0)
            {
                // Calculate time in hours
                double timeInHours = calculateTimeInHours(distanceMatrix[i][j], averageSpeed);

                // Convert time in hours to hours and minutes
                deliveryTimeMatrix[i][j] = convertTimeToHoursAndMinutes(timeInHours);
            }
        }
    }

    return deliveryTimeMatrix;

}

// Helper function to adjust speed for morning
double adjustSpeedForMorning(double averageSpeed)
{

    return averageSpeed * 0.8; // Morning rush hour

}

// Helper function to adjust speed for afternoon
double adjustSpeedForAfternoon(double averageSpeed)
{

    return averageSpeed * 0.9; // Afternoon traffic

}


// Helper function to adjust speed for evening
double adjustSpeedForEvening(double averageSpeed)
{

    return averageSpeed * 0.7; // Evening peak

}


// Helper function to adjust speed for night
double adjustSpeedForNight(double averageSpeed)
{

    return averageSpeed * 1.1; // Less traffic at night

}


// Main function to dynamically adjust speed based on time of day
double adjustSpeedForTimeOfDay(double averageSpeed, const std::string& timeOfDay)
{

    if (timeOfDay == "morning")
    {
        return adjustSpeedForMorning(averageSpeed);
    }
    if (timeOfDay == "afternoon")
    {
        return adjustSpeedForAfternoon(averageSpeed);
    }
    if (timeOfDay == "evening")
    {
        return adjustSpeedForEvening(averageSpeed);
    }
    if (timeOfDay == "night")
    {
        return adjustSpeedForNight(averageSpeed);
    }
    return averageSpeed; // Default speed

}


// Function to dynamically adjust speed based on time of day
double adjustSpeedForTimeOfDay(double averageSpeed, const std::string& timeOfDay);


// Helper function to print adjusted speed
void printAdjustedSpeed(double averageSpeed, const std::string& timeOfDay)
{

    double adjustedSpeed = adjustSpeedForTimeOfDay(averageSpeed, timeOfDay);
    std::cout << "Adjusted Speed for ";
    std::cout << timeOfDay;
    std::cout << ": ";
    std::cout << adjustedSpeed;
    std::cout << " km/h";
    std::cout << std::endl;

}


// Demonstration function to show usage of adjustSpeedForTimeOfDay
void demonstrateAdjustSpeed()
{

    double averageSpeed = 60.0; // Average speed in km/h

    // Demonstrate for morning
    std::string timeOfDay = "morning";
    printAdjustedSpeed(averageSpeed, timeOfDay);

    // Demonstrate for afternoon
    timeOfDay = "afternoon";
    printAdjustedSpeed(averageSpeed, timeOfDay);

    // Demonstrate for evening
    timeOfDay = "evening";
    printAdjustedSpeed(averageSpeed, timeOfDay);

    // Demonstrate for night
    timeOfDay = "night";
    printAdjustedSpeed(averageSpeed, timeOfDay);

    // Demonstrate for default time (invalid or unspecified)
    timeOfDay = "midnight";
    printAdjustedSpeed(averageSpeed, timeOfDay);

}


// Helper function to check if index is within matrix bounds
bool isIndexWithinBounds(int index, int size)
{

    return index >= 0 && index < size;

}


// Helper function to print invalid index message
void printInvalidIndexMessage(int index, int size)
{

    cout << "Invalid index: " << index << ". Please enter a valid index between 0 and " << size - 1 << "." << endl;

}


// Helper function to print same source and destination message
void printSameSourceDestinationMessage()
{

    cout << "Source and destination must be different." << endl;

}


// Helper function to print no direct route message
void printNoDirectRouteMessage()
{

    cout << "No direct route exists between the selected locations." << endl;

}


// Additional helper function to check if distance matrix is empty
bool isDistanceMatrixEmpty(const vector<vector<double>>& distanceMatrix)
{

    return distanceMatrix.empty();

}


// Additional helper function to check if matrix is square
bool isMatrixSquare(const vector<vector<double>>& distanceMatrix)
{

    for (const auto& row : distanceMatrix)
    {
        if (row.size() != distanceMatrix.size())
        {
            return false;
        }
    }
    return true;

}


// Function to validate locations dynamically
bool validateLocations(int source, int destination, const vector<vector<double>>& distanceMatrix)
{

    int size = distanceMatrix.size();

    // Check if distance matrix is empty
    if (isDistanceMatrixEmpty(distanceMatrix))
    {
        cout << "The distance matrix is empty. Please provide a valid distance matrix." << endl;
        return false;
    }

    // Check if distance matrix is square
    if (!isMatrixSquare(distanceMatrix))
    {
        cout << "The distance matrix is not square. Please provide a valid square matrix." << endl;
        return false;
    }

    // Validate source index
    if (!isIndexWithinBounds(source, size))
    {
        printInvalidIndexMessage(source, size);
        return false;
    }

    // Validate destination index
    if (!isIndexWithinBounds(destination, size))
    {
        printInvalidIndexMessage(destination, size);
        return false;
    }

    // Check if source and destination are the same
    if (source == destination)
    {
        printSameSourceDestinationMessage();
        return false;
    }

    // Check if there is a direct route
    if (distanceMatrix[source][destination] <= 0)
    {
        printNoDirectRouteMessage();
        return false;
    }

    return true;

}


// Function to estimate fuel consumption
double estimateFuelConsumption(double distance, double fuelEfficiency)
{

    // Validate distance
    if (distance <= 0)
    {
        std::cerr << "Error: Distance must be greater than zero." << std::endl;
        throw std::invalid_argument("Invalid distance");
    }

    // Validate fuel efficiency
    if (fuelEfficiency <= 0)
    {
        std::cerr << "Error: Fuel efficiency must be greater than zero." << std::endl;
        throw std::invalid_argument("Invalid fuel efficiency");
    }

    // Calculate fuel consumption
    double fuelConsumption = distance / fuelEfficiency;

    // Log the estimation process
    std::cout << "Estimating fuel consumption..." << std::endl;
    std::cout << "Distance: " << distance << " km" << std::endl;
    std::cout << "Fuel Efficiency: " << fuelEfficiency << " km/l" << std::endl;
    std::cout << "Estimated Fuel Consumption: " << fuelConsumption << " liters" << std::endl;

    return fuelConsumption;

}


// Function to find the shortest path using Dijkstra's algorithm
vector<int> dijkstra(int source, const vector<vector<double>>& distanceMatrix)
{

    int n = distanceMatrix.size();
    vector<double> dist(n, numeric_limits<double>::max());
    vector<int> prev(n, -1);
    vector<bool> visited(n, false);

    dist[source] = 0.0;

    for (int i = 0; i < n; ++i)
    {
        int u = -1;

        for (int v = 0; v < n; ++v)
        {
            if (!visited[v] && (u == -1 || dist[v] < dist[u]))
            {
                u = v;
            }
        }

        if (dist[u] == numeric_limits<double>::max())
        {
            break;
        }

        visited[u] = true;

        for (int v = 0; v < n; ++v)
        {
            if (distanceMatrix[u][v] > 0 && dist[u] + distanceMatrix[u][v] < dist[v])
            {
                dist[v] = dist[u] + distanceMatrix[u][v];
                prev[v] = u;
            }
        }
    }

    return prev;

}


// Function to display optimal route and total distance
void displayOptimalRoute(const vector<vector<double>>& distanceMatrix, int source, int destination)
{

    vector<int> prev = dijkstra(source, distanceMatrix);
    vector<int> path;
    int current = destination;

    while (current != -1)
    {
        path.push_back(current);
        current = prev[current];
    }

    reverse(path.begin(), path.end()); // Fix for the reverse error

    cout << "Optimal Route: ";
    for (int node : path)
    {
        cout << node << " ";
    }
    cout << "| Total Distance: " << distanceMatrix[source][destination] << " km" << endl;

}


// Function to calculate delivery cost
double calculateDeliveryCost(double distance, const pair<int, int>& time, double costPerKm, double costPerHour)
{

    double distanceCost = distance * costPerKm;
    double timeCost = (time.first + time.second / 60.0) * costPerHour;
    return distanceCost + timeCost;

}


// Function to calculate and display route details
void displayRouteDetails
(
    const vector<vector<double>>& distanceMatrix,
    const vector<vector<pair<int, int>>>& timeMatrix,
    double costPerKm,
    double costPerHour,
    double fuelEfficiency,
    int source,
    int destination
)
{

    double distance = distanceMatrix[source][destination];
    pair<int, int> time = timeMatrix[source][destination];
    double routeCost = calculateDeliveryCost(distance, time, costPerKm, costPerHour);
    double fuelUsed = estimateFuelConsumption(distance, fuelEfficiency);

    cout << "Route Details:" << endl;
    cout << "  Distance: " << distance << " km" << endl;
    cout << "  Delivery Time: " << time.first << " hours and " << time.second << " minutes" << endl;
    cout << "  Delivery Cost: " << fixed << setprecision(2) << routeCost << " currency units" << endl;
    cout << "  Estimated Fuel Consumption: " << fixed << setprecision(2) << fuelUsed << " liters" << endl;

}


// Load agents from a file (simulating database retrieval)
void loadAgents()
{

    std::ifstream inFile("agents.txt");
    if (inFile.is_open())
    {
        Agent agent;
        while (inFile >> agent.id >> agent.name >> agent.phone >> agent.isAvailable >> agent.assignedOrders >> agent.completedOrders)
        {
            agents.push_back(agent);
            agentMap[agent.id] = agent; // Populate the map for quick access
        }
        inFile.close();
    }

}


// Save agents to a file (simulating database storage)
void saveAgents()
{

    std::ofstream outFile("agents.txt");
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open agents.txt for writing.\n";
        return;
    }

    for (const auto& agent : agents)
    {
        outFile << agent.id << " ";
        outFile << agent.name << " ";
        outFile << agent.phone << " ";
        outFile << agent.isAvailable << " ";
        outFile << agent.assignedOrders << " ";
        outFile << agent.completedOrders << "\n";
    }

    outFile.close();

    if (outFile.fail())
    {
        std::cerr << "Failed to write to agents.txt.\n";
    }
    else
    {
        std::cout << "Agents saved successfully.\n";
    }

}


// Load orders from a file (simulating database retrieval)
void loadOrders()
{

    std::ifstream inFile("orders.txt");
    if (inFile.is_open())
    {
        Order order;
        while (inFile >> order.id >> order.agentID >> std::ws && std::getline(inFile, order.description) >> order.isCompleted >> order.assignedTime >> order.completionTime)
        {
            orders.push_back(order);
            agentOrders[order.agentID].push_back(order);
        }
        inFile.close();
    }

}


// Save orders to a file (simulating database storage)
void saveOrders()
{

    std::ofstream outFile("orders.txt");
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open orders.txt for writing.\n";
        return;
    }

    for (const auto& order : orders)
    {
        outFile << order.id << " ";
        outFile << order.agentID << " ";
        outFile << order.description << " ";
        outFile << order.isCompleted << " ";
        outFile << order.assignedTime << " ";
        outFile << order.completionTime << "\n";
    }

    outFile.close();

    if (outFile.fail())
    {
        std::cerr << "Failed to write to orders.txt.\n";
    }
    else
    {
        std::cout << "Orders saved successfully.\n";
    }

}


// Helper function to check if the agent's name is unique
bool isUniqueName(const std::string& name)
{

    for (const auto& agent : agents)
    {
        if (agent.name == name)
        {
            return false;
        }
    }
    return true;

}


// Register a new agent
void registerAgent()
{

    Agent newAgent;
    newAgent.id = agents.size() + 1; // Assign unique ID
    std::cout << "Enter Agent Name: ";
    std::cin.ignore();
    std::getline(std::cin, newAgent.name);

    // Check if the agent's name is unique
    if (!isUniqueName(newAgent.name))
    {
        std::cout << "Agent name already exists. Please choose a different name.\n";
        return;
    }

    std::cout << "Enter Phone Number: ";
    std::cin >> newAgent.phone;

    // Removed phone number validation

    newAgent.isAvailable = true; // Default to available
    newAgent.assignedOrders = 0;
    newAgent.completedOrders = 0;

    agents.push_back(newAgent);
    agentMap[newAgent.id] = newAgent;
    saveAgents();

    // Log the registration activity
    std::ofstream logFile("registration_activity.log", std::ios_base::app);
    if (logFile.is_open())
    {
        logFile << "Registered Agent ID: " << newAgent.id
                << " | Name: " << newAgent.name
                << " | Phone: " << newAgent.phone
                << " | Registered At: " << std::ctime(nullptr) << "\n";
    }

    std::cout << "Agent registered successfully!\n";

}


// Login agent (just checking if agent exists and is registered)
void loginAgent()
{

    int agentID;
    std::cout << "Enter Agent ID: ";
    std::cin >> agentID;

    if (agentMap.find(agentID) != agentMap.end())
    {
        std::cout << "Agent " << agentMap[agentID].name << " logged in successfully.\n";
    }
    else
    {
        std::cout << "Agent not found. Please register first.\n";
    }

}


// Update agent availability
void updateAvailability(int agentID, bool available)
{

    // Check if the agent exists in the agentMap
    if (agentMap.find(agentID) != agentMap.end())
    {
        // Update the agent's availability status
        agentMap[agentID].isAvailable = available;

        // Save the updated agents data
        saveAgents();

        // Log the availability update
        std::ofstream logFile("availability_updates.log", std::ios_base::app);
        if (logFile.is_open())
        {
            logFile << "Agent ID: " << agentID
                    << " | Name: " << agentMap[agentID].name
                    << " | New Availability: " << (available ? "Available" : "Not Available")
                    << " | Time: " << std::ctime(nullptr) << "\n";
        }

        // Notify that the agent's availability has been updated
        std::cout << "Agent availability updated successfully.\n";
    }
    else
    {
        // Notify if the agent is not found
        std::cout << "Agent not found.\n";
    }

}


// Assign order to an agent
void assignOrderToAgent(int agentID, const std::string& description)
{

    // Check if the agent exists and is available
    if (agentMap.find(agentID) != agentMap.end() && agentMap[agentID].isAvailable)
    {
        // Create a new order
        Order newOrder;
        newOrder.id = orders.size() + 1; // Assign a unique order ID
        newOrder.agentID = agentID;
        newOrder.description = description;
        newOrder.isCompleted = false;
        newOrder.assignedTime = std::time(nullptr);
        newOrder.completionTime = 0;

        // Add the new order to the orders list and agent's order list
        orders.push_back(newOrder);
        agentOrders[agentID].push_back(newOrder);

        // Update the agent's assigned orders count
        agentMap[agentID].assignedOrders += 1;

        // Save the updated data
        saveAgents();
        saveOrders();

        // Log the assignment of the new order
        std::ofstream logFile("order_assignment.log", std::ios_base::app);
        if (logFile.is_open())
        {
            logFile << "Assigned Order ID: " << newOrder.id << " to Agent ID: " << newOrder.agentID
                    << " at " << std::ctime(&newOrder.assignedTime) << "\n";
        }

        // Notify that the order has been assigned
        std::cout << "Order assigned to agent " << agentMap[agentID].name << ".\n";
    }
    else
    {
        // Notify if the agent is not available or not found
        if (agentMap.find(agentID) == agentMap.end())
        {
            std::cout << "Agent with ID " << agentID << " not found.\n";
        }
        else if (!agentMap[agentID].isAvailable)
        {
            std::cout << "Agent " << agentMap[agentID].name << " is not available.\n";
        }
    }

}


// Mark an order as completed
void markOrderAsCompleted(int orderID)
{

    // Find the order with the given orderID
    auto it = std::find_if(orders.begin(), orders.end(), [orderID](const Order& order)
    {
        return order.id == orderID;
    }
    );

    // Check if the order is found and not already completed
    if (it != orders.end() && !it->isCompleted)
    {
        // Mark the order as completed and set the completion time
        it->isCompleted = true;
        it->completionTime = std::time(nullptr);

        // Update the agent's completed orders count
        auto& agent = agentMap[it->agentID];
        agent.completedOrders += 1;

        // Save the updated data
        saveAgents();
        saveOrders();

        // Notify the user that the order has been marked as completed
        std::cout << "Order " << orderID << " marked as completed.\n";
    }
    else if (it == orders.end())
    {
        // Notify the user if the order was not found
        std::cout << "Order with ID " << orderID << " not found.\n";
    }
    else
    {
        // Notify the user if the order is already completed
        std::cout << "Order with ID " << orderID << " is already completed.\n";
    }

}


// Display list of all agents
void displayAgentList()
{

    std::cout << "\n--- Agent List ---\n";

    // Print table header
    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(20) << "Name"
              << std::setw(15) << "Phone"
              << std::setw(12) << "Available"
              << std::setw(18) << "Assigned Orders"
              << std::setw(18) << "Completed Orders"
              << "\n";

    for (const auto& agent : agents)
    {
        std::cout << std::left
                  << std::setw(5) << agent.id
                  << std::setw(20) << agent.name
                  << std::setw(15) << agent.phone
                  << std::setw(12) << (agent.isAvailable ? "Yes" : "No")
                  << std::setw(18) << agent.assignedOrders
                  << std::setw(18) << agent.completedOrders
                  << "\n";
    }

}


// Function to display details of a specific agent
void displayAgentDetails(int agentID)
{

    if (agentMap.find(agentID) != agentMap.end())
    {
        const auto& agent = agentMap[agentID];
        std::cout << "\n--- Agent Details ---\n";
        std::cout << "ID: " << agent.id << "\n";
        std::cout << "Name: " << agent.name << "\n";
        std::cout << "Phone: " << agent.phone << "\n";
        std::cout << "Available: " << (agent.isAvailable ? "Yes" : "No") << "\n";
        std::cout << "Assigned Orders: " << agent.assignedOrders << "\n";
        std::cout << "Completed Orders: " << agent.completedOrders << "\n";

        // Calculate and display the completion rate
        double completionRate = (agent.assignedOrders > 0) ?
            (static_cast<double>(agent.completedOrders) / agent.assignedOrders) * 100 : 0.0;
        std::cout << "Completion Rate: " << completionRate << "%\n";

        // Display an additional message based on the agent's availability
        if (agent.isAvailable)
        {
            std::cout << "Status: The agent is currently available to take new orders.\n";
        }
        else
        {
            std::cout << "Status: The agent is currently not available.\n";
        }
    }
    else
    {
        std::cout << "Agent not found.\n";
    }

}


// Display list of all orders
void displayOrderList()
{

    std::cout << "\n--- Order List ---\n";

    // Print table header
    std::cout << std::left
              << std::setw(10) << "Order ID" << " | "
              << std::setw(10) << "Agent ID" << " | "
              << std::setw(30) << "Description" << " | "
              << std::setw(12) << "Completed" << " | "
              << std::setw(20) << "Assigned Time" << " | "
              << std::setw(20) << "Completion Time"
              << "\n";

    // Print each order
    for (const auto& order : orders)
    {
        std::cout << std::left
                  << std::setw(10) << order.id << " | "
                  << std::setw(10) << order.agentID << " | "
                  << std::setw(30) << order.description << " | "
                  << std::setw(12) << (order.isCompleted ? "Yes" : "No") << " | "
                  << std::setw(20) << formatTime(order.assignedTime) << " | "
                  << std::setw(20) << (order.isCompleted ? formatTime(order.completionTime) : "N/A")
                  << "\n";
    }

}


// Display details of a specific order
void displayOrderDetails(int orderID)
{

    auto it = std::find_if(orders.begin(), orders.end(), [orderID](const Order& order)
    {
        return order.id == orderID;
    });

    if (it != orders.end())
    {
        const auto& order = *it;
        std::cout << "\n--- Order Details ---\n";
        std::cout << "Order ID: " << order.id << "\n";
        std::cout << "Agent ID: " << order.agentID << "\n";
        std::cout << "Description: " << order.description << "\n";
        std::cout << "Completed: " << (order.isCompleted ? "Yes" : "No") << "\n";
        std::cout << "Assigned Time: " << std::ctime(&order.assignedTime);
        if (order.isCompleted)
        {
            std::cout << "Completion Time: " << std::ctime(&order.completionTime);
        }
    }
    else
    {
        std::cout << "Order not found.\n";
    }

}


// Function to display order history of a specific agent
void displayAgentOrderHistory(int agentID)
{

    if (agentMap.find(agentID) != agentMap.end())
    {
        const auto& agent = agentMap[agentID];
        std::cout << "\n--- Order History for Agent " << agent.name << " ---\n";
        const auto& orders = agentOrders[agentID];

        // Check if the agent has any orders
        if (orders.empty())
        {
            std::cout << "No orders found for this agent.\n";
            return;
        }

        // Print table header
        std::cout << std::left << std::setw(10) << "Order ID"
                  << std::setw(30) << "Description"
                  << std::setw(12) << "Completed"
                  << std::setw(20) << "Assigned Time"
                  << std::setw(20) << "Completion Time"
                  << "\n";

        // Print each order
        for (const auto& order : orders)
        {
            std::cout << std::left << std::setw(10) << order.id
                      << std::setw(30) << order.description
                      << std::setw(12) << (order.isCompleted ? "Yes" : "No")
                      << std::setw(20) << formatTime(order.assignedTime)
                      << std::setw(20) << (order.isCompleted ? formatTime(order.completionTime) : "N/A")
                      << "\n";
        }
    }
    else
    {
        std::cout << "Agent not found.\n";
    }

}


// Function to display performance statistics of all agents
void displayAgentPerformanceStatistics()
{

    if (agents.empty())
    {
        std::cout << "\nNo agents available to display statistics.\n";
        return;
    }

    std::cout << "\n--- Agent Performance Statistics ---\n";
    std::cout << std::left << std::setw(5) << "ID"
              << std::setw(20) << "Name"
              << std::setw(15) << "Phone"
              << std::setw(12) << "Available"
              << std::setw(18) << "Assigned Orders"
              << std::setw(18) << "Completed Orders"
              << "\n";

    for (const auto& agent : agents)
    {
        std::cout << std::left << std::setw(5) << agent.id
                  << std::setw(20) << agent.name
                  << std::setw(15) << agent.phone
                  << std::setw(12) << (agent.isAvailable ? "Yes" : "No")
                  << std::setw(18) << agent.assignedOrders
                  << std::setw(18) << agent.completedOrders
                  << "\n";
    }

}


// Display main menu
void displayMainMenu()
{

    std::cout << "\n--- Delivery Management System ---\n";
    std::cout << "1. Manage Agents\n";
    std::cout << "2. Manage Orders\n";
    std::cout << "3. Calculate Shortest Path\n";
    std::cout << "4. Calculate delivery time, cost, and fuel efficiency\n";
    std::cout << "5. Vehicle and Order Assignment\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: ";

}


// Display agent management menu
void displayAgentMenu()
{

    std::cout << "\n--- Agent Management ---\n";
    std::cout << "1. Register Agent\n";
    std::cout << "2. Login Agent\n";
    std::cout << "3. Update Availability\n";
    std::cout << "4. Assign Order to Agent\n";
    std::cout << "5. Display Agent List\n";
    std::cout << "6. Display Agent Details\n";
    std::cout << "7. Display Agent Order History\n";
    std::cout << "8. Display Agent Performance Statistics\n";
    std::cout << "9. Return to Main Menu\n";
    std::cout << "Enter your choice: ";

}


// Display order management menu
void displayOrderMenu()
{

    std::cout << "\n--- Order Management ---\n";
    std::cout << "1. Display Order List\n";
    std::cout << "2. Display Order Details\n";
    std::cout << "3. Mark Order as Completed\n";
    std::cout << "4. Return to Main Menu\n";
    std::cout << "Enter your choice: ";

}


// Function to find the shortest path from the source node using Dijkstra's algorithm
void dijkstra(int source, std::vector<int>& distances, std::vector<bool>& visited, std::vector<int>& previous)
{

    using P = std::pair<int, int>;  // (distance, node) pair
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;  // min-heap priority queue
    distances[source] = 0;  // distance to the source is 0
    pq.push({0, source});  // push source node with distance 0

    while (!pq.empty())
    {
        int currentDistance = pq.top().first;  // current shortest distance
        int currentNode = pq.top().second;  // current node
        pq.pop();  // remove the node from the priority queue

        if (visited[currentNode]) continue;  // if already visited, skip

        visited[currentNode] = true;  // mark the current node as visited

        // Explore neighbors
        for (int i = 0; i < numLocations; ++i)
        {
            if (dist[currentNode][i] != 0 && distances[currentNode] + dist[currentNode][i] < distances[i])
            {
                distances[i] = distances[currentNode] + dist[currentNode][i];  // update distance
                pq.push({distances[i], i});  // push neighbor with updated distance
                previous[i] = currentNode;  // update the previous node for the path
            }
        }
    }

}


// Function to reconstruct the path from source to destination
std::vector<int> reconstructPath(int source, int destination, const std::vector<int>& previous)
{

    std::vector<int> path;

    // Check if the destination is reachable
    if (previous[destination] == -1 && source != destination)
    {
        std::cerr << "Error: No path found from " << source << " to " << destination << std::endl;
        return path; // return an empty path
    }

    // Reconstruct the path
    for (int at = destination; at != -1; at = previous[at])
    {
        path.push_back(at);
    }

    std::reverse(path.begin(), path.end());

    // Check if the path starts at the source
    if (path.front() != source)
    {
        std::cerr << "Error: Path reconstruction failed. Starting point does not match the source." << std::endl;
        return std::vector<int>(); // return an empty path
    }

    return path;

}


// Function to save the path to a file
void savePathToFile(const std::vector<int>& path, const std::map<int, std::string>& locationNames, const std::string& filename)
{

    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        outFile << "Path: ";
        for (size_t i = 0; i < path.size(); ++i)
        {
            outFile << locationNames.at(path[i]);
            if (i != path.size() - 1) outFile << " -> ";
        }
        outFile << std::endl;
        outFile.close();
        std::cout << "\nPath saved to " << filename << std::endl;
    }
    else
    {
        std::cout << "\nUnable to open file " << filename << " for writing." << std::endl;
    }

}


// Function to load the path from a file
std::vector<int> loadPathFromFile(const std::string& filename)
{

    std::vector<int> path;
    std::ifstream inFile(filename);

    // Check if file opened successfully
    if (!inFile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return path;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        // Process the line and extract location indices
        size_t pos = 0;
        while ((pos = line.find(" -> ")) != std::string::npos)
        {
            std::string loc = line.substr(0, pos);
            // Find the location index by name (reverse lookup)
            auto it = std::find_if(locationNames.begin(), locationNames.end(),
                                   [&loc](const std::pair<int, std::string>& pair) { return pair.second == loc; });
            if (it != locationNames.end())
            {
                path.push_back(it->first);
            }
            else
            {
                std::cerr << "Warning: Location '" << loc << "' not found in locationNames map" << std::endl;
            }
            line.erase(0, pos + 4);
        }

        // Handle the last location after the final " -> "
        auto it = std::find_if(locationNames.begin(), locationNames.end(),
                               [&line](const std::pair<int, std::string>& pair) { return pair.second == line; });
        if (it != locationNames.end())
        {
            path.push_back(it->first);
        }
        else
        {
            std::cerr << "Warning: Location '" << line << "' not found in locationNames map" << std::endl;
        }
    }

    inFile.close();
    return path;

}


// Function to visualize the map and the path
void visualizeMap(const std::vector<int>& path)
{

    if (path.empty())
    {
        std::cout << "\nNo path to visualize.\n";
        return;
    }

    std::cout << "\nPath Visualization:\n";
    int totalDistance = 0;

    for (size_t i = 0; i < path.size(); ++i)
    {
        std::cout << locationNames[path[i]];
        if (i != path.size() - 1)
        {
            int distance = dist[path[i]][path[i + 1]];
            totalDistance += distance;
            std::cout << " -> (" << distance << " km) -> ";
        }
    }

    std::cout << "\nTotal Distance: " << totalDistance << " km" << std::endl;

}










class Feedback
{
public:
    string userName;        // User's name
    string userPlace;       // User's place
    int foodRating;         // Rating for food quality (1-5)
    int deliveryRating;     // Rating for delivery time (1-5)
    int packagingRating;    // Rating for packaging (1-5)
    string comments;        // Optional comments from the user
    string feedbackType;    // Type of feedback (Vegetarian, Non-Vegetarian, Italian, French, Chinese)

    // Constructor to initialize the feedback
    Feedback(const string& name, const string& place, int food, int delivery, int packaging, const string& comm, const string& type)
        : userName(name), userPlace(place), foodRating(food), deliveryRating(delivery), packagingRating(packaging), comments(comm), feedbackType(type) {}

    // Default constructor
    Feedback() : userName(""), userPlace(""), foodRating(0), deliveryRating(0), packagingRating(0), comments(""), feedbackType("") {}

    // Function to input feedback from the user
    void submitFeedback(const string& type)
    {
        feedbackType = type;

        // Ensure userName is not empty
        while (userName.empty())
        {
            cout << "Enter user name: ";
            getline(cin, userName);
        }

        // Prompt the user for their place
        cout << "Enter user place: ";
        getline(cin, userPlace);

        // Prompt the user for their rating of food quality, ensuring it is between 1 and 5
        do
        {
            cout << "Rate the food quality (1-5): ";
            cin >> foodRating;
            if (foodRating < 1 || foodRating > 5)
            {
                cout << "Invalid input. Please enter a number between 1 and 5.\n";
            }
        }
        while (foodRating < 1 || foodRating > 5);

        // Prompt the user for their rating of delivery time, ensuring it is between 1 and 5
        do
        {
            cout << "Rate the delivery time (1-5): ";
            cin >> deliveryRating;
            if (deliveryRating < 1 || deliveryRating > 5)
            {
                cout << "Invalid input. Please enter a number between 1 and 5.\n";
            }
        }
        while (deliveryRating < 1 || deliveryRating > 5);

        // Prompt the user for their rating of packaging, ensuring it is between 1 and 5
        do
        {
            cout << "Rate the packaging (1-5): ";
            cin >> packagingRating;
            if (packagingRating < 1 || packagingRating > 5)
            {
                cout << "Invalid input. Please enter a number between 1 and 5.\n";
            }
        }
        while (packagingRating < 1 || packagingRating > 5);

        // Ignore the newline character left by the last input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Prompt the user for additional comments
        cout << "Additional comments: ";
        getline(cin, comments);

        // Provide a summary of the feedback submitted
        cout << "\nThank you for your feedback!\n";
        cout << "Here is a summary of your feedback:\n";
        cout << "Name: " << userName << "\n";
        cout << "Place: " << userPlace << "\n";
        cout << "Food Quality Rating: " << foodRating << "/5\n";
        cout << "Delivery Time Rating: " << deliveryRating << "/5\n";
        cout << "Packaging Rating: " << packagingRating << "/5\n";
        cout << "Comments: " << comments << "\n";
        cout << "Feedback Type: " << feedbackType << "\n";
        cout << "---------------------------------\n";
    }

    // Function to check for inappropriate content in feedback
    bool isValidFeedback() const
    {
        // List of inappropriate words to check for
        vector<string> inappropriateWords = {"bad", "horrible", "terrible", "awful", "disgusting"};
        // Convert comments to lowercase for case-insensitive comparison
        string commentsLower = comments;
        transform(commentsLower.begin(), commentsLower.end(), commentsLower.begin(), ::tolower);

        // Check for inappropriate words in the comments
        for (const auto& word : inappropriateWords)
        {
            size_t pos = commentsLower.find(word);
            while (pos != string::npos)
            {
                // Check if the found word is a standalone word (not part of another word)
                if ((pos == 0 || !isalnum(commentsLower[pos - 1])) &&
                    (pos + word.length() == commentsLower.length() || !isalnum(commentsLower[pos + word.length()])))
                {
                    return false; // Invalid feedback if inappropriate words are found
                }
                pos = commentsLower.find(word, pos + 1);
            }
        }
        return true; // Valid feedback
    }

    // Function to calculate average rating
    double averageRating() const
    {

        // Initialize variables to store the sum of ratings and the number of valid ratings
        int sumOfRatings = 0;
        int numberOfValidRatings = 0;

        // Check if the food rating is valid (between 1 and 5)
        if (foodRating >= 1 && foodRating <= 5)
        {
            sumOfRatings += foodRating;
            numberOfValidRatings++;
        }
        else
        {
            cout << "Invalid food rating: " << foodRating << ". Please provide a rating between 1 and 5.\n";
        }

        // Check if the delivery rating is valid (between 1 and 5)
        if (deliveryRating >= 1 && deliveryRating <= 5)
        {
            sumOfRatings += deliveryRating;
            numberOfValidRatings++;
        }
        else
        {
            cout << "Invalid delivery rating: " << deliveryRating << ". Please provide a rating between 1 and 5.\n";
        }

        // Check if the packaging rating is valid (between 1 and 5)
        if (packagingRating >= 1 && packagingRating <= 5)
        {
            sumOfRatings += packagingRating;
            numberOfValidRatings++;
        }
        else
        {
            cout << "Invalid packaging rating: " << packagingRating << ". Please provide a rating between 1 and 5.\n";
        }

        // Calculate the average rating, ensuring there are valid ratings to avoid division by zero
        double average = (numberOfValidRatings > 0) ? static_cast<double>(sumOfRatings) / numberOfValidRatings : 0.0;

        // Debugging statements to print intermediate values
        cout << "Sum of ratings: " << sumOfRatings << "\n";
        cout << "Number of valid ratings: " << numberOfValidRatings << "\n";
        cout << "Calculated average rating: " << average << "\n";

        // Return the calculated average rating
        return average;
    }

    // Display the feedback summary
    void displayFeedback() const
    {
        // Print a decorative border
        cout << "\n========================================\n";
        cout << "           Feedback Summary            \n";
        cout << "========================================\n";

        // Display user name with proper formatting
        cout << left << setw(20) << "User Name:" << userName << "\n";

        // Display user place with proper formatting
        cout << left << setw(20) << "User Place:" << userPlace << "\n";

        // Display food rating with a graphical representation
        cout << left << setw(20) << "Food Rating:" << foodRating << "/5 ";
        for (int i = 0; i < foodRating; ++i) cout << "*";
        cout << "\n";

        // Display delivery rating with a graphical representation
        cout << left << setw(20) << "Delivery Rating:" << deliveryRating << "/5 ";
        for (int i = 0; i < deliveryRating; ++i) cout << "*";
        cout << "\n";

        // Display packaging rating with a graphical representation
        cout << left << setw(20) << "Packaging Rating:" << packagingRating << "/5 ";
        for (int i = 0; i < packagingRating; ++i) cout << "*";
        cout << "\n";

        // Display additional comments
        cout << left << setw(20) << "Comments:" << comments << "\n";

        // Display feedback type
        cout << left << setw(20) << "Feedback Type:" << feedbackType << "\n";

        // Print a closing border
        cout << "========================================\n";

        // Thank the user for their feedback
        cout << "\nThank you for providing your feedback!\n";
        cout << "We appreciate your input and will work to improve our services.\n";
    }
    // Display the feedback as a table row with enhanced formatting
    void displayFeedbackTableRow() const
    {
        // Print the table row with proper alignment and formatting
        cout << "| " << left << setw(12) << userName
             << " | " << left << setw(12) << userPlace
             << " | " << right << setw(12) << foodRating << "/5"
             << " | " << right << setw(14) << deliveryRating << "/5"
             << " | " << right << setw(15) << packagingRating << "/5"
             << " | " << left << setw(20) << comments << " |"
             << left <<setw(17)<< feedbackType << " |\n";

        // Print a separator line for the table row
        cout << "+--------------+--------------+----------------+------------------+-------------------+----------------------+------------------+\n";
    }
};


// FeedbackManager class to manage multiple feedbacks
class FeedbackManager
{
private:
    vector<Feedback> feedbackList;

    // Helper function for quick sort
    int partition(vector<Feedback>& arr, int low, int high)
    {
        double pivot = arr[high].averageRating();
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j].averageRating() > pivot)
            {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    void quickSort(vector<Feedback>& arr, int low, int high)
    {
        if (low < high)
        {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

public:
    // Add feedback to the list
    void addFeedback(const Feedback& feedback)
    {
        // Validate the feedback ratings
        if (feedback.foodRating < 1 || feedback.foodRating > 5)
        {
            cout << "Invalid food rating. Please provide a rating between 1 and 5.\n";
            return;
        }
        if (feedback.deliveryRating < 1 || feedback.deliveryRating > 5)
        {
            cout << "Invalid delivery rating. Please provide a rating between 1 and 5.\n";
            return;
        }
        if (feedback.packagingRating < 1 || feedback.packagingRating > 5)
        {
            cout << "Invalid packaging rating. Please provide a rating between 1 and 5.\n";
            return;
        }

        // Optional: Check if the feedback contains inappropriate content
        if (!feedback.isValidFeedback())
        {
            cout << "The provided feedback contains inappropriate content and will not be added.\n";
            return;
        }

        // Add the feedback to the feedback list
        feedbackList.push_back(feedback);

        // Log the addition of feedback
        cout << "Feedback from " << feedback.userName << " has been successfully added.\n";

        // Optionally, display the total number of feedbacks
        cout << "Total number of feedbacks: " << feedbackList.size() << "\n";
    }

    // Display vegetarian feedbacks
    void displayVegetarianFeedbacks() const
    {
        if (feedbackList.empty())
        {
            std::cout << "\nNo feedbacks available to display.\n";
            return;
        }

        std::cout << "\n========================================\n";
        std::cout << "          Vegetarian Feedbacks          \n";
        std::cout << "========================================\n";

        int feedbackCounter = 1;
        for (const auto& feedback : feedbackList)
        {
            if (feedback.feedbackType == "Vegetarian")
            {
                std::cout << "Feedback #" << feedbackCounter << ":\n";
                feedback.displayFeedback();
                std::cout << "----------------------------------------\n";
                feedbackCounter++;
            }
        }

        if (feedbackCounter == 1)
        {
            std::cout << "No vegetarian feedbacks available.\n";
        }

        std::cout << "========================================\n";
        std::cout << "      End of Vegetarian Feedbacks       \n";
        std::cout << "========================================\n";
        std::cout << "\nTotal number of vegetarian feedbacks displayed: " << feedbackCounter - 1 << "\n";
    }

    // Display non-vegetarian feedbacks
    void displayNonVegetarianFeedbacks() const
    {
        if (feedbackList.empty())
        {
            std::cout << "\nNo feedbacks available to display.\n";
            return;
        }

        std::cout << "\n========================================\n";
        std::cout << "        Non-Vegetarian Feedbacks        \n";
        std::cout << "========================================\n";

        int feedbackCounter = 1;
        for (const auto& feedback : feedbackList)
        {
            if (feedback.feedbackType == "Non-Vegetarian")
            {
                std::cout << "Feedback #" << feedbackCounter << ":\n";
                feedback.displayFeedback();
                std::cout << "----------------------------------------\n";
                feedbackCounter++;
            }
        }

        if (feedbackCounter == 1)
        {
            std::cout << "No non-vegetarian feedbacks available.\n";
        }

        std::cout << "========================================\n";
        std::cout << "    End of Non-Vegetarian Feedbacks     \n";
        std::cout << "========================================\n";
        std::cout << "\nTotal number of non-vegetarian feedbacks displayed: " << feedbackCounter - 1 << "\n";
    }

    // Display feedbacks by type
    void displayFeedbacksByType(const string& type) const
    {
        if (feedbackList.empty())
        {
            std::cout << "\nNo feedbacks available to display.\n";
            return;
        }

        std::cout << "\n========================================\n";
        std::cout << "          " << type << " Feedbacks          \n";
        std::cout << "========================================\n";

        int feedbackCounter = 1;
        for (const auto& feedback : feedbackList)
        {
            if (feedback.feedbackType == type)
            {
                std::cout << "Feedback #" << feedbackCounter << ":\n";
                feedback.displayFeedback();
                std::cout << "----------------------------------------\n";
                feedbackCounter++;
            }
        }

        if (feedbackCounter == 1)
        {
            std::cout << "No " << type << " feedbacks available.\n";
        }

        std::cout << "========================================\n";
        std::cout << "      End of " << type << " Feedbacks       \n";
        std::cout << "========================================\n";
        std::cout << "\nTotal number of " << type << " feedbacks displayed: " << feedbackCounter - 1 << "\n";
    }

    // Display all feedbacks
    void displayAllFeedbacks() const
    {
        if (feedbackList.empty())
        {
            cout << "\nNo feedbacks available to display.\n";
            return;
        }

        cout << "\n========================================\n";
        cout << "              All Feedbacks             \n";
        cout << "========================================\n";

        int feedbackCounter = 1;
        for (const auto& feedback : feedbackList)
        {
            cout << "Feedback #" << feedbackCounter << ":\n";
            feedback.displayFeedback();
            cout << "----------------------------------------\n";
            feedbackCounter++;
        }

        cout << "========================================\n";
        cout << "        End of All Feedbacks            \n";
        cout << "========================================\n";
        cout << "\nTotal number of feedbacks displayed: " << feedbackList.size() << "\n";
    }

    // Display all feedbacks as a table
    void displayAllFeedbacksTable() const
    {
        if (feedbackList.empty())
        {
            cout << "\nNo feedbacks available to display.\n";
            return;
        }

        cout << "\n+--------------+--------------+----------------+------------------+-------------------+----------------------+------------------+\n";
        cout << "| User Name    | User Place   | Food Rating    |   Delivery Rating|   Packaging Rating| Comments             |   feedbackType   |\n";
        cout << "+--------------+--------------+----------------+------------------+-------------------+----------------------+------------------+\n";

        for (const auto& feedback : feedbackList)
        {
            feedback.displayFeedbackTableRow();
        }

        cout << "+--------------+--------------+----------------+------------------+-------------------+----------------------+------------------+\n";
        cout << "\nTotal number of feedbacks displayed: " << feedbackList.size() << "\n";
    }


    // Display user's feedback history
    void displayFeedbackHistory() const
    {

        // Check if there are any feedbacks to display
        if (feedbackList.empty())
        {
            cout << "\nNo feedback history available to display.\n";
            return;
        }

        // Print a header for the feedback history section
        cout << "\n========================================\n";
        cout << "             Your Feedback History             \n";
        cout << "========================================\n";

        // Initialize a counter for feedback numbering
        int feedbackCounter = 1;

        // Loop through each feedback in the feedback list
        for (const auto& feedback : feedbackList)
        {
            // Print the feedback number
            cout << "Feedback #" << feedbackCounter << ":\n";

            // Display the feedback details
            feedback.displayFeedback();

            // Print a separator line
            cout << "----------------------------------------\n";

            // Increment the feedback counter
            feedbackCounter++;
        }

        // Print a footer for the feedback history section
        cout << "========================================\n";
        cout << "        End of Your Feedback History           \n";
        cout << "========================================\n";

        // Log the number of feedbacks displayed
        cout << "\nTotal number of feedbacks in your history: " << feedbackList.size() << "\n";

    }

    // Calculate and display ratings
     void displayRatings() const
    {
        if (feedbackList.empty())
        {
            cout << "\nNo feedbacks available to display ratings.\n";
            return;
        }

        // Counters for ratings
        int foodRatings[5] = {0};
        int deliveryRatings[5] = {0};
        int packagingRatings[5] = {0};

        // Count the ratings
        for (const auto& feedback : feedbackList)
        {
            if (feedback.foodRating >= 1 && feedback.foodRating <= 5)
                foodRatings[feedback.foodRating - 1]++;
            if (feedback.deliveryRating >= 1 && feedback.deliveryRating <= 5)
                deliveryRatings[feedback.deliveryRating - 1]++;
            if (feedback.packagingRating >= 1 && feedback.packagingRating <= 5)
                packagingRatings[feedback.packagingRating - 1]++;
        }

        // Display the ratings
        cout << "\n========================================\n";
        cout << "            Ratings Summary             \n";
        cout << "========================================\n";

        cout << "Food Ratings:\n";
        for (int i = 0; i < 5; ++i)
            cout << i + 1 << "/5 rating = " << foodRatings[i] << " users\n";

        cout << "\nDelivery Ratings:\n";
        for (int i = 0; i < 5; ++i)
            cout << i + 1 << "/5 rating = " << deliveryRatings[i] << " users\n";

        cout << "\nPackaging Ratings:\n";
        for (int i = 0; i < 5; ++i)
            cout << i + 1 << "/5 rating = " << packagingRatings[i] << " users\n";

        cout << "========================================\n";
    }

    // Function to find feedback with the highest average rating
    Feedback findHighestRatedFeedback() const
    {
        if (feedbackList.empty())
        {
            cout << "No feedback available to find the highest rated feedback.\n";
            return Feedback();
        }

        auto highestRatedFeedback = max_element(feedbackList.begin(), feedbackList.end(),
                                                [](const Feedback& a, const Feedback& b)
                                                {
                                                    return a.averageRating() < b.averageRating();
                                                });

        cout << "Highest Rated Feedback Details:\n";
        highestRatedFeedback->displayFeedback();

        return *highestRatedFeedback;
    }

    // Function to find feedback with the lowest average rating
    Feedback findLowestRatedFeedback() const
    {
        if (feedbackList.empty())
        {
            cout << "No feedback available to find the lowest rated feedback.\n";
            return Feedback();
        }

        auto lowestRatedFeedback = min_element(feedbackList.begin(), feedbackList.end(),
                                               [](const Feedback& a, const Feedback& b)
                                               {
                                                   return a.averageRating() < b.averageRating();
                                               });

        cout << "Lowest Rated Feedback Details:\n";
        lowestRatedFeedback->displayFeedback();

        return *lowestRatedFeedback;
    }

    // Function to display feedback statistics
    void displayFeedbackStatistics() const
    {

        // Check if there are any feedbacks to evaluate
        if (feedbackList.empty())
        {
            cout << "No feedback available to display statistics.\n";
            return;
        }

        // Find the highest and lowest rated feedbacks
        Feedback highestRated = findHighestRatedFeedback();
        Feedback lowestRated = findLowestRatedFeedback();

        // Initialize variables to store the total ratings and the count of feedbacks
        int totalFoodRating = 0, totalDeliveryRating = 0, totalPackagingRating = 0;
        double totalRating = 0;
        int feedbackCount = feedbackList.size();

        // Loop through each feedback and accumulate the ratings
        for (const auto& feedback : feedbackList)
        {
            totalFoodRating += feedback.foodRating;
            totalDeliveryRating += feedback.deliveryRating;
            totalPackagingRating += feedback.packagingRating;
            totalRating += feedback.averageRating();
        }

        // Calculate the average ratings
        double averageFoodRating = static_cast<double>(totalFoodRating) / feedbackCount;
        double averageDeliveryRating = static_cast<double>(totalDeliveryRating) / feedbackCount;
        double averagePackagingRating = static_cast<double>(totalPackagingRating) / feedbackCount;
        double overallAverageRating = totalRating / feedbackCount;

        // Print the feedback statistics with proper formatting
        cout << "\n========================================\n";
        cout << "            Feedback Statistics            \n";
        cout << "========================================\n";
        cout << "Total Feedbacks: " << feedbackCount << "\n";
        cout << "Average Food Rating: " << fixed << setprecision(2) << averageFoodRating << "/5\n";
        cout << "Average Delivery Rating: " << fixed << setprecision(2) << averageDeliveryRating << "/5\n";
        cout << "Average Packaging Rating: " << fixed << setprecision(2) << averagePackagingRating << "/5\n";
        cout << "Overall Average Rating: " << fixed << setprecision(2) << overallAverageRating << "/5\n";
        cout << "========================================\n";

        // Print details of the highest rated feedback
        cout << "\nHighest Rated Feedback:\n";
        highestRated.displayFeedback();
        cout << "----------------------------------------\n";

        // Print details of the lowest rated feedback
        cout << "\nLowest Rated Feedback:\n";
        lowestRated.displayFeedback();
        cout << "----------------------------------------\n";

        // Log the calculation details
        cout << "\nDetailed Calculation Log:\n";
        cout << "Total Food Rating: " << totalFoodRating << "\n";
        cout << "Total Delivery Rating: " << totalDeliveryRating << "\n";
        cout << "Total Packaging Rating: " << totalPackagingRating << "\n";
        cout << "Total Average Rating Sum: " << totalRating << "\n";
        cout << "Number of Feedbacks: " << feedbackCount << "\n";

    }

    // Function to display user names and places
        void displayUserNamesAndPlaces() const
    {
        // Check if there are any feedbacks to display
        if (feedbackList.empty())
        {
            cout << "No feedbacks available to display user names and places.\n";
            return;
        }

        // Print the table header
        cout << "\nUser Names and Places:\n";
        cout << "+--------------+--------------+\n";
        cout << "| User Name    | User Place   |\n";
        cout << "+--------------+--------------+\n";

        // Loop through each feedback and display user names and places
        for (const auto& feedback : feedbackList)
        {
            // Check if userName is empty and set a default value if it is
            string userNameDisplay = feedback.userName.empty() ? "N/A" : feedback.userName;

            cout << "| " << setw(12) << left << userNameDisplay
                 << " | " << setw(12) << left << feedback.userPlace << " |\n";
            cout << "+--------------+--------------+\n";
        }

        // Log the total number of feedbacks displayed
        cout << "Total number of feedbacks: " << feedbackList.size() << "\n";
    }

    // Function to display a welcome message
    void displayWelcomeMessage() const
    {

        cout << "\n";
        cout << "------------------------------------------------------------------\n";
        cout << "*        Welcome to Online Food Delivery Feedback System         *\n";
        cout << "------------------------------------------------------------------\n";
        cout << "\n";
        cout << "\n";
        cout << "Hello and welcome to our Feedback System!\n";
        cout << "\n";
        cout << "We hope you have a great experience using our Feedback System!\n";
        cout << "*\n";
        cout << "\n";
    }

    // Function to display a thank you message
    void displayThankYouMessage() const
    {

        cout << "\n";
        cout << "---------------------------------------------\n";
        cout << "*        Thank you for your feedback!        *\n";
        cout << "---------------------------------------------\n";
        cout << "\n";
        cout << "\n";
        cout << "We greatly appreciate your time and effort in providing us with your valuable feedback.\n";
        cout << "\n";
        cout << "If you have any further comments or suggestions, please do not hesitate to share them with us.\n";
        cout << "We are always here to listen and make improvements based on your feedback.\n";
        cout << "\n";
        cout << "Thank you once again for being a valued contributor to our community!\n";
        cout << "\n";
        cout << "\n";
        cout << "---------------------------------------------\n";
        cout << "*        Have a wonderful day ahead!         *\n";
        cout << "---------------------------------------------\n";
        cout << "\n";
        cout << "\n";
    }

    // Function to display menu options
    void displayMenu() const
    {
        cout << "\n********** User Feedback Menu **********\n";
        cout << "Welcome to the User Feedback Menu! Please choose an option from the list below:\n";
        cout << "1. Submit Feedback\n";
        cout << "2. Display Vegetarian Feedback\n";
        cout << "3. Display Non-Vegetarian Feedback\n";
        cout << "4. Display Italian Food Feedback\n";
        cout << "5. Display French Food Feedback\n";
        cout << "6. Display Chinese Food Feedback\n";
        cout << "7. Display All Feedbacks\n";
        cout << "8. Display Feedback History\n";
        cout << "9. Display Ratings\n";
        cout << "a. Display Feedback Statistics\n";
        cout << "b. Display User Names and Places\n";
        cout << "c. Back to Main Menu\n";
        cout << "Enter your choice: ";


    }
    // Function to display a visual separator
    void displaySeparator() const
    {
        cout << "\n**\n";
    }
};

// Hotel Feedback class to handle feedback for specific hotels
class HotelFeedback
{
public:
    string hotelName;
    string hotelPlace;
    string comment;
    int rating; // 1 to 5 scale
    string response; // Owner's response

    HotelFeedback(string name, string place, string c, int r)
        : hotelName(name), hotelPlace(place), comment(c), rating(r), response("") {}

    void respond(string res)
    {
        response = res;
    }

        void showFeedback() const
    {
        cout << "*\n";
        cout << "-----------------------------------------------\n";
        cout << "*               Hotel Feedback                *\n";
        cout << "-----------------------------------------------\n";
        cout << "*\n";
        cout << "*\n";

        cout << "+--------------+--------------+--------+----------------------+\n";
        cout << "| Hotel Name   | Hotel Place  | Rating | Comment              |\n";
        cout << "+--------------+--------------+--------+----------------------+\n";
        cout << "| " << setw(12) << hotelName
             << " | " << setw(12) << hotelPlace
             << " | " << setw(6) << rating
             << " | " << setw(20) << comment << " |\n";
        cout << "+--------------+--------------+--------+----------------------+\n";

        if (!response.empty()) {
            cout << "| Owner's Response: " << response << "\n";
            cout << "+-----------------------------------------------+\n";
        }


    }
};


// Class to manage hotel feedbacks
class HotelFeedbackManager
{
private:
    vector<HotelFeedback> feedbacks;
    map<string, vector<int>> hotelRatingsDatabase;

public:
    // Function used to add Hotel Feedback
    void addFeedback(string hotelName, string hotelPlace, string comment, int rating)
    {
        // Check if the rating is within the valid range (1 to 5)
        if (rating < 1 || rating > 5)
        {
            cout << "Invalid rating. Please provide a rating between 1 and 5.\n";
            return;
        }

        // Check if the hotel name and place are not empty
        if (hotelName.empty() || hotelPlace.empty())
        {
            cout << "Hotel name and place cannot be empty.\n";
            return;
        }

        // Add the feedback to the feedbacks vector
        feedbacks.push_back(HotelFeedback(hotelName, hotelPlace, comment, rating));

        // Add the rating to the hotelRatingsDatabase map
        hotelRatingsDatabase[hotelName].push_back(rating);

        // Log the addition of feedback
        cout << "Feedback for hotel " << hotelName << " at " << hotelPlace << " has been successfully added.\n";

        // Optionally, display the total number of feedbacks
        cout << "Total number of feedbacks: " << feedbacks.size() << "\n";

        // Send an alert if the rating is below a certain threshold
        sendAlert();
    }
    // Function used display Feedbacks
        void showFeedbacks() const
    {
        // Check if there are any feedbacks to display
        if (feedbacks.empty())
        {
            cout << "\n********** All Hotel Feedbacks **********\n";
            cout << "No feedbacks available to display.\n";
            cout << "\n";
            return;
        }

        // Print a header for the feedbacks section
        cout << "\n********** All Hotel Feedbacks **********\n";

        // Initialize a counter for feedback numbering
        int feedbackCounter = 1;

        // Loop through each feedback in the feedbacks vector
        for (const auto& feedback : feedbacks)
            {
            // Print the feedback number
            cout << "Feedback #" << feedbackCounter << ":\n";

            // Display the feedback details
            feedback.showFeedback();

            // Print a separator line
            cout << "----------------------------------------\n";

            // Increment the feedback counter
            feedbackCounter++;
        }

        // Print a footer for the feedbacks section
        cout << "\n";
        cout << "Total number of feedbacks: " << feedbacks.size() << "\n";
    }

        void respondToFeedback(int index, string response)
    {
        // Check if the index is within the valid range
        if (index < 0 || index >= feedbacks.size())
        {
            cout << "Invalid feedback index. Please provide a valid index between 0 and " << feedbacks.size() - 1 << ".\n";
            return;
        }

        // Check if the response is not empty
        if (response.empty())
        {
            cout << "Response cannot be empty. Please provide a valid response.\n";
            return;
        }

        // Add the response to the feedback
        feedbacks[index].respond(response);

        // Log the response addition
        cout << "Response to feedback #" << index + 1 << " has been successfully added.\n";

        // Optionally, display the feedback with the added response
        cout << "Updated Feedback Details:\n";
        feedbacks[index].showFeedback();
    }

    //Function use display the feedback history
        void showFeedbackHistory() const
    {
        // Check if there are any feedbacks to display
        if (feedbacks.empty())
        {
            cout << "\n********** Hotel Feedback History **********\n";
            cout << "No feedback history available to display.\n";
            cout << "\n";
            return;
        }

        // Print a header for the feedback history section
        cout << "\n********** Hotel Feedback History **********\n";

        // Initialize a counter for feedback numbering
        int feedbackCounter = 1;

        // Loop through each feedback in the feedbacks vector
        for (const auto& feedback : feedbacks)
        {
            // Print the feedback number
            cout << "Feedback #" << feedbackCounter << ":\n";

            // Display the feedback details
            feedback.showFeedback();

            // Print a separator line
            cout << "----------------------------------------\n";

            // Increment the feedback counter
            feedbackCounter++;
        }

        // Print a footer for the feedback history section
        cout << "\n";
        cout << "Total number of feedbacks: " << feedbacks.size() << "\n";
    }
    // Function used display the ratings
     void displayHotelRating() const
     {
        // Using std::sort to sort feedbacks by rating
        vector<HotelFeedback> sortedFeedbacks = feedbacks;
        sort(sortedFeedbacks.begin(), sortedFeedbacks.end(), [](const HotelFeedback& a, const HotelFeedback& b)
            {
            return a.rating < b.rating;
        });

        // Group hotels by their ratings
        map<int, vector<string>> ratingGroups;
        for (const auto& feedback : sortedFeedbacks)
        {
            ratingGroups[feedback.rating].push_back(feedback.hotelName);
        }

        cout << "\n********** Hotel Ratings **********\n";
        for (int i = 1; i <= 5; ++i)
            {
            cout << i << "/5 rating = " << ratingGroups[i].size() << " Hotels";
            if (!ratingGroups[i].empty())
            {
                cout << " (";
                for (const auto& name : ratingGroups[i])
                {
                    cout << name << " ";
                }
                cout << ")";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    // Function use show feedback Analytics
    void showFeedbackAnalytics() const
    {
        // Check if there are any feedbacks to analyze
        if (feedbacks.empty())
        {
            cout << "\n********** Feedback Analytics **********\n";
            cout << "No feedbacks available for analysis.\n";
            cout << "\n";
            return;
        }

        // Initialize counters and total rating
        int total = 0;
        int positive = 0;
        int negative = 0;
        int neutral = 0;

        // Loop through each feedback to accumulate statistics
        for (const auto& feedback : feedbacks)
            {
            total += feedback.rating;

            if (feedback.rating >= 4)
            {
                positive++;
            } else if (feedback.rating <= 2)
            {
                negative++;
            } else {
                neutral++;
            }
        }

        // Calculate average rating
        double averageRating = static_cast<double>(total) / feedbacks.size();

        // Print the feedback analytics with proper formatting
        cout << "\n********** Feedback Analytics **********\n";
        cout << "Total Feedbacks: " << feedbacks.size() << "\n";
        cout << "Average Rating: " << fixed << setprecision(2) << averageRating << "/5\n";
        cout << "Positive Feedback: " << positive << " (" << fixed << setprecision(2) << (positive * 100.0 / feedbacks.size()) << "%)\n";
        cout << "Neutral Feedback: " << neutral << " (" << fixed << setprecision(2) << (neutral * 100.0 / feedbacks.size()) << "%)\n";
        cout << "Negative Feedback: " << negative << " (" << fixed << setprecision(2) << (negative * 100.0 / feedbacks.size()) << "%)\n";
        cout << "\n";

        // Additional detailed logging
        cout << "\nDetailed Feedback Distribution:\n";
        cout << "Positive (4-5): " << positive << "\n";
        cout << "Neutral (3): " << neutral << "\n";
        cout << "Negative (1-2): " << negative << "\n";
        cout << "Total Ratings Sum: " << total << "\n";
    }

    void sendAlert() const
    {
        // Check if there are any feedbacks to evaluate
        if (feedbacks.empty())
        {
            cout << "\n********** ALERT **********\n";
            cout << "No feedbacks available to evaluate.\n";
            cout << "\n";
            return;
        }

        // Get the last feedback
        const HotelFeedback& lastFeedback = feedbacks.back();

        // Check if the last feedback rating is below the threshold (e.g., less than 3)
        if (lastFeedback.rating < 3)
        {
            cout << "\n********** ALERT **********\n";
            cout << "A negative feedback was received.\n";
            cout << "Hotel Name: " << lastFeedback.hotelName << "\n";
            cout << "Hotel Place: " << lastFeedback.hotelPlace << "\n";
            cout << "Rating: " << lastFeedback.rating << "/5\n";
            cout << "Comment: " << lastFeedback.comment << "\n";
            cout << "\n";
        }
        else
        {
            cout << "\nNo negative feedbacks received.\n";
        }
    }

    void displayMenu() const
    {
        cout << "\n********** Hotel Feedback Menu **********\n";
        cout << "1. Submit Hotel Feedback\n";
        cout << "2. Display All Hotel Feedbacks\n";
        cout << "3. Display Hotel Feedback History\n";
        cout << "4. Display Hotel Ratings\n";
        cout << "5. Display Feedback Analytics\n";
        cout << "6. Back to Main Menu\n";
        cout << "Enter your choice: ";
    }

    void displayWelcomeMessage() const
    {
        cout << "\n";
        cout << "----------------------------------------------------\n";
        cout << "*         Welcome to Hotel Feedback System         *\n";
        cout << "----------------------------------------------------\n";
        cout << "\n";
        cout << "\n";
    }

    void displayThankYouMessage() const
    {
        cout << "*\n";
        cout << "---------------------------------------------\n";
        cout << "*        Thank you for your feedback!       *\n";
        cout << "---------------------------------------------\n";
        cout << "*\n";
        cout << "\n";
    }

    void displaySeparator() const
    {
        cout << "\n**\n";
    }
};

// Function to display the main menu
void displayMainMenu1()
{
    cout << "\n********** Main Menu **********\n";
    cout << "1. User Feedback Menu\n";
    cout << "2. Hotel Feedback Menu\n";
    cout << "3. Display Both User and Hotel Feedbacks\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
}

// Function to handle user feedback menu
void handleUserFeedbackMenu(FeedbackManager& userFeedbackManager)
{
    char subChoice;

    do
    {
        userFeedbackManager.displayMenu();
        cin >> subChoice;

        // Clear the input buffer to remove any leftover newline characters
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (subChoice)
        {
            case '1':
            {
                char feedbackTypeChoice;
                cout << "\nChoose Feedback Type:\n";
                cout << "1. Vegetarian Feedback\n";
                cout << "2. Non-Vegetarian Feedback\n";
                cout << "3. Italian Food Feedback\n";
                cout << "4. French Food Feedback\n";
                cout << "5. Chinese Food Feedback\n";
                cout << "6. Return to User Menu\n";
                cout << "Enter your choice: ";
                cin >> feedbackTypeChoice;

                // Clear the input buffer to remove any leftover newline characters
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (feedbackTypeChoice)
                {
                    case '1':
                    {
                        Feedback vegFeedback;
                        vegFeedback.submitFeedback("Vegetarian");

                        if (vegFeedback.isValidFeedback())
                        {
                            userFeedbackManager.addFeedback(vegFeedback);
                            cout << "Vegetarian Feedback Submitted:\n";
                            vegFeedback.displayFeedback();
                            userFeedbackManager.displayThankYouMessage();
                        }
                        else
                        {
                            cout << "Your feedback contains inappropriate content and cannot be submitted.\n";
                        }
                        break;
                    }
                    case '2':
                    {
                        Feedback nonVegFeedback;
                        nonVegFeedback.submitFeedback("Non-Vegetarian");

                        if (nonVegFeedback.isValidFeedback())
                        {
                            userFeedbackManager.addFeedback(nonVegFeedback);
                            cout << "Non-Vegetarian Feedback Submitted:\n";
                            nonVegFeedback.displayFeedback();
                            userFeedbackManager.displayThankYouMessage();
                        }
                        else
                        {
                            cout << "Your feedback contains inappropriate content and cannot be submitted.\n";
                        }
                        break;
                    }
                    case '3':
                    {
                        Feedback italianFeedback;
                        italianFeedback.submitFeedback("Italian");

                        if (italianFeedback.isValidFeedback())
                        {
                            userFeedbackManager.addFeedback(italianFeedback);
                            cout << "Italian Food Feedback Submitted:\n";
                            italianFeedback.displayFeedback();
                            userFeedbackManager.displayThankYouMessage();
                        }
                        else
                        {
                            cout << "Your feedback contains inappropriate content and cannot be submitted.\n";
                        }
                        break;
                    }
                    case '4':
                    {
                        Feedback frenchFeedback;
                        frenchFeedback.submitFeedback("French");

                        if (frenchFeedback.isValidFeedback())
                        {
                            userFeedbackManager.addFeedback(frenchFeedback);
                            cout << "French Food Feedback Submitted:\n";
                            frenchFeedback.displayFeedback();
                            userFeedbackManager.displayThankYouMessage();
                        }
                        else
                        {
                            cout << "Your feedback contains inappropriate content and cannot be submitted.\n";
                        }
                        break;
                    }
                    case '5':
                    {
                        Feedback chineseFeedback;
                        chineseFeedback.submitFeedback("Chinese");

                        if (chineseFeedback.isValidFeedback())
                        {
                            userFeedbackManager.addFeedback(chineseFeedback);
                            cout << "Chinese Food Feedback Submitted:\n";
                            chineseFeedback.displayFeedback();
                            userFeedbackManager.displayThankYouMessage();
                        }
                        else
                        {
                            cout << "Your feedback contains inappropriate content and cannot be submitted.\n";
                        }
                        break;
                    }
                    case '6':
                        cout << "Returning to User Menu...\n";
                        break;
                    default:
                        cout << "Invalid choice! Please try again.\n";
                        break;
                }
                break;
            }
            case '2':
                userFeedbackManager.displayVegetarianFeedbacks();
                break;
            case '3':
                userFeedbackManager.displayNonVegetarianFeedbacks();
                break;
            case '4':
                userFeedbackManager.displayFeedbacksByType("Italian");
                break;
            case '5':
                userFeedbackManager.displayFeedbacksByType("French");
                break;
            case '6':
                userFeedbackManager.displayFeedbacksByType("Chinese");
                break;
            case '7':
                userFeedbackManager.displayAllFeedbacksTable();
                break;
            case '8':
                userFeedbackManager.displayFeedbackHistory();
                break;
            case '9':
                userFeedbackManager.displayRatings();
                break;
            case 'a':
                userFeedbackManager.displayFeedbackStatistics();
                break;
            case 'b':
                userFeedbackManager.displayUserNamesAndPlaces();
                break;
            case 'c':
                cout << "Returning to Main Menu...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
        userFeedbackManager.displaySeparator();
    }
    while (subChoice != 'c');
}
// Function to handle hotel feedback menu
void handleHotelFeedbackMenu(HotelFeedbackManager& hotelFeedbackManager) {
    char subChoice;
    do {
        hotelFeedbackManager.displayMenu();
        cin >> subChoice;

        switch (subChoice)
        {
            case '1':
                {
                string name, place, comment;
                int rating;
                cout << "Enter hotel name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter hotel place: ";
                getline(cin, place);
                cout << "Rate the hotel (1-5): ";
                cin >> rating;
                cin.ignore();
                cout << "Additional comments: ";
                getline(cin, comment);
                hotelFeedbackManager.addFeedback(name, place, comment, rating);
                hotelFeedbackManager.displayThankYouMessage();
                break;
            }
            case '2':
                hotelFeedbackManager.showFeedbacks();
                break;
            case '3':
                hotelFeedbackManager.showFeedbackHistory();
                break;
            case '4':
                hotelFeedbackManager.displayHotelRating();
                break;
            case '5':
                hotelFeedbackManager.showFeedbackAnalytics();
                break;
            case '6':
                cout << "Returning to Main Menu...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
        hotelFeedbackManager.displaySeparator();
    }
    while (subChoice != '6');
}

// Function to display both user and hotel feedbacks
void displayBothFeedbacks(const FeedbackManager& userFeedbackManager, const HotelFeedbackManager& hotelFeedbackManager)
{

    cout << "\n********** Both User and Hotel Feedbacks **********\n";
    cout << "\nUser Feedbacks:\n";
    userFeedbackManager.displayAllFeedbacksTable();
    cout << "\nHotel Feedbacks:\n";
    hotelFeedbackManager.showFeedbacks();
}

// Main function
int module4()
{
    FeedbackManager userFeedbackManager;
    HotelFeedbackManager hotelFeedbackManager;
    char mainChoice;

    do
    {
        displayMainMenu1();
        cin >> mainChoice;

        // Clear the input buffer to remove any leftover newline characters
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (mainChoice)
        {
            case '1':
                handleUserFeedbackMenu(userFeedbackManager);
                break;
            case '2':
                handleHotelFeedbackMenu(hotelFeedbackManager);
                break;
            case '3':
                cout << "Displaying Both User and Hotel Feedbacks...\n";
                userFeedbackManager.displayAllFeedbacksTable();
                hotelFeedbackManager.showFeedbacks();
                break;
            case '4':
                cout << "Exiting the program...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
                break;
        }
    }
    while (mainChoice != '4');

    return 0;
}




















#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <ctime>
#include <vector>
#include <sstream>
#include <iomanip>
#include <regex>


// Structures to store hotel and user details
struct Hotel
{
    std::string hotel_id;
    std::string name;
    std::string location;
    double price;
    int rating; // Rating out of 5
};


struct User
{
    std::string user_id;
    std::string name;
    std::string preferred_location;
    double min_price;
    double max_price;
    std::vector<std::string> booking_history;  // To track booked hotels
    std::string password; // User password for authentication
};


// Simulated database for hotels and users
std::unordered_map<std::string, Hotel> hotelDatabase;
std::unordered_map<std::string, User> userDatabase;


// File paths for logging
const std::string LOG_FILE = "log.txt";
const std::string USER_FILE = "users.txt";
const std::string HOTEL_FILE = "hotels.txt";


// Define logging levels
enum LogLevel
 {
    INFO,
    WARNING,
    ERROR
};


void logMessage(const std::string& message, LogLevel level = INFO)
{
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (logFile)
    {
        // Get current time
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);

        // Format time
        char buffer[80];
        std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localTime);

        // Determine log level string
        std::string levelStr;
        switch (level)
        {
            case INFO: levelStr = "INFO"; break;
            case WARNING: levelStr = "WARNING"; break;
            case ERROR: levelStr = "ERROR"; break;
        }

        // Write log entry
        logFile << "[" << buffer << "] [" << levelStr << "] " << message << "\n";
    }

    // Ensure the log file is closed
    if (logFile.is_open())
    {
        logFile.close();
    }
}


void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void printDivider()
{
    std::cout << "============================================\n";
}

// Function to get the current date and time as a string
std::string getCurrentDateTime()
{
    std::time_t now = std::time(nullptr);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}


// Expanded printHeader function
void printHeader(const std::string& title, const std::string& customMessage = "", bool showDateTime = true)
{

    clearScreen();
    printDivider();

    // Calculate the width needed for the title
    int width = title.length() > 20 ? title.length() + 4 : 24;
    std::cout << std::setw(width) << std::right << title << "\n";
    printDivider();

    // Print custom message if provided
    if (!customMessage.empty())
    {
        std::cout << customMessage << "\n";
        printDivider();
    }

    // Print current date and time if required
    if (showDateTime)
    {
        std::cout << "Current Date and Time: " << getCurrentDateTime() << "\n";
        printDivider();
    }

}


// Enhanced function to display a welcome message with ASCII art
// Expanded displayWelcomeMessage function
void displayWelcomeMessage()
{
    clearScreen();
    std::cout << R"(
    __    __     _                            _     _                 _
   / / /\ \ \| | ___ ___  _ __ ___   ___  | |_ () __   __ _    / \   _ __ ___
   \ \/  \/ / _ \ |/ _/ _ \| ' ` _ \ / _ \ | '_ \| | '_ \ / ` |  / _ \ | ' ` _ \
    \  /\  /  _/ | (| () | | | | | |  __/ | | | | | | | | (| | / ___ \| | | | | |
     \/  \/ \||\\/|| || ||\| || |||| ||\, |//   \\| || ||
                                                          |_/
    )" << "\n";
    printDivider();
    std::cout << std::setw(28) << std::right << "Welcome to the Hotel Management System\n";
    printDivider();

    // Display current date and time
    std::cout << "Current Date and Time: " << getCurrentDateTime() << "\n";
    printDivider();

    // Personalized welcome message
    std::string userName = "Guest"; // Replace with actual user name if available
    std::cout << "Hello, " << userName << "!\n";
    std::cout << "We are excited to have you here.\n";
    printDivider();

    // Brief description of the system
    std::cout << "This system allows you to:\n";
    std::cout << "- Register as a user or hotel\n";
    std::cout << "- Search for hotels based on your preferences\n";
    std::cout << "- Book and manage your hotel reservations\n";
    std::cout << "- View and edit your profile and booking history\n";
    printDivider();

    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}


void saveUserData()
{
    std::ofstream userFile(USER_FILE);

    // Check if the file is open successfully
    if (!userFile)
    {
        std::cerr << "Error: Could not open user file for writing.\n";
        logMessage("Error: Could not open user file for writing.", ERROR);
        return;
    }

    // Iterate through the user database and write data to the file
    for (const auto& entry : userDatabase)
    {
        const User& user = entry.second;

        // Write basic user information without min_price and max_price
        userFile << user.user_id << "," << user.name << "," << user.preferred_location << "," << user.password << "\n";

        // Write user booking history
        for (const auto& booking : user.booking_history)
        {
            userFile << booking << ",";
        }
        userFile << "\n"; // Newline after each user's booking history
    }

    // Ensure the file is properly closed after writing
    if (userFile.is_open())
    {
        userFile.close();
    }

    // Log the successful save operation
    logMessage("User data saved successfully.", INFO);
}


void saveHotelData()
{
    std::ofstream hotelFile(HOTEL_FILE);

    // Check if the file is open successfully
    if (!hotelFile)
    {
        std::cerr << "Error: Could not open hotel file for writing.\n";
        logMessage("Error: Could not open hotel file for writing.", ERROR);
        return;
    }

    // Iterate through the hotel database and write data to the file
    for (const auto& entry : hotelDatabase)
    {
        const Hotel& hotel = entry.second;

        // Write hotel information without price and rating
        hotelFile << hotel.hotel_id << "," << hotel.name << "," << hotel.location << "\n";
    }

    // Ensure the file is properly closed after writing
    if (hotelFile.is_open())
    {
        hotelFile.close();
    }

    // Log the successful save operation
    logMessage("Hotel data saved successfully.", INFO);
}


// Load Data from Files with enhanced features
void loadUserData()
{

    std::ifstream userFile(USER_FILE);

    // Check if the file is open successfully
    if (!userFile)
    {
        std::cerr << "Error: Could not open user file for reading.\n";
        logMessage("Error: Could not open user file for reading.", ERROR);
        return;
    }

    std::string line;
    while (std::getline(userFile, line))
    {
        std::stringstream ss(line);
        User user;

        // Read and parse the user data
        if (!std::getline(ss, user.user_id, ',')) continue;
        if (!std::getline(ss, user.name, ',')) continue;
        if (!std::getline(ss, user.preferred_location, ',')) continue;
        if (!(ss >> user.min_price)) continue;
        ss.ignore();
        if (!(ss >> user.max_price)) continue;
        ss.ignore();
        if (!std::getline(ss, user.password, ',')) continue;

        // Read and parse the booking history
        if (std::getline(userFile, line))
        {
            std::stringstream ssBooking(line);
            std::string booking;
            while (std::getline(ssBooking, booking, ','))
            {
                if (!booking.empty())
                {
                    user.booking_history.push_back(booking);
                }
            }
        }

        // Add the user to the database
        userDatabase[user.user_id] = user;
    }

    // Ensure the file is properly closed after reading
    if (userFile.is_open())
    {
        userFile.close();
    }

    // Log the successful load operation
    logMessage("User data loaded successfully.", INFO);

}


// Load Data from Files with enhanced features
void loadHotelData()
{

    std::ifstream hotelFile(HOTEL_FILE);

    // Check if the file is open successfully
    if (!hotelFile)
    {
        std::cerr << "Error: Could not open hotel file for reading.\n";
        logMessage("Error: Could not open hotel file for reading.", ERROR);
        return;
    }

    std::string line;
    while (std::getline(hotelFile, line))
    {
        std::stringstream ss(line);
        Hotel hotel;

        // Read and parse the hotel data
        if (!std::getline(ss, hotel.hotel_id, ',')) continue;
        if (!std::getline(ss, hotel.name, ',')) continue;
        if (!std::getline(ss, hotel.location, ',')) continue;
        if (!(ss >> hotel.price)) continue;
        ss.ignore();
        if (!(ss >> hotel.rating)) continue;

        // Add the hotel to the database
        hotelDatabase[hotel.hotel_id] = hotel;
    }

    // Ensure the file is properly closed after reading
    if (hotelFile.is_open())
    {
        hotelFile.close();
    }

    // Log the successful load operation
    logMessage("Hotel data loaded successfully.", INFO);

}

bool validateUserInput(const User& user)
{
    // Check if any required field is empty
    if (user.user_id.empty() || user.name.empty() || user.password.empty())
    {
        std::cout << "Error: All fields are required.\n";
        logMessage("Error: User registration failed due to missing fields.", ERROR);
        return false;
    }

    // Check if the user ID is a valid email
    std::regex email_regex(R"((\w+)(\.{1}\w+)*@(\w+)(\.\w{2,})+)");
    if (!std::regex_match(user.user_id, email_regex))
    {
        std::cout << "Error: Invalid email format for user ID.\n";
        logMessage("Error: User registration failed due to invalid email format for ID: " + user.user_id, ERROR);
        return false;
    }

    // Check if the user with the same ID already exists
    if (userDatabase.find(user.user_id) != userDatabase.end())
    {
        std::cout << "Error: User with the same ID already exists.\n";
        logMessage("Error: Duplicate user registration attempt for ID: " + user.user_id, ERROR);
        return false;
    }

    // Check for minimum password length for better security
    const int MIN_PASSWORD_LENGTH = 8;
    if (user.password.length() < MIN_PASSWORD_LENGTH)
    {
        std::cout << "Error: Password must be at least " << MIN_PASSWORD_LENGTH << " characters long.\n";
        logMessage("Error: User registration failed due to short password for ID: " + user.user_id, ERROR);
        return false;
    }

    // If all validations pass
    logMessage("User input validation successful for ID: " + user.user_id, INFO);
    return true;
}


bool validateHotelInput(const Hotel& hotel)
{
    // Check if any required field is empty
    if (hotel.name.empty() || hotel.location.empty())
    {
        std::cout << "Error: All fields are required.\n";
        logMessage("Error: Hotel registration failed due to missing fields.", ERROR);
        return false;
    }

    // Check for valid hotel name and location lengths
    const int MAX_NAME_LENGTH = 50;
    const int MAX_LOCATION_LENGTH = 50;
    if (hotel.name.length() > MAX_NAME_LENGTH)
    {
        std::cout << "Error: Hotel name is too long (max " << MAX_NAME_LENGTH << " characters).\n";
        logMessage("Error: Hotel registration failed due to too long name: " + hotel.name, ERROR);
        return false;
    }
    if (hotel.location.length() > MAX_LOCATION_LENGTH)
    {
        std::cout << "Error: Hotel location is too long (max " << MAX_LOCATION_LENGTH << " characters).\n";
        logMessage("Error: Hotel registration failed due to too long location: " + hotel.location, ERROR);
        return false;
    }

    // Generate and check the hotel ID based on name and location
    std::string hotel_id = hotel.name + "_" + hotel.location;
    if (hotelDatabase.find(hotel_id) != hotelDatabase.end())
    {
        std::cout << "Error: Hotel with the same name and location already exists.\n";
        logMessage("Error: Duplicate hotel registration attempt for ID: " + hotel_id, ERROR);
        return false;
    }

    // If all validations pass
    logMessage("Hotel input validation successful for ID: " + hotel_id, INFO);
    return true;
}



void readPassword(const std::string& prompt, std::string& password)
{
    std::cout << prompt;
    char ch;
    password.clear();
    while ((ch = _getch()) != '\r')
        { // '\r' is the Enter key
        if (ch == '\b')
        { // Handle backspace
            if (!password.empty())
            {
                password.pop_back();
                std::cout << "\b \b"; // Erase the last '*' from the console
            }
        }
            else
        {
            password.push_back(ch);
            std::cout << '*'; // Display '*' for each character typed
        }
    }
    std::cout << std::endl;
}


void userRegistrationForm()
{
    printHeader("User Registration");
    User newUser;

    auto readUserInput = [](const std::string& prompt, std::string& input)
    {
        do {
            std::cout << prompt;
            std::getline(std::cin, input);
            if (input.empty()) {
                std::cout << "Error: This field is required. Please try again.\n";
            }
        } while (input.empty());
    };

    // Collect email and name
    readUserInput("Enter Email: ", newUser.user_id);
    readUserInput("Enter Name: ", newUser.name);

    // Read and confirm password
    std::string confirmPassword;
    do {
        readPassword("Enter Password: ", newUser.password);
        if (newUser.password.empty())
        {
            std::cout << "Error: Password cannot be empty. Please try again.\n";
            continue;
        }
        readPassword("Confirm Password: ", confirmPassword);
        if (newUser.password != confirmPassword) {
            std::cout << "Error: Passwords do not match. Please try again.\n";
        }
    } while (newUser.password.empty() || newUser.password != confirmPassword);

    // Validate user input and register user
    if (validateUserInput(newUser))
    {
        userDatabase[newUser.user_id] = newUser;
        std::cout << "User registered successfully!\n";
        logMessage("User registered successfully with ID: " + newUser.user_id, INFO);
    }
    else
    {
        std::cout << "User registration failed. Please try again.\n";
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}


void hotelRegistrationForm()
{
    printHeader("Hotel Registration");
    Hotel newHotel;

    // Function to read hotel input safely
    auto readHotelInput = [](const std::string& prompt, std::string& input)
    {
        do {
            std::cout << prompt;
            std::getline(std::cin, input);
            if (input.empty()) {
                std::cout << "Error: This field is required. Please try again.\n";
            }
        } while (input.empty());
    };

    // Collect hotel name and location
    readHotelInput("Enter Hotel Name: ", newHotel.name);
    readHotelInput("Enter Hotel Location: ", newHotel.location);

    // Validate hotel input and register hotel
    if (validateHotelInput(newHotel))
    {
        // Generate a unique ID for the hotel
        newHotel.hotel_id = newHotel.name + "_" + newHotel.location;
        hotelDatabase[newHotel.hotel_id] = newHotel;
        std::cout << "Hotel registered successfully!\n";
        logMessage("Hotel registered successfully with ID: " + newHotel.hotel_id, INFO);
    }
    else
    {
        std::cout << "Hotel registration failed. Please try again.\n";
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}



void searchHotels()
{
    printHeader("Search Hotels");

    std::string hotel_name;
    std::cout << "Enter Hotel Name to search: ";
    std::getline(std::cin, hotel_name);

    // Vector to store matching hotels
    std::vector<Hotel> matchingHotels;

    // Search for hotels that match the entered name
    for (const auto& entry : hotelDatabase)
    {
        const Hotel& hotel = entry.second;
        if (hotel.name.find(hotel_name) != std::string::npos) // Partial match
        {
            matchingHotels.push_back(hotel);
        }
    }

    if (!matchingHotels.empty())
    {
        // Display matching hotels
        for (const auto& hotel : matchingHotels)
        {
            std::cout << "Hotel Name: " << hotel.name << "\n";
            std::cout << "Location: " << hotel.location << "\n";
            std::cout << "--------------------------\n";
        }

        logMessage("Search completed. Found " + std::to_string(matchingHotels.size()) + " matching hotels.", INFO);
    }
    else
    {
        std::cout << "No hotels found matching the name: " << hotel_name << "\n";
        logMessage("Search completed. No matching hotels found for name: " + hotel_name, INFO);
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}



// Function to book a hotel
void bookHotel(const std::string& user_id)
{
    printHeader("Book Hotel");
    std::string hotel_id;

    // Prompt the user to enter the hotel ID
    std::cout << "Enter hotel ID to book: ";
    std::cin >> hotel_id;
    std::cin.ignore(); // Consume newline

    auto hotelIt = hotelDatabase.find(hotel_id);
    if (hotelIt != hotelDatabase.end())
    {
        auto userIt = userDatabase.find(user_id);
        if (userIt != userDatabase.end())
        {
            // Check if the hotel is already booked by the user
            if (std::find(userIt->second.booking_history.begin(), userIt->second.booking_history.end(), hotel_id) != userIt->second.booking_history.end())
            {
                std::cout << "Error: You have already booked this hotel.\n";
                logMessage("User " + user_id + " attempted to book already booked hotel: " + hotel_id, WARNING);
            }
        else
            {
                // Book the hotel
                userIt->second.booking_history.push_back(hotel_id);
                std::cout << "Booking successful for hotel: " << hotelIt->second.name << "\n";
                logMessage("User " + user_id + " booked hotel: " + hotel_id, INFO);
            }
        }
    else
        {
            std::cout << "Error: User not found.\n";
            logMessage("Booking attempt failed. User not found: " + user_id, ERROR);
        }
    }
    else
    {
        std::cout << "Error: Hotel not found.\n";
        logMessage("Booking attempt failed. Hotel not found: " + hotel_id, ERROR);
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}


// Function to delete a hotel
void deleteHotel()
{
    printHeader("Delete Hotel");
    std::string hotel_name;

    // Prompt the user to enter the hotel name
    std::cout << "Enter Hotel Name to delete: ";
    std::getline(std::cin, hotel_name);

    // Find the hotel by name
    auto it = std::find_if(hotelDatabase.begin(), hotelDatabase.end(), [&](const auto& entry) {
        return entry.second.name == hotel_name;
    });

    if (it != hotelDatabase.end())
    {
        // Hotel found, proceed to delete
        std::string hotel_id = it->first; // Retrieve the hotel ID
        hotelDatabase.erase(it);
        std::cout << "Hotel deleted successfully.\n";
        logMessage("Hotel with Name '" + hotel_name + "' was deleted.", INFO);
    }
    else
    {
        // Hotel not found
        std::cout << "Error: Hotel not found.\n";
        logMessage("Attempt to delete non-existent hotel with Name: '" + hotel_name + "'.", WARNING);
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}


// Function to view all registered users
void viewAllUsers()
{
    printHeader("All Users");

    if (userDatabase.empty())
    {
        std::cout << "No registered users found.\n";
        logMessage("Attempted to view all users, but no users were found.", INFO);
        std::cout << "Press Enter to return to the main menu...";
        std::cin.ignore();
        return;
    }

    // Collect all users into a vector for sorting
    std::vector<User> users;
    for (const auto& entry : userDatabase)
    {
        users.push_back(entry.second);
    }

    // Sort users alphabetically by name
    std::sort(users.begin(), users.end(), [](const User& a, const User& b)
    {
        return a.name < b.name;
    });

    // Display user information
    for (const auto& user : users)
    {
        std::cout << "User ID: " << user.user_id << "\n";
        std::cout << "Name: " << user.name << "\n";
        std::cout << "\n";
    }

    // Log the activity
    logMessage("Viewed all registered users.", INFO);

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}


// Function to view all registered hotels
void viewAllHotels()
{
    printHeader("All Hotels");

    if (hotelDatabase.empty())
    {
        std::cout << "No registered hotels found.\n";
        logMessage("Attempted to view all hotels, but no hotels were found.", INFO);
        std::cout << "Press Enter to return to the main menu...";
        std::cin.ignore();
        return;
    }

    // Collect all hotels into a vector for sorting
    std::vector<Hotel> hotels;
    for (const auto& entry : hotelDatabase)
    {
        hotels.push_back(entry.second);
    }

    // Sort hotels by name
    std::sort(hotels.begin(), hotels.end(), [](const Hotel& a, const Hotel& b)
    {
        return a.name < b.name;
    });

    // Display hotel information without showing Hotel ID
    for (const auto& hotel : hotels)
    {
        std::cout << "Name: " << hotel.name << "\n";
        std::cout << "Location: " << hotel.location << "\n";
        std::cout << "--------------------------\n";
    }

    // Log the activity
    logMessage("Viewed all registered hotels.", INFO);

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}




// User login function
bool loginUser(std::string& loggedInUserId)
{
    printHeader("User Login");
    std::string user_id, password;

    // Prompt the user to enter the user ID and password
    std::cout << "Enter User Email: ";
    std::getline(std::cin, user_id);

    // Read the password as asterisks
    readPassword("Enter Password: ", password);

    // Validate user ID and password
    if (user_id.empty() || password.empty())
    {
        std::cout << "Error: User ID and Password cannot be empty.\n";
        logMessage("Failed login attempt due to empty User ID or Password.");
        return false;
    }

    // Find the user in the database
    auto userIt = userDatabase.find(user_id);
    if (userIt != userDatabase.end())
    {
        // Check if the password matches
        if (userIt->second.password == password)
        {
            loggedInUserId = user_id;
            std::cout << "Login successful!\n";
            logMessage("User " + user_id + " logged in successfully.", INFO);
            return true;
        }
        else
        {
            std::cout << "Error: Invalid Password.\n";
            logMessage("Failed login attempt due to invalid password for User ID: " + user_id, WARNING);
        }
    }
    else
    {
        std::cout << "Error: User ID not found.\n";
        logMessage("Failed login attempt for non-existent User ID: " + user_id, WARNING);
    }

    return false;
}



// Function to exit the system with confirmation
bool confirmExit()
{
    char confirm;
    std::cout << "Are you sure you want to exit? (y/n): ";
    std::cin >> confirm;
    return (confirm == 'y' || confirm == 'Y');
}


// Function to edit user information
void editUserInformation(const std::string& user_id)
{
    auto userIt = userDatabase.find(user_id);
    if (userIt != userDatabase.end())
    {
        User& user = userIt->second;
        std::cout << "Editing information for user: " << user.name << "\n";

        // Edit name
        std::cout << "Enter new name (or press Enter to keep current): ";
        std::string name;
        std::getline(std::cin, name);
        if (!name.empty()) user.name = name;

        logMessage("User information updated for ID: " + user_id, INFO);
        std::cout << "Information updated successfully!\n";
    }
    else
    {
        std::cout << "User not found.\n";
        logMessage("Attempt to edit non-existent user with ID: " + user_id, WARNING);
    }
}



// Function to edit hotel information
void editHotelInformation()
{
    printHeader("Edit Hotel Information");

    std::string hotel_name;
    std::cout << "Enter Hotel name to edit: ";
    std::getline(std::cin, hotel_name);

    // Find the hotel based on name
    auto hotelIt = std::find_if(hotelDatabase.begin(), hotelDatabase.end(),
                                [&hotel_name](const auto& entry) {
                                    return entry.second.name == hotel_name;
                                });

    if (hotelIt != hotelDatabase.end())
    {
        Hotel& hotel = hotelIt->second;
        std::cout << "Editing information for hotel: " << hotel.name << "\n";

        // Edit name
        std::cout << "Enter new name (or press Enter to keep current): ";
        std::string name;
        std::getline(std::cin, name);
        if (!name.empty()) hotel.name = name;

        // Edit location
        std::cout << "Enter new location (or press Enter to keep current): ";
        std::string location;
        std::getline(std::cin, location);
        if (!location.empty()) hotel.location = location;

        logMessage("Hotel information updated for name: " + hotel_name, INFO);
        std::cout << "Information updated successfully!\n";
    }
    else
    {
        std::cout << "Hotel not found.\n";
        logMessage("Attempt to edit non-existent hotel with name: " + hotel_name, WARNING);
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}


// Function to view booking history
void viewBookingHistory(const std::string& user_id)
{

    printHeader("Booking History");
    auto userIt = userDatabase.find(user_id);
    if (userIt != userDatabase.end())
        {
        const User& user = userIt->second;
        if (user.booking_history.empty())
        {
            std::cout << "No booking history found for " << user.name << ".\n";
            logMessage("Viewed empty booking history for user ID: " + user_id, INFO);
        }
        else
        {
            std::cout << "Booking History for " << user.name << ":\n";
            for (const auto& booking : user.booking_history)
            {
                auto hotelIt = hotelDatabase.find(booking);
                if (hotelIt != hotelDatabase.end())
                {
                    const Hotel& hotel = hotelIt->second;
                    std::cout << "Hotel Name: " << hotel.name << ", Location: " << hotel.location << ", Price: " << hotel.price << ", Rating: " << hotel.rating << "\n";
                }
                else
                {
                    std::cout << "Hotel ID " << booking << " not found.\n";
                    logMessage("Hotel ID " + booking + " not found in booking history for user ID: " + user_id, WARNING);
                }
            }
        }
        logMessage("Viewed booking history for user ID: " + user_id, INFO);
    }
    else
    {
        std::cout << "User not found.\n";
        logMessage("Attempt to view booking history for non-existent user ID: " + user_id, WARNING);
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();
}


/* Function to cancel a booking
void cancelBooking(const std::string& user_id)
{

    printHeader("Cancel Booking");

    auto userIt = userDatabase.find(user_id);
    if (userIt != userDatabase.end())
    {
        User& user = userIt->second;

        if (user.booking_history.empty())
        {
            std::cout << "No bookings found for " << user.name << ".\n";
            logMessage("Attempted to cancel booking for user with no bookings: " + user_id, INFO);
        }
        else
        {
            std::cout << "Booking History for " << user.name << ":\n";
            for (size_t i = 0; i < user.booking_history.size(); ++i)
            {
                auto hotelIt = hotelDatabase.find(user.booking_history[i]);
                if (hotelIt != hotelDatabase.end())
                {
                    const Hotel& hotel = hotelIt->second;
                    std::cout << i + 1 << ". Hotel Name: " << hotel.name << ", Location: " << hotel.location << ", Price: " << hotel.price << ", Rating: " << hotel.rating << "\n";
                }
                else
                {
                    std::cout << i + 1 << ". Hotel ID " << user.booking_history[i] << " not found.\n";
                    logMessage("Hotel ID " + user.booking_history[i] + " not found in booking history for user ID: " + user_id, WARNING);
                }
            }

            std::cout << "Enter the number of the booking to cancel: ";
            size_t booking_index;
            std::cin >> booking_index;
            std::cin.ignore(); // Consume newline

            if (booking_index > 0 && booking_index <= user.booking_history.size())
            {
                user.booking_history.erase(user.booking_history.begin() + booking_index - 1);
                std::cout << "Booking cancelled successfully.\n";
                logMessage("User " + user_id + " cancelled booking number " + std::to_string(booking_index) + ".", INFO);
            }
            else
            {
                std::cout << "Invalid booking number.\n";
                logMessage("Invalid booking number entered for cancellation by user ID: " + user_id, WARNING);
            }
        }
    }
    else
    {
        std::cout << "User not found.\n";
        logMessage("Attempt to cancel booking for non-existent user ID: " + user_id, WARNING);
    }

    std::cout << "Press Enter to return to the main menu...";
    std::cin.ignore();

}*/

// Main Menu
void mainMenu()
{
    std::string loggedInUserId;
    displayWelcomeMessage();
    while (true)
    {
        printHeader("Hotel Management System");
        std::cout << "1. Register User\n";
        std::cout << "2. Register Hotel\n";
        std::cout << "3. Login\n";
        std::cout << "4. Search Hotels\n";
        std::cout << "5. View All Users\n";
        std::cout << "6. View All Hotels\n";
        std::cout << "7. Delete Hotel\n";
        std::cout << "8. Edit User Information\n";
        std::cout << "9. Edit Hotel Information\n";
        std::cout << "10. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // Consume newline

        switch (choice)
        {
            case 1:
                userRegistrationForm();
                break;
            case 2:
                hotelRegistrationForm();
                break;
            case 3:
                if (!loginUser(loggedInUserId))
                {
                    std::cout << "Login failed. Returning to main menu...\n";
                }
                break;
            case 4:
                searchHotels(); // No arguments should be passed.

                break;
            /*case 5:
                if (!loggedInUserId.empty()) {
                    bookHotel(loggedInUserId);
                } else {
                    std::cout << "Please log in first.\n";
                }*/
                break;
            case 5:
                viewAllUsers();
                break;
            case 6:
                viewAllHotels();
                break;
            case 7:
                deleteHotel();
                break;
            case 8:
                if (!loggedInUserId.empty())
                {
                    editUserInformation(loggedInUserId);
                }
                else
                {
                    std::cout << "Please log in first.\n";
                }
                break;
            case 9:
                editHotelInformation();
                break;
            /*case 10:
                if (!loggedInUserId.empty()) {
                    viewBookingHistory(loggedInUserId);
                } else {
                    std::cout << "Please log in first.\n";
                }
                break;

              case 13:
                if (!loggedInUserId.empty())
                {
                    cancelBooking(loggedInUserId);
                }
                else
                {
                    std::cout << "Please log in first.\n";
                }*/
                break;
            case 10:
                if (confirmExit())
                {
                    saveUserData();
                    saveHotelData();
                    std::cout << "Data saved. Exiting...\n";
                    return;
                }
                break;
            default:
                std::cout << "Invalid choice! Please try again.\n";
        }

        std::cout << "Press Enter to continue...";
        std::cin.ignore();
    }
}

// Main function
int module1()
{
    loadUserData();
    loadHotelData();
    mainMenu();
    return 0;
}










//header files
#include <iostream>    // For input and output operations
#include <vector>      // For using the vector container
#include <iomanip>     // For setting output formatting
#include <string>      // For using the string class
#include <algorithm>   // For using algorithms like find, sort, etc.
#include <fstream>     // For file input and output operations
#include <sstream>     // For string stream operations
#include <map>         // For using the map container


using namespace std;


class MenuItem

{

public:

    string name;
    double price;

    MenuItem(string n, double p) : name(n), price(p) {}


};

class OrderItem

{

public:

    MenuItem item;
    int quantity;

    OrderItem(MenuItem i, int q) : item(i), quantity(q) {}

};

class FoodOrderingSystem

 {

private:

    vector<MenuItem> vegMenu;

    vector<MenuItem> nonVegMenu;

    vector<MenuItem>ItalianCuisine;

    vector<MenuItem>FrenchCuisine;

    vector<MenuItem>ChineseCuisine;

    vector<MenuItem>japaneseCuisine;

    vector<MenuItem>indianCuisine;

    vector<MenuItem>MexicanCuisine;

    vector<MenuItem>thaiCuisine;

    vector<MenuItem>AmericanCuisine;

    vector<MenuItem>KoreanCuisine;

    vector<OrderItem> order;

    // Function to initialize vegetarian menu
    void initializeVegMenu()

    {


        vegMenu.push_back(MenuItem("Veg Burger", 5.99));

        vegMenu.push_back(MenuItem("Veg Pizza", 8.99));

        vegMenu.push_back(MenuItem("Veg Pasta", 7.99));

        vegMenu.push_back(MenuItem("Salad", 4.99));

        vegMenu.push_back(MenuItem("Veg Sandwich", 6.49));

        vegMenu.push_back(MenuItem("Paneer Tikka", 9.99));

        vegMenu.push_back(MenuItem("Veg Biryani", 6.99));

        vegMenu.push_back(MenuItem("Tofu Stir Fry", 10.99));

        vegMenu.push_back(MenuItem("idli", 6.23));

        vegMenu.push_back(MenuItem(" masala dosa", 8.12));

        vegMenu.push_back(MenuItem("neer dosa", 6.49));

        vegMenu.push_back(MenuItem("plain dosa", 4.69));

        vegMenu.push_back(MenuItem("vada pav", 8.19));

        vegMenu.push_back(MenuItem("pav bhaji", 5.76));

        vegMenu.push_back(MenuItem("combo vada pav", 5.72));

        vegMenu.push_back(MenuItem("aloo paratha", 9.93));

        vegMenu.push_back(MenuItem("veg rice", 4.63));

        vegMenu.push_back(MenuItem("fried rice", 6.10));

        vegMenu.push_back(MenuItem("samosa", 6.94));

        vegMenu.push_back(MenuItem("Veg samosa", 7.45));

        vegMenu.push_back(MenuItem("masala paratha", 8.43));

        vegMenu.push_back(MenuItem("pakoda", 7.29));

        vegMenu.push_back(MenuItem("cheese pav bhaji", 2.94));

        vegMenu.push_back(MenuItem("masala pav", 6.95));

        vegMenu.push_back(MenuItem("rava masala dosa", 5.43));

        vegMenu.push_back(MenuItem("mysore masala dosa", 5.41));

        vegMenu.push_back(MenuItem("idli fry", 6.99));


        vegMenu.push_back(MenuItem("uttappa", 6.99));

        vegMenu.push_back(MenuItem("rumoli roti", 8.31));

        vegMenu.push_back(MenuItem("mix veg", 9.00));


        vegMenu.push_back(MenuItem("veg noodles", 7.92));

        vegMenu.push_back(MenuItem("french fries", 6.45));



    }



    // Function to initialize non-vegetarian menu
    void initializeNonVegMenu()

    {


        nonVegMenu.push_back(MenuItem("Chicken Burger", 6.99));

        nonVegMenu.push_back(MenuItem("Pepperoni Pizza", 9.99));

        nonVegMenu.push_back(MenuItem("Chicken Pasta", 8.99));

        nonVegMenu.push_back(MenuItem("Grilled Chicken", 12.99));

        nonVegMenu.push_back(MenuItem("Fish Taco", 5.49));

        nonVegMenu.push_back(MenuItem("Chicken Biryani", 7.99));

        nonVegMenu.push_back(MenuItem("Lamb Kebab", 11.99));

        nonVegMenu.push_back(MenuItem("Shrimp Scampi", 13.99));

        nonVegMenu.push_back(MenuItem("Chicken 65", 6.91));

        nonVegMenu.push_back(MenuItem("Chilly fish", 7.34));

        nonVegMenu.push_back(MenuItem("Biryani", 6.45));

        nonVegMenu.push_back(MenuItem("Fish curry", 5.67));

        nonVegMenu.push_back(MenuItem("Butter chicken", 8.83));

        nonVegMenu.push_back(MenuItem("Ginger fish", 7.24));

        nonVegMenu.push_back(MenuItem("Soya chicken", 6.34));

        nonVegMenu.push_back(MenuItem("Loose prawns", 8.45));

        nonVegMenu.push_back(MenuItem("lemon chicken", 7.76));

        nonVegMenu.push_back(MenuItem("pepper  chicken", 4.99));

        nonVegMenu.push_back(MenuItem(" chicken 555", 12.94));

        nonVegMenu.push_back(MenuItem("Prawn 65", 13.23));


        nonVegMenu.push_back(MenuItem("mutton", 6.41));

        nonVegMenu.push_back(MenuItem("Mutton 65", 7.29));

        nonVegMenu.push_back(MenuItem("Crab soup", 8.63));

        nonVegMenu.push_back(MenuItem("Fry squid", 9.92));

        nonVegMenu.push_back(MenuItem("Squid Curry", 8.09));

        nonVegMenu.push_back(MenuItem("Egg rice", 7.18));

        nonVegMenu.push_back(MenuItem("Egg Curry", 6.60));

    }

    void initializeItalianCuisine()

    {
        ItalianCuisine.push_back(MenuItem("Pizza", 8.99));

        ItalianCuisine.push_back(MenuItem("Pasta", 12.99));

        ItalianCuisine.push_back(MenuItem("Risotto", 14.99));

        ItalianCuisine.push_back(MenuItem("Tiramisu", 6.99));

        ItalianCuisine.push_back(MenuItem("Tiramisu", 6.99));

        ItalianCuisine.push_back(MenuItem("Gelato", 4.99));

        ItalianCuisine.push_back(MenuItem("Bruschetta", 5.99));

        ItalianCuisine.push_back(MenuItem("Tiramisu", 5.99));

         ItalianCuisine.push_back(MenuItem("Panna Cotta", 4.99));

         ItalianCuisine.push_back(MenuItem("Caprese Salad", 7.99));

         ItalianCuisine.push_back(MenuItem("Arancini", 6.49));

         ItalianCuisine.push_back(MenuItem("Gnocchi", 9.49));

         ItalianCuisine.push_back(MenuItem("Bruschetta", 5.49));

         ItalianCuisine.push_back(MenuItem("Gelato", 3.99));

         ItalianCuisine.push_back(MenuItem("Polenta", 7.49));


    }


    void initializeFrenchCuisine()

    {
        FrenchCuisine.push_back(MenuItem("Croissants", 3.50));

        FrenchCuisine.push_back(MenuItem("Coq au Vin", 18.99));

        FrenchCuisine.push_back(MenuItem("Ratatouille", 13.99));

        FrenchCuisine.push_back(MenuItem("Bouillabaisse", 21.99));

        FrenchCuisine.push_back(MenuItem("Escargot", 15.99));

        FrenchCuisine.push_back(MenuItem("Cr me Br l e", 7.99));

        FrenchCuisine.push_back(MenuItem("Baguette", 2.50));

        FrenchCuisine.push_back(MenuItem("Quiche Lorraine", 10.99));

        FrenchCuisine.push_back(MenuItem("Beef Bourguignon", 16.99));

        FrenchCuisine.push_back(MenuItem("Quiche Lorraine", 8.49));

        FrenchCuisine.push_back(MenuItem("Soupe   l'Oignon", 6.99));

        FrenchCuisine.push_back(MenuItem("Cr pes", 5.49));

        FrenchCuisine.push_back(MenuItem("Macarons", 3.99));

        FrenchCuisine.push_back(MenuItem(" clairs", 4.49));






    }

    void initializeChineseCuisine()

    {
        ChineseCuisine.push_back(MenuItem("Peking Duck", 24.99));

        ChineseCuisine.push_back(MenuItem("Dumplings", 8.99));

        ChineseCuisine.push_back(MenuItem("Sweet and Sour Pork", 12.99));

        ChineseCuisine.push_back(MenuItem("Kung Pao Chicken", 11.99));

        ChineseCuisine.push_back(MenuItem("Hot Pot", 25.99));

        ChineseCuisine.push_back(MenuItem("Dim Sum", 15.99));

        ChineseCuisine.push_back(MenuItem("Chow Mein", 9.99));

        ChineseCuisine.push_back(MenuItem("Spring Rolls", 6.99));
    }

    void initializejapaneseCuisine()

    {

          japaneseCuisine.push_back(MenuItem("Sushi", 19.99));

         japaneseCuisine.push_back(MenuItem("Sashimi", 21.99));

         japaneseCuisine.push_back(MenuItem("Ramen", 12.99));

         japaneseCuisine.push_back(MenuItem("Tempura", 14.99));

         japaneseCuisine.push_back(MenuItem("Miso Soup", 3.99));

         japaneseCuisine.push_back(MenuItem("Okonomiyaki", 10.99));

         japaneseCuisine.push_back(MenuItem("Mochi", 4.99));

         japaneseCuisine.push_back(MenuItem("Takoyaki", 8.49));

         japaneseCuisine.push_back(MenuItem("Udon", 11.49));

         japaneseCuisine.push_back(MenuItem("Teriyaki Chicken", 13.99));

         japaneseCuisine.push_back(MenuItem("Matcha Ice Cream", 5.99));


    }

    void initializeindianCuisine()

    {
        indianCuisine.push_back(MenuItem("Butter Chicken", 14.99));

        indianCuisine.push_back(MenuItem("Paneer Tikka", 12.99));

        indianCuisine.push_back(MenuItem("Biryani", 13.49));

        indianCuisine.push_back(MenuItem("Masala Dosa", 8.99));


        indianCuisine.push_back(MenuItem("Samosa", 4.99));

       indianCuisine.push_back(MenuItem("Naan", 2.99));

       indianCuisine.push_back(MenuItem("Dal Tadka", 9.99));


       indianCuisine.push_back(MenuItem("Aloo Paratha", 7.49));

       indianCuisine.push_back(MenuItem("Chole Bhature", 10.99));

        indianCuisine.push_back(MenuItem("Pav Bhaji", 8.49));

       indianCuisine.push_back(MenuItem("Rogan Josh", 15.99));

       indianCuisine.push_back(MenuItem("Gulab Jamun", 5.49));


        indianCuisine.push_back(MenuItem("Ras Malai", 6.49));
    }

    void initializeMexicanCuisine()


    {
        MexicanCuisine.push_back(MenuItem("Tacos", 9.99));

        MexicanCuisine.push_back(MenuItem("Burritos", 12.99));

        MexicanCuisine.push_back(MenuItem("Quesadillas", 10.99));


        MexicanCuisine.push_back(MenuItem("Guacamole", 6.99));

        MexicanCuisine.push_back(MenuItem("Enchiladas", 13.99));

        MexicanCuisine.push_back(MenuItem("Churros", 5.99));

        MexicanCuisine.push_back(MenuItem("Nachos", 8.49));

        MexicanCuisine.push_back(MenuItem("Tamales", 11.49));

        MexicanCuisine.push_back(MenuItem("Fajitas", 14.99));

        MexicanCuisine.push_back(MenuItem("Pozole", 15.99));


        MexicanCuisine.push_back(MenuItem("Elote", 4.99));

    }


    void initializethaiCuisine()

    {
        thaiCuisine.push_back(MenuItem("Pad Thai", 5.99));



        thaiCuisine.push_back(MenuItem("Tom Yum", 6.99));

        thaiCuisine.push_back(MenuItem("Green Curry", 7.99));

        thaiCuisine.push_back(MenuItem("Massaman Curry", 8.99));

        thaiCuisine.push_back(MenuItem("Som Tum", 4.99));

        thaiCuisine.push_back(MenuItem("Khao Pad", 6.49));

        thaiCuisine.push_back(MenuItem("Moo Pad Krapow", 7.49));

        thaiCuisine.push_back(MenuItem("Pad See Ew", 6.99));

        thaiCuisine.push_back(MenuItem("Satay", 4.99));

        thaiCuisine.push_back(MenuItem("Khao Soi", 7.99));

        thaiCuisine.push_back(MenuItem("Gai Yang", 8.49));

        thaiCuisine.push_back(MenuItem("Sticky Rice with Mango", 3.99));



    }

    void initializeAmericanCuisine()
    {
    AmericanCuisine.push_back(MenuItem("Burger", 5.99));

     AmericanCuisine.push_back(MenuItem("Hot Dog", 3.99));

    AmericanCuisine.push_back(MenuItem("Mac and Cheese", 6.49));

    AmericanCuisine.push_back(MenuItem("Apple Pie", 2.99));

    AmericanCuisine.push_back(MenuItem("Buffalo Wings", 7.99));

    AmericanCuisine.push_back(MenuItem("Clam Chowder", 5.49));

    AmericanCuisine.push_back(MenuItem("BBQ Ribs", 12.99));

    AmericanCuisine.push_back(MenuItem("Grilled Cheese", 4.99));

    AmericanCuisine.push_back(MenuItem("Cheesesteak", 8.49));

    AmericanCuisine.push_back(MenuItem("Cornbread", 3.49));

    AmericanCuisine.push_back(MenuItem("Pancakes", 4.99));


    AmericanCuisine.push_back(MenuItem("Fried Chicken", 8.99));




    }

    void initializeKoreanCuisine()

    {
    KoreanCuisine.push_back(MenuItem("Kimchi", 3.99));

    KoreanCuisine.push_back(MenuItem("Bibimbap", 7.99));

    KoreanCuisine.push_back(MenuItem("Bulgogi", 9.99));

    KoreanCuisine.push_back(MenuItem("Japchae", 8.49));

    KoreanCuisine.push_back(MenuItem("Samgyeopsal", 12.99));

    KoreanCuisine.push_back(MenuItem("Tteokbokki", 6.49));

    KoreanCuisine.push_back(MenuItem("Jjigae", 7.49));

    KoreanCuisine.push_back(MenuItem("Banchan", 4.99));

    KoreanCuisine.push_back(MenuItem("Sundubu Jjigae", 8.99));

    KoreanCuisine.push_back(MenuItem("Hotteok", 2.99));

    KoreanCuisine.push_back(MenuItem("Kimbap", 5.99));


    KoreanCuisine.push_back(MenuItem("Hoddeok", 3.49));



    }



public:

    FoodOrderingSystem()

     {

        initializeVegMenu();

        initializeNonVegMenu();

        initializeItalianCuisine();

        initializeFrenchCuisine();

        initializeChineseCuisine();

        initializejapaneseCuisine();

        initializeindianCuisine();

        initializeMexicanCuisine();

        initializethaiCuisine();

        initializeAmericanCuisine();

        initializeKoreanCuisine();

    }

    // Function to display the vegetarian menu
    void displayVegMenu()
     {

        cout << "\nVegetarian Menu:\n";
        for (size_t i = 0; i < vegMenu.size(); ++i) {
            cout << i + 1 << ". " << vegMenu[i].name << " - $" << fixed << setprecision(2) << vegMenu[i].price << "\n";
        }

    }


    // Function to display the non-vegetarian menu
    void displayNonVegMenu()
    {


        cout << "\nNon-Vegetarian Menu:\n";
        for (size_t i = 0; i < nonVegMenu.size(); ++i)
        {
            cout << i + 1 + vegMenu.size() << ". " << nonVegMenu[i].name << " - $" << fixed << setprecision(2) << nonVegMenu[i].price << "\n";
        }


    }




    void displayItalianCuisine()

    {

    cout << "\nItalianCuisine:\n";
        for (size_t i = 0; i < ItalianCuisine.size(); ++i)
        {
            cout << i + 1 + vegMenu.size() + nonVegMenu.size() << ". " << ItalianCuisine[i].name << " - $" << fixed << setprecision(2) << ItalianCuisine[i].price << "\n";
        }






    }
    void displayFrenchCuisine()
    {


        cout << "\nFrenchCuisine:\n";
        for (size_t i = 0; i < FrenchCuisine.size(); ++i)
            {
            cout << i + 1  + nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()<< ". " << FrenchCuisine[i].name << " - $" << fixed << setprecision(2) << FrenchCuisine[i].price << "\n";
        }


    }

     void displayChineseCuisine()
    {


        cout << "\nChineseCuisine:\n";
        for (size_t i = 0; i < ChineseCuisine.size(); ++i)
            {
            cout << i + 1 + nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+FrenchCuisine.size() << ". " << ChineseCuisine[i].name << " - $" << fixed << setprecision(2) << ChineseCuisine[i].price << "\n";
        }


    }

    void displayjapaneseCuisine()
    {


        cout << "\njapaneseCuisine:\n";
        for (size_t i = 0; i < japaneseCuisine.size(); ++i)
            {
            cout << i + 1 + nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+FrenchCuisine.size()+ChineseCuisine.size() << ". " << japaneseCuisine[i].name << " - $" << fixed << setprecision(2) << japaneseCuisine[i].price << "\n";
        }


    }

    void displayindianCuisine()
    {


        cout << "\nindianCuisine:\n";
        for (size_t i = 0; i < indianCuisine.size(); ++i)
            {
            cout << i + 1 +  nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+FrenchCuisine.size()+ChineseCuisine.size()+japaneseCuisine.size() << ". " << indianCuisine[i].name << " - $" << fixed << setprecision(2) <<indianCuisine[i].price << "\n";
        }


    }

    void displayMexicanCuisine()
    {


        cout << "\nMexicanCuisine:\n";
        for (size_t i = 0; i < MexicanCuisine.size(); ++i)
            {
            cout << i + 1 + nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+FrenchCuisine.size()+ChineseCuisine.size()+japaneseCuisine.size()+indianCuisine.size() << ". " << MexicanCuisine[i].name << " - $" << fixed << setprecision(2) <<MexicanCuisine[i].price << "\n";
        }


    }



   void displaythaiCuisine()
    {


        cout << "\nthaiCuisine:\n";
        for (size_t i = 0; i < thaiCuisine.size(); ++i)
            {
            cout << i + 1 +  nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+FrenchCuisine.size()+ChineseCuisine.size()+japaneseCuisine.size()+indianCuisine.size()+MexicanCuisine.size() << ". " << thaiCuisine[i].name << " - $" << fixed << setprecision(2) <<thaiCuisine[i].price << "\n";
        }


    }

    void displayAmericanCuisine()
    {


        cout << "\nAmericanCuisine:\n";
        for (size_t i = 0; i < AmericanCuisine.size(); ++i)
            {
            cout << i + 1 + nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+FrenchCuisine.size()+ChineseCuisine.size()+japaneseCuisine.size()+indianCuisine.size()+MexicanCuisine.size()+thaiCuisine.size() << ". " << AmericanCuisine[i].name << " - $" << fixed << setprecision(2) <<AmericanCuisine[i].price << "\n";
        }


    }

    void displayKoreanCuisine()
    {


        cout << "\nAmericanCuisine:\n";
        for (size_t i = 0; i < KoreanCuisine.size(); ++i)
            {
            cout << i + 1 + nonVegMenu.size()+vegMenu.size()+ItalianCuisine.size()+ FrenchCuisine.size()+ChineseCuisine.size()+japaneseCuisine.size()+indianCuisine.size()+MexicanCuisine.size()+thaiCuisine.size()+AmericanCuisine.size() << ". " << KoreanCuisine[i].name << " - $" << fixed << setprecision(2) <<KoreanCuisine[i].price << "\n";
        }


    }




// Function to display both menus
    void displayMenu()

     {
        displayVegMenu();
        displayNonVegMenu();
        displayItalianCuisine();
        displayFrenchCuisine();
        displayChineseCuisine();
        displayjapaneseCuisine();
        displayindianCuisine();
        displayMexicanCuisine();
        displaythaiCuisine();
        displayAmericanCuisine();
        displayKoreanCuisine();

    }

    // Function to place an order
   void placeOrder(int itemNumber, int quantity)

    {
    if (itemNumber < 1 || itemNumber > vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size() + thaiCuisine.size() + AmericanCuisine.size() + KoreanCuisine.size()) {
        cout << "Invalid item number!\n";
        return;
    }

    // Checking if itemNumber corresponds to vegetarian menu
    if (itemNumber <= vegMenu.size())

        {
        order.push_back(OrderItem(vegMenu[itemNumber - 1], quantity));
    }


    // Checking if itemNumber corresponds to non-vegetarian menu
    else if (itemNumber <= vegMenu.size() + nonVegMenu.size())

    {
        order.push_back(OrderItem(nonVegMenu[itemNumber - vegMenu.size() - 1], quantity));
    }


    // Checking if itemNumber corresponds to Italian cuisine menu
    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size())


    {
        order.push_back(OrderItem(ItalianCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - 1], quantity));
    }


    // Checking if itemNumber corresponds to French cuisine menu
    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size())

    {
        order.push_back(OrderItem(FrenchCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - 1], quantity));
    }
    // Checking if itemNumber corresponds to chinese cuisine menu
    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size())


    {
        order.push_back(OrderItem(ChineseCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - 1], quantity));
    }
    // Checking if itemNumber corresponds to japanese cuisine menu

    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size())


    {
        order.push_back(OrderItem(japaneseCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - 1], quantity));
    }
    // Checking if itemNumber corresponds to indian cuisine menu



    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size())


    {
        order.push_back(OrderItem(indianCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - 1], quantity));
    }


    // Checking if itemNumber corresponds to Mexican cuisine menu



    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size()) {
        order.push_back(OrderItem(MexicanCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - 1], quantity));
    }


    // Checking if itemNumber corresponds to thai cuisine menu


    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size() + thaiCuisine.size()) {
        order.push_back(OrderItem(thaiCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - MexicanCuisine.size() - 1], quantity));
    }


    // Checking if itemNumber corresponds to American cuisine menu

    else if (itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size() + thaiCuisine.size() + AmericanCuisine.size()) {
        order.push_back(OrderItem(AmericanCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - MexicanCuisine.size() - thaiCuisine.size() - 1], quantity));
    }


    // Checking if itemNumber corresponds to korean cuisine menu
    else

    {
        order.push_back(OrderItem(KoreanCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - MexicanCuisine.size() - thaiCuisine.size() - AmericanCuisine.size() - 1], quantity));
    }

    // Displaying the order confirmation with item details
    cout << "Added " << quantity << " x "
         << (itemNumber <= vegMenu.size() ? vegMenu[itemNumber - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() ? nonVegMenu[itemNumber - vegMenu.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() ? ItalianCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() ? FrenchCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() ? ChineseCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() ? japaneseCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() ? indianCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size() ? MexicanCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size() + thaiCuisine.size() ? thaiCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - MexicanCuisine.size() - 1].name :
             itemNumber <= vegMenu.size() + nonVegMenu.size() + ItalianCuisine.size() + FrenchCuisine.size() + ChineseCuisine.size() + japaneseCuisine.size() + indianCuisine.size() + MexicanCuisine.size() + thaiCuisine.size() + AmericanCuisine.size() ? AmericanCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - MexicanCuisine.size() - thaiCuisine.size() - 1].name :
             KoreanCuisine[itemNumber - vegMenu.size() - nonVegMenu.size() - ItalianCuisine.size() - FrenchCuisine.size() - ChineseCuisine.size() - japaneseCuisine.size() - indianCuisine.size() - MexicanCuisine.size() - thaiCuisine.size() - AmericanCuisine.size() - 1].name)
         << " to your order.\n";
}



    // Function to view the current order
    void viewOrder()
    {
        if (order.empty())

            {

            cout << "Your order is empty.\n";
            return;
        }

        cout << "\nYour Order:\n";
        double total = 0.0;
        for (const auto& o : order)
        {
            double itemTotal = o.quantity * o.item.price;
            cout << o.quantity << " x " << o.item.name << " - $" << fixed << setprecision(2) << itemTotal << "\n";
            total += itemTotal;
        }
        cout << "Total Bill: $" << fixed << setprecision(2) << total << "\n";
    }

    // Function to clear the current order
    void clearOrder()

     {
        order.clear();
        cout << "Your order has been cleared.\n";
    }
    void searchMenu(const string& keyword)
    {
        cout << "\nSearch Results for '" << keyword << "':\n";
        bool found = false;
        for (const auto& item : vegMenu)
        {
            if (item.name.find(keyword) != string::npos)
            {
                cout << item.name << " - $" << fixed << setprecision(2) << item.price << "\n";
                found = true;
            }
        }
        for (const auto& item : nonVegMenu)
            {
            if (item.name.find(keyword) != string::npos)
            {
                cout << item.name << " - $" << fixed << setprecision(2) << item.price << "\n";
                found = true;
            }
        }
        if (!found)
        {
            cout << "No matching items found.\n";
        }
    }

    void sortMenuByPrice()

    {
    auto compare = [](const MenuItem& a, const MenuItem& b)
    {
        return a.price < b.price;
    };

    sort(vegMenu.begin(), vegMenu.end(), compare);
    sort(nonVegMenu.begin(), nonVegMenu.end(), compare);
    sort(ItalianCuisine.begin(), ItalianCuisine.end(), compare);
    sort(FrenchCuisine.begin(), FrenchCuisine.end(), compare);
    sort(ChineseCuisine.begin(), ChineseCuisine.end(), compare);
    sort(japaneseCuisine.begin(), japaneseCuisine.end(), compare);
    sort(indianCuisine.begin(), indianCuisine.end(), compare);
    sort(MexicanCuisine.begin(), MexicanCuisine.end(), compare);
    sort(thaiCuisine.begin(), thaiCuisine.end(), compare);
    sort(AmericanCuisine.begin(), AmericanCuisine.end(), compare); // American cuisine
    sort(KoreanCuisine.begin(), KoreanCuisine.end(), compare); // Korean cuisine

    cout << "Menus sorted by price.\n";
    }
    void displayTopItems()
    {
    vector<MenuItem> allItems = vegMenu;

    // Merging all menus into allItems
    allItems.insert(allItems.end(), nonVegMenu.begin(), nonVegMenu.end());
    allItems.insert(allItems.end(), ItalianCuisine.begin(), ItalianCuisine.end());
    allItems.insert(allItems.end(), FrenchCuisine.begin(), FrenchCuisine.end());
    allItems.insert(allItems.end(), ChineseCuisine.begin(), ChineseCuisine.end());
    allItems.insert(allItems.end(), japaneseCuisine.begin(), japaneseCuisine.end());
    allItems.insert(allItems.end(), indianCuisine.begin(), indianCuisine.end());
    allItems.insert(allItems.end(), MexicanCuisine.begin(), MexicanCuisine.end());
    allItems.insert(allItems.end(), thaiCuisine.begin(), thaiCuisine.end());
    allItems.insert(allItems.end(), AmericanCuisine.begin(), AmericanCuisine.end());
    allItems.insert(allItems.end(), KoreanCuisine.begin(), KoreanCuisine.end());

    // Sorting all items in descending order of price
    sort(allItems.begin(), allItems.end(), [](const MenuItem& a, const MenuItem& b) {
        return a.price > b.price;
    });

    // Display the top 3 most expensive items
    cout << "\nTop 3 Most Expensive Items:\n";
    for (size_t i = 0; i < 3 && i < allItems.size(); ++i) {
        cout << i + 1 << ". " << allItems[i].name << " - $" << fixed << setprecision(2) << allItems[i].price << "\n";
    }
}

   void calculateAveragePrice()

   {
    vector<MenuItem> allItems = vegMenu;

    // Merging all menus into allItems
    allItems.insert(allItems.end(), nonVegMenu.begin(), nonVegMenu.end());
    allItems.insert(allItems.end(), ItalianCuisine.begin(), ItalianCuisine.end()); // Changed to ItalianCuisine
    allItems.insert(allItems.end(), FrenchCuisine.begin(), FrenchCuisine.end());
    allItems.insert(allItems.end(), ChineseCuisine.begin(), ChineseCuisine.end());
    allItems.insert(allItems.end(), japaneseCuisine.begin(), japaneseCuisine.end());
    allItems.insert(allItems.end(), indianCuisine.begin(), indianCuisine.end());
    allItems.insert(allItems.end(), MexicanCuisine.begin(), MexicanCuisine.end());
    allItems.insert(allItems.end(), thaiCuisine.begin(), thaiCuisine.end());
    allItems.insert(allItems.end(), AmericanCuisine.begin(), AmericanCuisine.end());
    allItems.insert(allItems.end(), KoreanCuisine.begin(), KoreanCuisine.end());

    // Calculating the total price
    double total = 0.0;
    for (const auto& item : allItems)
        {
        total += item.price;
    }

    // Calculating the average price
    double avg = allItems.empty() ? 0.0 : total / allItems.size();

    // Displaying the average price
    cout << "Average Price of All Menu Items: $" << fixed << setprecision(2) << avg << "\n";
}

    // Function to display the receipt in a table format and clear the order
    void displayReceipt()

    {
        if (order.empty())

            {
            cout << "Your order is empty.\n";
            return;
        }

        cout << "\nReceipt:\n";
        cout << left << setw(10) << "Qty" << setw(20) << "Item" << setw(10) << "Price" << setw(10) << "Total" << "\n";
        cout << string(50, '-') << "\n"; // Separator line

        double total = 0.0;
        for (const auto& o : order)

            {
            double itemTotal = o.quantity * o.item.price;
            cout << left << setw(10) << o.quantity << setw(20) << o.item.name << setw(10) << fixed << setprecision(2) << o.item.price << setw(10) << itemTotal << "\n";
            total += itemTotal;
        }
        cout << string(50, '-') << "\n"; // Separator line
        cout << left << setw(40) << "Total Bill:" << "$" << fixed << setprecision(2) << total << "\n";

        clearOrder();
    }

};

int module2()

{


    FoodOrderingSystem system;

    int choice;
    do
        {

        cout << "\n1. Display Menu:";
        cout <<"\n2. Place Order:\n3.View Order\n4. Clear Order\n5.search item \n6.display items \n7.sort menu by price \n8.Display top items \n9.calculate avg price \n10.exit   \nEnter your choice: ";
        cin >> choice;

        switch (choice)

         {
            case 1:
                system.displayMenu();
                break;
            case 2:
                int itemNumber, quantity;
                cout << "Enter item number: ";
                cin >> itemNumber;
                cout << "Enter quantity: ";
                cin >> quantity;
                system.placeOrder(itemNumber, quantity);
                break;
            case 3:
                system.viewOrder();
                break;
            case 4:
                system.clearOrder();
                break;
            case 5: {
                string keyword;
                cout << "Enter keyword to search: ";
                cin.ignore(); // Clear the input buffer
                getline(cin, keyword);
                system.searchMenu(keyword);
                break;
            }
            case 6:
                system.displayReceipt();
                cout << "Thank you for using the Food Ordering System!\n";
                break;
            case 7:
                system.sortMenuByPrice();
                break;
            case 8:
                system.displayTopItems();
                break;
            case 9:
                system.calculateAveragePrice();
                break;
            case 10:
                cout << "Exiting the system" << endl;
                return 0; // Exit the program
            default:
                cout << "Invalid choice! Please try again.\n";
        }


    }
    while (choice != 11);

    return 0;
}


int main()
{
    int c;

    while (true)
    { // Infinite loop to keep the program running
        #include <iostream>
using namespace std;

        cout << "********************************************" << endl;
        cout << "*------------------------------------------*" << endl;
        cout << "*-----Welcome to Online Food Delivery------*" << endl;
        cout << "*------------Management System!------------*" << endl;
        cout << "*------------------------------------------*" << endl;
        cout << "********************************************\n\n" << endl;
        cout << "***********Main Menu***********\n";
        cout<< "1:register for hotels and users \n";
        cout << "2:list of all foods \n";
        cout << "3:shortest distance and efficient delivery\n";
        cout << "4:feedback of users and hotels\n";
        cout << "5:exit\n";
        cout << "Enter your choice:";
        cin >> c;

        // Check if input is valid
        if (cin.fail())
        {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            cout << "Invalid input. Please enter a number." << endl;
            continue; // Restart the loop
        }

        cout << "You entered: " << c << endl;

        switch (c)
        {

            case 1:
                module1();
                break;
             case 2:
                module2();
                break;
            case 3:
                module3();
                break; // Exit the switch and continue the loop
            case 4:
                module4();
                break;
            case 5:
                cout << "Exiting the system" << endl;
                return 0; // Exit the program
            default:
                cout << "wrong choice";
        }
    }

    return 0;
}
