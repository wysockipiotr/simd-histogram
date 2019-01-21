#pragma once
#include <QWidget>

class LogWidget : public QWidget {
Q_OBJECT
public:
	explicit LogWidget(QWidget * parent = nullptr);

public slots:
	void print_message(QStringView message) const;

	void print_message(QStringView tag, QStringView message) const;

	void appendHtml(QStringView html) const;

	void clear() const;

private:
	class QTextEdit * m_log_text_edit {};
	class QPushButton * m_clear_btn {};
};
