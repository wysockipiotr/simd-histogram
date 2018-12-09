#pragma once
#include <QStatusBar>

struct IMessageProvider {
	virtual void show_message(QStringView message, int timeout) = 0;
	virtual ~IMessageProvider() {}
};

using MessageProvider = std::shared_ptr<IMessageProvider>;

class StatusBarMessageProvider : public IMessageProvider {
public:
	StatusBarMessageProvider(QStatusBar* status_bar) : status_bar(status_bar) { }

	void show_message(QStringView message, const int timeout = 0) override {
		status_bar->showMessage(message.toString(), timeout);
	}

private:
	QStatusBar* status_bar;
};
