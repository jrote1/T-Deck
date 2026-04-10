#pragma once

#include <MyMesh.h>
#include <vector>

typedef std::function<void(const mesh::GroupChannel &channel, mesh::Packet *pkt, uint32_t timestamp, const char *text)> channel_message_received_handler;

class MeshClient : public MyMesh {
  fs::FS* _fs;
  channel_message_received_handler _channel_message_handler;
//radio, mesh::RNG &rng, mesh::RTCClock &rtc, SimpleMeshTables &tables, DataStore& store, AbstractUITask* ui=NULL
public:
  MeshClient(mesh::Radio &radio, mesh::RNG &rng, mesh::RTCClock &rtc, SimpleMeshTables &tables, DataStore& store)
     : MyMesh(radio, rng, rtc, tables, store)
  {
  }

  void registerChannelMessageHandler(channel_message_received_handler handler) {
    _channel_message_handler = handler;
  }

  void begin(fs::FS& fs);
  void onChannelMessageRecv(const mesh::GroupChannel &channel, mesh::Packet *pkt, uint32_t timestamp, const char *text);

  std::vector<ChannelDetails> getChannels() {
    int idx = 0;

    ChannelDetails dest;
    std::vector<ChannelDetails> channels;

    while(this->getChannel(idx, dest)) {
      channels.push_back(dest);
      idx++;
    }

    return channels;
  }
};