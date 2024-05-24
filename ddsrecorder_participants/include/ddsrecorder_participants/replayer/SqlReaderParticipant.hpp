// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <map>
#include <set>
#include <string>

#include <sqlite3.h>

#include <cpp_utils/memory/Heritable.hpp>

#include <ddspipe_core/efficiency/payload/PayloadPool.hpp>
#include <ddspipe_core/types/topic/dds/DdsTopic.hpp>
#include <ddspipe_core/types/topic/dds/DistributedTopic.hpp>

#include <ddsrecorder_participants/library/library_dll.h>
#include <ddsrecorder_participants/replayer/BaseReaderParticipant.hpp>
#include <ddsrecorder_participants/replayer/McapReaderParticipantConfiguration.hpp>

#if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13
    #include <ddsrecorder_participants/common/types/dynamic_types_collection/v1/DynamicTypesCollection.hpp>
#else
    #include <ddsrecorder_participants/common/types/dynamic_types_collection/v2/DynamicTypesCollection.hpp>
#endif // if FASTRTPS_VERSION_MAJOR <= 2 && FASTRTPS_VERSION_MINOR < 13

namespace eprosima {
namespace ddsrecorder {
namespace participants {

/**
 * Participant that reads MCAP files and passes its messages to other DDS Pipe participants.
 *
 * @implements BaseReaderParticipant
 */
class SqlReaderParticipant : public BaseReaderParticipant
{
public:

    /**
     * SqlReaderParticipant constructor by required values.
     *
     * Creates SqlReaderParticipant instance with given configuration, payload pool and input file path.
     *
     * @param config:       Structure encapsulating all configuration options.
     * @param payload_pool: Owner of every payload contained in sent messages.
     * @param file_path:    Path to the MCAP file with the messages to be read and sent.
     */
    DDSRECORDER_PARTICIPANTS_DllAPI
    SqlReaderParticipant(
            const std::shared_ptr<McapReaderParticipantConfiguration>& configuration,
            const std::shared_ptr<ddspipe::core::PayloadPool>& payload_pool,
            const std::string& file_path);

    /**
     * @brief SqlReaderParticipant destructor.
     */
    DDSRECORDER_PARTICIPANTS_DllAPI
    ~SqlReaderParticipant();

    /**
     * @brief Process the topics and the types stored in the SQLite database.
     *
     * @param topics: Set of topics to be filled with the information from the SQLite database.
     * @param types:  DynamicTypesCollection instance to be filled with the types information from the SQLite database.
     */
    DDSRECORDER_PARTICIPANTS_DllAPI
    void process_summary(
        std::set<utils::Heritable<ddspipe::core::types::DdsTopic>>& topics,
        DynamicTypesCollection& types) override;

    /**
     * @brief Process the messages stored in the SQLite database.
     *
     * Reads and sends messages sequentially (according to timestamp).
     */
    DDSRECORDER_PARTICIPANTS_DllAPI
    void process_messages() override;

protected:

    /**
     * @brief Open the SQLite file.
     */
    void open_file_();

    /**
     * @brief Close the SQLite file.
     */
    void close_file_();

    /**
     * @brief Execute a SQL statement.
     *
     * @param statement:     SQL statement to be executed.
     * @param process_row:   Function to be called for each row of the result.
     */
    void exec_sql_statement_(
        const std::string& statement,
        const std::function<void(sqlite3_stmt*)>& process_row);

    /**
     * @brief Get the \c DdsTopic for \c topic_name.
     *
     * If the topic is in the cache, return it.
     * If not, find the topic's type in the database, create a topic, and store it in the cache.
     *
     * @param topic_name: Name of the topic to be found.
     * @return \c DdsTopic instance for \c topic_name.
     */
    ddspipe::core::types::DdsTopic find_topic_(
            const std::string& topic_name);

    /**
     * @brief Find the type of the topic with name \c topic_name.
     *
     * @param topic_name: Name of the topic to find its type.
     * @return Type of the topic with name \c topic_name.
     */
    std::string find_type_of_topic_(
            const std::string& topic_name);

    // Database
    sqlite3* database_;

    // Topics cache
    std::map<std::string, ddspipe::core::types::DdsTopic> topics_;
};

} /* namespace participants */
} /* namespace ddsrecorder */
} /* namespace eprosima */