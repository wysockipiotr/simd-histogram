#include "LogWidget.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QDateTime>


LogWidget::LogWidget(QWidget * parent)
	: QWidget(parent) {
	m_log_text_edit = new QTextEdit{ this };
	m_clear_btn = new QPushButton{ tr("Clear"), this };

	auto layout { new QVBoxLayout{ this } };
	layout->addWidget(m_log_text_edit);
	layout->addWidget(m_clear_btn, 1);
	m_log_text_edit->setReadOnly(true);
	m_clear_btn->setDisabled(true);

	connect(m_clear_btn, &QPushButton::pressed, [this] {
		this->m_log_text_edit->clear();
		m_clear_btn->setDisabled(true);
	});
}

void LogWidget::print_message(QStringView message) const {
	print_message(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"), message);
}

void LogWidget::print_message(QStringView tag, QStringView message) const {
	m_log_text_edit->append(QString { "<b>" } + tag.toString() + QString { "</b> " } + message.toString());
	m_clear_btn->setEnabled(true);
}

void LogWidget::appendHtml(QStringView html) const {
	m_log_text_edit->append(html.toString());
}

void LogWidget::clear() const {
	m_log_text_edit->clear();
}
