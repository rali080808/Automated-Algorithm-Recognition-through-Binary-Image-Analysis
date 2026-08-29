#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

// Event class to represent scheduled events
class Event {
public:
    int id;
    string name;
    int startTime;
    int duration;

    Event(int _id, const string& _name, int _start, int _duration) :
        id(_id), name(_name), startTime(_start), duration(_duration) {}

    int endTime() const {
        return startTime + duration;
    }

    void print() const {
        cout << setw(3) << id << " | "
             << setw(15) << name << " | "
             << setw(5) << startTime << " | "
             << setw(8) << duration << " | "
             << setw(3) << endTime() << endl;
    }
};

// Fenwick Tree for scheduling and resource allocation
class SchedulerFenwick {
private:
    vector<int> tree;      // Stores resource usage count
    int timeSlots;         // Number of time slots
    vector<Event> events;  // Store all events

    // Update the tree at position idx
    void updateTree(int idx, int val) {
        while (idx <= timeSlots) {
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    // Query the tree for resource count up to idx
    int queryTree(int idx) {
        int sum = 0;
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & -idx;
        }
        return sum;
    }

public:
    SchedulerFenwick(int maxTime) : timeSlots(maxTime) {
        tree.assign(timeSlots + 1, 0);
    }

    // Schedule an event, returns event ID if successful, -1 if not
    int scheduleEvent(const string& name, int start, int duration, int maxResources) {
        // Check if event fits within time slots
        if (start <= 0 || start + duration > timeSlots) {
            cout << "Event outside of valid time range." << endl;
            return -1;
        }

        // Check if adding this event would exceed resource limits
        for (int t = start; t <= start + duration; t++) {
            int currentUsage = resourcesAtTime(t);
            if (currentUsage + 1 > maxResources) {
                cout << "Not enough resources available at time " << t << endl;
                return -1;
            }
        }

        // Event is schedulable, add it
        int eventId = events.size() + 1;
        events.emplace_back(eventId, name, start, duration);

        // Update resource usage in the tree
        updateTree(start, 1);             // Increment at start
        updateTree(start + duration, -1); // Decrement at end

        return eventId;
    }

    // Cancel a scheduled event by ID
    bool cancelEvent(int eventId) {
        if (eventId <= 0 || eventId > events.size() || events[eventId-1].id == -1) {
            cout << "Invalid event ID." << endl;
            return false;
        }

        Event& event = events[eventId-1];

        // Update resource usage in the tree
        updateTree(event.startTime, -1);             // Decrement at start
        updateTree(event.startTime + event.duration, 1); // Increment at end

        // Mark as canceled (set ID to -1)
        event.id = -1;

        return true;
    }

    // Get resource usage at a specific time
    int resourcesAtTime(int time) {
        if (time <= 0 || time > timeSlots) return 0;
        return queryTree(time);
    }

    // Get max resource usage in a time range
    int maxResourcesInRange(int start, int end) {
        int maxUsage = 0;
        for (int t = start; t <= end; t++) {
            maxUsage = max(maxUsage, resourcesAtTime(t));
        }
        return maxUsage;
    }

    // List all active events
    void listEvents() {
        cout << "Scheduled Events:" << endl;
        cout << "ID  |      Name      | Start | Duration | End " << endl;
        cout << "----+----------------+-------+----------+-----" << endl;

        for (const Event& event : events) {
            if (event.id != -1) { // Skip canceled events
                event.print();
            }
        }
    }

    // Print resource usage timeline
    void printTimeline() {
        cout << "\nResource Usage Timeline:" << endl;
        cout << "Time: ";
        for (int t = 1; t <= timeSlots; t++) {
            cout << setw(3) << t;
        }
        cout << endl;

        cout << "Used: ";
        for (int t = 1; t <= timeSlots; t++) {
            cout << setw(3) << resourcesAtTime(t);
        }
        cout << endl;
    }
};

int main() {
    // Create a scheduler with 24 time slots
    SchedulerFenwick scheduler(24);

    // Schedule some events (name, start time, duration, max concurrent resources)
    int maxResources = 3;

    cout << "Scheduling events with max " << maxResources << " concurrent resources..." << endl;

    int e1 = scheduler.scheduleEvent("Morning Meeting", 9, 2, maxResources);
    int e2 = scheduler.scheduleEvent("Lunch Break", 12, 1, maxResources);
    int e3 = scheduler.scheduleEvent("Team Work", 10, 4, maxResources);
    int e4 = scheduler.scheduleEvent("Client Call", 11, 1, maxResources);
    int e5 = scheduler.scheduleEvent("Project Review", 15, 3, maxResources);
    int e6 = scheduler.scheduleEvent("Training", 14, 2, maxResources);

    // This should fail as it would exceed resource limits
    int e7 = scheduler.scheduleEvent("Extra Meeting", 11, 2, maxResources);

    // List all events
    scheduler.listEvents();

    // Show resource timeline
    scheduler.printTimeline();

    // Cancel an event
    cout << "\nCanceling event with ID " << e4 << endl;
    scheduler.cancelEvent(e4);

    // Show updated schedule
    scheduler.listEvents();
    scheduler.printTimeline();

    // Now we can schedule the previously failed event
    cout << "\nTrying to schedule the meeting again..." << endl;
    e7 = scheduler.scheduleEvent("Extra Meeting", 11, 2, maxResources);

    // Show final schedule
    scheduler.listEvents();
    scheduler.printTimeline();

    // Check max resource usage during business hours
    int businessHoursMax = scheduler.maxResourcesInRange(9, 17);
    cout << "\nMaximum resource usage during business hours (9-17): "
         << businessHoursMax << endl;

    return 0;
}
