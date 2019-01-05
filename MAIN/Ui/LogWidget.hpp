#pragma once

#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QPushButton>

class LogWidget : public QWidget {
Q_OBJECT
public:
	explicit LogWidget(QWidget* parent = nullptr) : QWidget(parent) {
		log_text_edit = new QTextEdit{this};
		m_clear_btn = new QPushButton{tr("Clear"), this};

		auto layout{new QVBoxLayout{this}};
		layout->addWidget(log_text_edit);
		layout->addWidget(m_clear_btn, 1);
		log_text_edit->setReadOnly(true);
		m_clear_btn->setDisabled(true);

		connect(m_clear_btn, &QPushButton::pressed, [=] {
			this->log_text_edit->clear();
			m_clear_btn->setDisabled(true);
		});
	}

public slots:
	void print_message(QStringView message) {
		print_message(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"),
		              message);
	}

	void print_message(QStringView tag, QStringView message) {
		log_text_edit->append(QString{"<b>"} + tag.toString() + QString{"</b> "} + message.toString());
		m_clear_btn->setEnabled(true);
	}

	void appendHtml(QStringView html) { log_text_edit->append(html.toString()); }

	void clear() { log_text_edit->clear(); }

private:
	QTextEdit* log_text_edit{};
	QPushButton* m_clear_btn{};
};
