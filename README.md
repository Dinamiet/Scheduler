# Scheduler

The Scheduler is a lightweight task scheduler designed for embedded applications. It aims to provide a simple and efficient way to schedule tasks without preemption.

# Features
-	Best Effort Scheduling:
Tasks are scheduled on a best effort basis, which means that the scheduler does not guarantee that all tasks will be executed in order or within a certain time frame.
-	Lightweight
The Scheduler is designed to be lightweight and efficient, making it suitable for resource-constrained embedded systems.

# Building
This project utilizes `CMake` for building and integration with other projects. Custom build configurations are recommended to simplify the process of including this library in your own projects.

# Documentation
Documentation for this project is generated using Doxygen (`CMake` target `docs`) to provide a clear and concise reference for users.
