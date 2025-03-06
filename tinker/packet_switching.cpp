#include <bits/stdc++.h>
using namespace std;

// Structure to represent a packet
struct Packet {
    int id;
    string source;
    string destination;
    string data;
};

// Function to simulate packet generation
Packet generate_packet(int id, const string &source, const string &destination) {
    return {id, source, destination, "Sample Data " + to_string(id)};
}

// Function to simulate packet switching
void switch_packets(queue<Packet> &input_queue, queue<Packet> &output_queue) {
    while (!input_queue.empty( )) {
        Packet packet = input_queue.front( );
        input_queue.pop( );

        cout << "Switching packet ID: " << packet.id << " from " << packet.source << " to " << packet.destination << endl;

        output_queue.push(packet); // Forward the packet to the output queue
    }
}

// Function to simulate packet transmission
void transmit_packets(queue<Packet> &output_queue) {
    while (!output_queue.empty( )) {
        Packet packet = output_queue.front( );
        output_queue.pop( );

        cout << "Transmitting packet ID: " << packet.id << " to destination: " << packet.destination << endl;
    }
}

int main( ) {
    srand(static_cast<unsigned>(time(0)));

    // Define sources and destinations
    vector<string> nodes = {"NodeA", "NodeB", "NodeC", "NodeD"};

    // Input and output queues
    queue<Packet> input_queue;
    queue<Packet> output_queue;

    // Generate random packets
    for (int i = 1; i <= 5; ++i) {
        string source = nodes[rand( ) % nodes.size( )];
        string destination = nodes[rand( ) % nodes.size( )];
        while (destination == source) {
            destination = nodes[rand( ) % nodes.size( )]; // Ensure source != destination
        }
        input_queue.push(generate_packet(i, source, destination));
    }

    // Simulate packet switching
    cout << "Switching Packets..." << endl;
    switch_packets(input_queue, output_queue);

    // Simulate packet transmission
    cout << "\nTransmitting Packets..." << endl;
    transmit_packets(output_queue);

    return 0;
}
