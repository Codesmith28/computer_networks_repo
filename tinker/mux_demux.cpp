#include <bits/stdc++.h>
using namespace std;
/* (╯°□°)╯︵ ┻━┻ */

// Function to implement a multiplexer
int multiplexer(const vector<int> &inputs, const vector<int> &select_lines) {
    int index = 0;
    for (size_t i = 0; i < select_lines.size( ); ++i) {
        index |= (select_lines[i] << i); // Compute the index using select lines
    }
    if (index < inputs.size( )) {
        return inputs[index];
    } else {
        throw out_of_range("Select lines point to an invalid input.");
    }
}

// Function to implement a demultiplexer
vector<int> demultiplexer(int input, const vector<int> &select_lines) {
    int size = 1 << select_lines.size( ); // 2^(number of select lines)
    vector<int> outputs(size, 0);
    int index = 0;
    for (size_t i = 0; i < select_lines.size( ); ++i) {
        index |= (select_lines[i] << i); // Compute the index using select lines
    }
    if (index < size) {
        outputs[index] = input;
    } else {
        throw out_of_range("Select lines point to an invalid output.");
    }
    return outputs;
}

int main( ) {
    // Example for multiplexer
    vector<int> inputs = {10, 20, 30, 40};
    vector<int> select_mux = {1, 0}; // Select line for index 2
    try {
        int mux_output = multiplexer(inputs, select_mux);
        cout << "Multiplexer Output: " << mux_output << endl;
    } catch (const exception &e) {
        cerr << e.what( ) << endl;
    }

    // Example for demultiplexer
    int input = 50;
    vector<int> select_demux = {1, 1}; // Select line for index 3
    try {
        vector<int> demux_outputs = demultiplexer(input, select_demux);
        cout << "Demultiplexer Outputs: ";
        for (int output : demux_outputs) {
            cout << output << " ";
        }
        cout << endl;
    } catch (const exception &e) {
        cerr << e.what( ) << endl;
    }

    return 0;
}
