<<<<<<< HEAD
# Sync Service

### Build Instructions
=======


### NOTE: 
This is the repository for the local desktop service (functionality only no ui) built using C++. most up-to-date branch is tcp-communications

### GENERAL PROGRESS REPORT: 
The service now is mostly done for when it comes to getting, saving and handling sync tasks, the UI correctly connects to it and recieves from it all the required information, and the UI currently can be used to add or remove sync tasks.

# ProjectSync
This project, i aim to build an application that handles file syncs, backups locally and for the cloud on windows. It will be essentially a [background service](https://github.com/ezzedineozone/Sync-Service), with a [deskop UI](https://github.com/ezzedineozone/Sync-Service-Ui) that controls it. but the end product will have both included and setup automatically for simplicity.

# Architechture
Two main parts, [the UI](https://github.com/ezzedineozone/Sync-Service-Ui) for ease of use by the users, and a [background service](https://github.com/ezzedineozone/Sync-Service) that handles the business logic. communicating with each other over the local network on a TCP socket. this ensure that (if needed) you can easily build a web UI for it.

# Goal features of the project
- unidirectional or bidirectional file syncing
- Local and or cloud through different cloud services like google drive, mega or one drive
- Lightweight and customizable
- Background service that is indepdendent from the UI that runs in the background, but is controlled through said UI


# Sync Service
## Completed features:
- Connect to the UI succesfully
- Succesfully recieve instructions of adding and removing sync tasks from the UI
- remove and add sync tasks through console commands (type ? for help)
- setup the environment and the required files for the service, with ease of use reset command if something breaks (helpful while its still in early alpha)

## Upcoming features:
- Actual sync logic, the service is now fully ready for in terms of configuration by the user, all thats required is implemented the sync logic
- Handle sudden connection closes from the UI better
- some bug fixes for command outputs
- send all logs to the UI so that the UI displays them in a log window

### Build Instructions BELOW
>>>>>>> 4f333a3f35a9f1658726b1b756a8295edc1b1b75

To compile this application, ensure all dependencies are correctly included in your project. Verify that `#include` statements for each dependency are valid.

### Dependencies

The following libraries are required:

- **SQLite3**: Database support
- **nlohmann JSON**: JSON parsing and handling
- **ASIO (non-Boost)**: Asynchronous I/O for network operations

### Default Paths

The application expects the dependencies to be located in the following paths:

- **SQLite3**:  
  - `Sync Service/dependencies/sqlite3/`  
  - Place `.h` and `.c` files here.

- **ASIO**:  
  - `Sync Service/dependencies/asio/`  
  - Place `asio.hpp` in this directory, with the `asio` folder.

- **nlohmann JSON**:  
  - `Sync Service/dependencies/json/`  
  - Place `json.hpp` in this directory.

