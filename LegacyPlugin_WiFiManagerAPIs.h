/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2023 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#pragma once

#include "Module.h"
#include "NetworkManagerTimer.h"

/*! Error code: A recoverable, unexpected error occurred,
 * as defined by one of the following values */
typedef enum _WiFiErrorCode_t {
    WIFI_SSID_CHANGED,              /**< The SSID of the network changed */
    WIFI_CONNECTION_LOST,           /**< The connection to the network was lost */
    WIFI_CONNECTION_FAILED,         /**< The connection failed for an unknown reason */
    WIFI_CONNECTION_INTERRUPTED,    /**< The connection was interrupted */
    WIFI_INVALID_CREDENTIALS,       /**< The connection failed due to invalid credentials */
    WIFI_NO_SSID,                   /**< The SSID does not exist */
    WIFI_UNKNOWN,                   /**< Any other error */
    WIFI_AUTH_FAILED                /**< The connection failed due to auth failure */
} WiFiErrorCode_t;

#define SSID_SIZE                   32
#define WIFI_MAX_PASSWORD_LEN       64

/**
 * @brief WIFI API return status
 *
 */
typedef enum _WIFI_API_RESULT {
    WIFI_API_RESULT_SUCCESS = 0,                  ///< operation is successful
    WIFI_API_RESULT_FAILED,                       ///< Operation general error. This enum is deprecated
    WIFI_API_RESULT_NULL_PARAM,                   ///< NULL argument is passed to the module
    WIFI_API_RESULT_INVALID_PARAM,                ///< Invalid argument is passed to the module
    WIFI_API_RESULT_NOT_INITIALIZED,              ///< module not initialized
    WIFI_API_RESULT_OPERATION_NOT_SUPPORTED,      ///< operation not supported in the specific platform
    WIFI_API_RESULT_READ_WRITE_FAILED,            ///< flash read/write failed or crc check failed
    WIFI_API_RESULT_MAX                           ///< Out of range - required to be the last item of the enum
} WIFI_API_RESULT;
/**
 * @brief WIFI credentials data struct
 *
 */
typedef struct {
    char cSSID[SSID_SIZE+1];                      ///< SSID field.
    char cPassword[WIFI_MAX_PASSWORD_LEN+1];      ///< password field
    int  iSecurityMode;                           ///< security mode. Platform dependent and caller is responsible to validate it
} WIFI_DATA;

typedef enum _WifiRequestType {
    WIFI_GET_CREDENTIALS = 0,
    WIFI_SET_CREDENTIALS = 1
} WifiRequestType_t;

typedef struct _IARM_BUS_MFRLIB_API_WIFI_Credentials_Param_t {
    WIFI_DATA wifiCredentials;
    WifiRequestType_t requestType;
    WIFI_API_RESULT returnVal;
} IARM_BUS_MFRLIB_API_WIFI_Credentials_Param_t;

namespace WPEFramework {

    namespace Plugin {
        // This is a server for a JSONRPC communication channel.
        // For a plugin to be capable to handle JSONRPC, inherit from PluginHost::JSONRPC.
        // By inheriting from this class, the plugin realizes the interface PluginHost::IDispatcher.
        // This realization of this interface implements, by default, the following methods on this plugin
        // - exists
        // - register
        // - unregister
        // Any other methood to be handled by this plugin  can be added can be added by using the
        // templated methods Register on the PluginHost::JSONRPC class.
        // As the registration/unregistration of notifications is realized by the class PluginHost::JSONRPC,
        // this class exposes a public method called, Notify(), using this methods, all subscribed clients
        // will receive a JSONRPC message as a notification, in case this method is called.
        class WiFiManager : public PluginHost::IPlugin, public PluginHost::JSONRPC {
        public:
            WiFiManager();
            ~WiFiManager();
            WiFiManager(const WiFiManager&) = delete;
            WiFiManager& operator=(const WiFiManager&) = delete;

            //Begin methods
            uint32_t getCurrentState(const JsonObject& parameters, JsonObject& response);
            uint32_t startScan(const JsonObject& parameters, JsonObject& response);
            uint32_t stopScan(const JsonObject& parameters, JsonObject& response);
            uint32_t getConnectedSSID(const JsonObject& parameters, JsonObject& response);
            uint32_t setEnabled(const JsonObject& parameters, JsonObject& response);
            uint32_t connect(const JsonObject& parameters, JsonObject& response);
            uint32_t disconnect(const JsonObject& parameters, JsonObject& response);
            uint32_t initiateWPSPairing(const JsonObject& parameters, JsonObject& response);
            uint32_t cancelWPSPairing(const JsonObject& parameters, JsonObject& response);
            uint32_t saveSSID(const JsonObject& parameters, JsonObject& response);
            uint32_t clearSSID(const JsonObject& parameters, JsonObject& response);
            uint32_t getPairedSSID(const JsonObject& parameters, JsonObject& response);
            uint32_t getPairedSSIDInfo(const JsonObject& parameters, JsonObject& response);
            uint32_t isPaired(const JsonObject& parameters, JsonObject& response);
            uint32_t getSupportedSecurityModes(const JsonObject& parameters, JsonObject& response);
#ifdef ENABLE_GET_WIFI_CREDENTIALS
            uint32_t retrieveSSID(const JsonObject& parameters, JsonObject& response);
#endif
            //End methods

            //Begin events
            static void onWiFiStateChange(const JsonObject& parameters);
            static void onAvailableSSIDs(const JsonObject& parameters);
            static void onWiFiSignalStrengthChange(const JsonObject& parameters);

            //End events

            //Build QueryInterface implementation, specifying all possible interfaces to be returned.
            BEGIN_INTERFACE_MAP(WiFiManager)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(PluginHost::IDispatcher)
            END_INTERFACE_MAP

            //IPlugin methods
            virtual const string Initialize(PluginHost::IShell* service) override;
            virtual void Deinitialize(PluginHost::IShell* service) override;
            virtual string Information() const override;

        private:
            void registerLegacyMethods(void);
            void unregisterLegacyMethods(void);
            void subscribeToEvents(void);
            static std::string getInterfaceMapping(const std::string &interface);
            static bool ErrorCodeMapping(const uint32_t ipvalue , uint32_t &opvalue);
            void activatePrimaryPlugin();

        private:
            PluginHost::IShell* m_service;
            std::shared_ptr<WPEFramework::JSONRPC::SmartLinkType<WPEFramework::Core::JSON::IElement>> m_networkmanager;
            NetworkManagerTimer m_timer;
            bool m_subsWiFiStateChange;
            bool m_subsAvailableSSIDs;
            bool m_subsWiFiStrengthChange;
        };
    } // namespace Plugin
} // namespace WPEFramework
