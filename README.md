# Simplified Multi-level Feedback Queue (MLFQ)

## Project Overview

The **MLFQ** project simulates a multi-level feedback queue scheduling algorithm allowing processes to move between different queues based on their execution history and priorities.

## Functionalities

1. **Implementation of Queues**: Develop an N-level feedback queue (with N ranging from 1 to 4) to manage at most 10 processes.
2. **Process Scheduling**: Create a program that effectively schedules processes based on their arrival times and execution behavior.
3. **Output Generation**: Generate a Gantt chart representation of the scheduled processes to visualize the scheduling outcome.

## Implementation Details

### Key Features

- **Process Management**: Implemented a system to handle up to 10 processes, ensuring that the process table is sorted by arrival time.
- **Queue Levels**: Designed a structure to manage multiple levels of queues, with the last queue being First-Come, First-Served (FCFS).
- **Input/Output Handling**: Built the program to accept input from a file and redirect output to another file, facilitating easier testing and validation.
- **Gantt Chart Generation**: Developed functionality to display the final Gantt chart based on the scheduling results.

