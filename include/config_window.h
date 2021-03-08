#pragma once

#include <memory>
#include <string>
#include <google/protobuf/message.h>

class ConfigWindowPrivate;
class ConfigWindow {
 public:
    ConfigWindow(const std::string &title = "Config");
    ~ConfigWindow();

    bool Initialize();
    bool Draw(google::protobuf::Message &msg);
 private:
    bool StartDraw();
    void EndDraw();
    void Uninit();
    void BuildGui(google::protobuf::Message &msg);
    ConfigWindow(const ConfigWindow&) = delete;
    ConfigWindow &operator=(const ConfigWindow&) = delete;
 private:
    ConfigWindowPrivate *p_{nullptr};
    std::string title_;
    bool auto_resize_{true};
};
