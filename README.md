Sync Service
Build Instructions

To compile this application, ensure all dependencies are correctly included in your project. Verify that #include statements for each dependency are valid.
Dependencies

The following libraries are required:

    SQLite3: Database support
    nlohmann JSON: JSON parsing and handling
    ASIO (non-Boost): Asynchronous I/O for network operations

Default Paths

The application expects the dependencies to be located in the following paths:

    SQLite3:
        Sync Service/dependencies/sqlite3/
        Place .h and .c files here.

    ASIO:
        Sync Service/dependencies/asio/
        Place asio.hpp in this directory, with the asio folder.

    nlohmann JSON:
        Sync Service/dependencies/json/
        Place json.hpp in this directory.

