Import("env")

# Add .o files
env.Append(
    LINKFLAGS=[
        "${PROJECT_DIR}/src/network/APManager.cpp.o",
        "${PROJECT_DIR}/src/network/WebServerManager.cpp.o",
        "${PROJECT_DIR}/src/network/WebSocketManager.cpp.o",
        "${PROJECT_DIR}/src/sensor/HeartRate.cpp.o",
        "${PROJECT_DIR}/src/sensor/SensorManager.cpp.o",
        "${PROJECT_DIR}/src/sensor/SpO2.cpp.o"
    ]
)