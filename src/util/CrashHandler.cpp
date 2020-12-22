#include "CrashHandler.h"

void CrashHandler::onLogContainFailure() {
    std::cerr << "Application detected an issue during his execution. See logs for more details." << std::endl;
}

void CrashHandler::onException(const std::exception& e) {
    urchin::Logger::instance()->logError("Error occurred: " + std::string(e.what()));
    std::cerr << "Application has been stopped in an unexpected way (exception). See logs for more details." << std::endl;
}

void CrashHandler::onSignalReceived(unsigned long signalId) {
    urchin::Logger::instance()->logError("Signal occurred: " + std::to_string(signalId));
    std::cerr << "Application has been stopped in an unexpected way (signal). See logs for more details." << std::endl;
}
