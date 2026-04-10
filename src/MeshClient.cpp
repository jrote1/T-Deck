#include "MeshClient.h"

void MeshClient::begin(fs::FS& fs) {
    _fs = &fs;

    MyMesh::begin(false);
}


void MeshClient::onChannelMessageRecv(const mesh::GroupChannel& channel, mesh::Packet* pkt, uint32_t timestamp, const char *text) {
    if(_channel_message_handler != nullptr) {
        _channel_message_handler(channel, pkt, timestamp, text);
    }
    MyMesh::onChannelMessageRecv(channel, pkt, timestamp, text);
    // Handle channel messages
}

